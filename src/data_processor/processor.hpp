#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <vector>
#include <string>
#include <queue>
#include <memory>
#include <fstream>
#include "../models/market_data.hpp"
#include "../utils/thread_pool.hpp"
using namespace std;


class Processor {
public:
    Processor();
    bool processAndMerge(const string& inputDirectory, const string& outputFilePath, const int &inputLimitTest);

private:
    static MarketData parseMarketDataLine(const string& line, const string& symbol);
    static MarketData parseMarketDataLineAfterMerge(const string& line);
    vector<string> createSortedRuns(const vector<string>& allFilePaths, const string& tempDir);
    bool mergeSortedRuns(const vector<string>& sortedRunPaths, const string& outputFilePath);
    static string getSymbolFromPath(const string& filePath);
    void cleanupTempFiles(const vector<string>& tempFiles);
};

#endif