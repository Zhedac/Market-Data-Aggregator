#include <iostream>
#include <string>
#include <filesystem>
#include "data_processor/processor.hpp"
#include "file_handler/reader.hpp"
#include "utils/memory_monitor.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input_directory> <output_file>" << endl;
        return 1;
    }

    string inputDirectory = argv[1];
    string outputFilePath = argv[2];
    // cout<<argc<<endl;
    int inputLimitTest = argc == 4 ? stoi(argv[3]) : 0;
    // cout<<inputLimitTest<<endl;

    if (!filesystem::exists(inputDirectory)) {
        cerr << "input directory does not exist: " << inputDirectory << endl;
        return 1;
    }
    if (!filesystem::is_directory(inputDirectory)) {
        cerr << "input path is not a directory: " << inputDirectory << endl;
        return 1;
    }

    size_t availableMemory = getAvailableSystemMemory();
    cout << "Available memory: " << availableMemory/1e6 << " Megabytes" << endl;

    Processor processor; 

    if (!processor.processAndMerge(inputDirectory, outputFilePath, inputLimitTest)) {
        cerr << "Error processing market data." << endl;
        return 1;
    }

    cout << "Market data processing completed successfully." << endl;
    return 0;
}