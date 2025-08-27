#include "processor.hpp"
#include "../file_handler/reader.hpp"
#include "../file_handler/writer.hpp"
#include "../models/market_data.hpp"
#include "../utils/memory_monitor.hpp"
#include "../utils/thread_pool.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <atomic>
using namespace std;
namespace fs = std::filesystem;
const double TARGET_AVAILABLE_MEMORY_PERCENTAGE = 0.95; // percentage
const size_t ESTIMATED_MARKET_DATA_ENTRY_SIZE = 128; //in bytes


atomic<long long> global_counter(0);
string generateUniquePathString() {
    auto now = chrono::high_resolution_clock::now();
    long long nanoseconds = chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch()).count();
    stringstream ss;
    ss << nanoseconds << "_" << global_counter.fetch_add(1);
    return ss.str();
}

Processor::Processor() {}

string Processor::getSymbolFromPath(const string& filePath) {
    fs::path p(filePath);
    return p.stem().string();
}

MarketData Processor::parseMarketDataLine(const string& line, const string& symbol) {
    istringstream ss(line);
    string timestamp_str, price_str, size_str, exchange, type;

    if (!getline(ss, timestamp_str, ',') ||
        !getline(ss, price_str, ',') ||
        !getline(ss, size_str, ',') ||
        !getline(ss, exchange, ',') ||
        !getline(ss, type)) {
        throw runtime_error("Malformed market data line: " + line);
    }

    try {
        double price = stod(price_str);
        int size = stoi(size_str);
        return MarketData(symbol, timestamp_str, price, size, exchange, type);
    } catch (const exception& e) {
        throw runtime_error("Error parsing market data values from line: " + line + " - " + e.what());
    }
}

MarketData Processor::parseMarketDataLineAfterMerge(const string& line) {
    istringstream ss(line);
    string symbol,timestamp_str, price_str, size_str, exchange, type;

    if (!getline(ss, symbol, ',') ||
        !getline(ss, timestamp_str, ',') ||
        !getline(ss, price_str, ',') ||
        !getline(ss, size_str, ',') ||
        !getline(ss, exchange, ',') ||
        !getline(ss, type)) {
        throw runtime_error("Malformed market data line: " + line);
    }

    try {
        double price = stod(price_str);
        int size = stoi(size_str);
        return MarketData(symbol, timestamp_str, price, size, exchange, type);
    } catch (const exception& e) {
        throw runtime_error("Error parsing market data values from line: " + line + " - " + e.what());
    }
}

void Processor::cleanupTempFiles(const vector<string>& tempFiles) {
    for (const auto& filePath : tempFiles) {
        try {
            if (fs::exists(filePath)) {
                fs::remove(filePath);
                cout << "Cleaned up temporary file: " << filePath << endl;
            }
        } catch (const fs::filesystem_error& e) {
            cerr << "Error cleaning up temporary file " << filePath << ": " << e.what() << endl;
        }
    }
}

vector<string> Processor::createSortedRuns(const vector<string>& allFilePaths, const string& tempDir) {
    vector<string> sortedRunFilePaths;
    mutex mtx;
    fs::create_directories(tempDir);
    unsigned int numThreads = thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 1;
    size_t availableMemory = getAvailableSystemMemory();
    size_t maxMemoryForProcessing = static_cast<size_t>(availableMemory * TARGET_AVAILABLE_MEMORY_PERCENTAGE);

    size_t maxEntriesPerBatch = (ESTIMATED_MARKET_DATA_ENTRY_SIZE > 0) ? maxMemoryForProcessing / ESTIMATED_MARKET_DATA_ENTRY_SIZE : 1000000;

    if (maxEntriesPerBatch < 10000) {
        maxEntriesPerBatch = 10000;
    }

    ThreadPool pool(numThreads);
    vector<future<void>> futures;

    size_t currentFileIndex = 0;
    while (currentFileIndex < allFilePaths.size()) {
        vector<string> batchFilePaths;
        size_t currentBatchEntries = 0;
        size_t filesInCurrentBatch = 0;

        while (currentFileIndex < allFilePaths.size() && currentBatchEntries < maxEntriesPerBatch && filesInCurrentBatch < 30) {
            batchFilePaths.push_back(allFilePaths[currentFileIndex++]);
            currentBatchEntries += 100000; // rough estimate
            filesInCurrentBatch++;
        }

        if (!batchFilePaths.empty()) {
            futures.push_back(pool.enqueue([&, batchFilePaths, tempDir]() {
                vector<MarketData> batchMarketData;
                batchMarketData.reserve(currentBatchEntries);

                for (const auto& filePath : batchFilePaths) {
                    ifstream file(filePath);
                    if (!file.is_open()) {
                        cerr << "Warning: Could not open input file: " << filePath << endl;
                        continue;
                    }

                    string line;
                    getline(file, line);

                    string symbol = getSymbolFromPath(filePath);

                    while (getline(file, line)) {
                        try {
                            batchMarketData.push_back(parseMarketDataLine(line, symbol));
                        } catch (const exception& e) {
                            cerr << "Error parsing line from " << filePath << ": " << e.what() << endl;
                        }
                    }
                    file.close();
                }

                sort(batchMarketData.begin(), batchMarketData.end());

                string tempRunFilePath = (fs::path(tempDir) / (generateUniquePathString() + ".tmp")).string();
                Writer tempFile(tempRunFilePath);
                tempFile.writeData(batchMarketData);

                lock_guard<mutex> lock(mtx);
                sortedRunFilePaths.push_back(tempRunFilePath);
            }));
        }
    }

    for (auto& future : futures) {
        future.get();
    }

    return sortedRunFilePaths;
}

bool Processor::mergeSortedRuns(const vector<string>& sortedRunPaths, const string& outputFilePath) {
    using Entry = pair<MarketData, int>;
    auto compareEntry = [](const Entry& a, const Entry& b) {
        if (a.first.getTimestamp() == b.first.getTimestamp()) {
            return a.first.getSymbol() > b.first.getSymbol();
        }
        return a.first.getTimestamp() > b.first.getTimestamp();
    };
    priority_queue<Entry, vector<Entry>, decltype(compareEntry)> pq(compareEntry);

    vector<ifstream> inputFiles(sortedRunPaths.size());

    for (size_t i = 0; i < sortedRunPaths.size(); ++i) {
        inputFiles[i].open(sortedRunPaths[i]);
        if (!inputFiles[i].is_open()) {
            cerr << "Error: Could not open sorted run file: " << sortedRunPaths[i] << endl;
            return false;
        }
        
        string line;
        getline(inputFiles[i], line);
        if (getline(inputFiles[i], line)) {
            try {
                pq.push({parseMarketDataLineAfterMerge(line), static_cast<int>(i)});
            } catch (const exception& e) {
                cerr << "Error parsing line from sorted run file " << sortedRunPaths[i] << ": " << e.what() << endl;
            }
        }
    }
    Writer writer(outputFilePath);

    //k-way merge
    while (!pq.empty()) {
        Entry current = pq.top();
        pq.pop();

        writer.writeData(current.first);

        string line;
        if (getline(inputFiles[current.second], line)) {
            try {
                pq.push({parseMarketDataLineAfterMerge(line), current.second});
            } catch (const exception& e) {
                cerr << "Error parsing line from sorted run file " << sortedRunPaths[current.second] << ": " << e.what() << endl;
            }
        }
    }

    for (auto& file : inputFiles) {
        if (file.is_open()) {
            file.close();
        }
    }

    return true;
}

bool Processor::processAndMerge(const string& inputDirectory, const string& outputFilePath, const int& inputLimitTest) {
    vector<string> allFilePaths;
    vector<string> sortedRunPaths;
    fs::path tempDirPath;

    try {
        allFilePaths = Reader::getAllTradeFilePaths(inputDirectory,inputLimitTest);
        if (allFilePaths.empty()) {
            cerr << "No trade files found in directory: " << inputDirectory << endl;
            return false;
        }
        cout << "Found " << allFilePaths.size() << " trade files." << endl;

        tempDirPath = fs::temp_directory_path() / ("market_data_processor_temp_" + generateUniquePathString());
        fs::create_directory(tempDirPath);
        cout << "Created temporary directory: " << tempDirPath << endl;

        sortedRunPaths = createSortedRuns(allFilePaths, tempDirPath.string());
        if (sortedRunPaths.empty()) {
            cerr << "Failed to create any sorted runs.\n";
            if (fs::exists(tempDirPath)) {
                fs::remove_all(tempDirPath);
                cout << "Temp Cleaned up directory: " << tempDirPath << endl;
            }
            return false;
        }
        cout << "temp sorted run files: " << sortedRunPaths.size() << "\n";

        cout << "Starting merge of sorted runs..." << endl;
        bool mergeSuccess = mergeSortedRuns(sortedRunPaths, outputFilePath);
        if (!mergeSuccess) {
            cerr << "Failed to merge sorted runs.\n";
            cleanupTempFiles(sortedRunPaths);
            if (fs::exists(tempDirPath)) {
                fs::remove_all(tempDirPath);
                cout << "Temp Cleaned up directory: " << tempDirPath << endl;
            }
            return false;
        }
        cout << "Merge completed successfully to: " << outputFilePath << endl;

        cleanupTempFiles(sortedRunPaths);
        if (fs::exists(tempDirPath)) {
            fs::remove_all(tempDirPath);
            cout << "Temp Cleaned up directory: " << tempDirPath << endl;
        }

    } catch (const exception& e) {
        cerr << "Error during processing: " << e.what() << endl;
        cleanupTempFiles(sortedRunPaths);
        if (fs::exists(tempDirPath)) {
            try {
                fs::remove_all(tempDirPath);
                cout << "Cleaned up temporary directory after error: " << tempDirPath << endl;
            } catch (const fs::filesystem_error& fe) {
                cerr << "Error cleaning up temporary directory " << tempDirPath << " after error: " << fe.what() << endl;
            }
        }
        return false;
    }

    return true;
}