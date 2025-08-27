#include "reader.hpp"
#include <iostream>
#include <stdexcept>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

vector<string> Reader::getAllTradeFilePaths(const string& directoryPath, const int& inputLimitTest) {
    vector<string> filePaths;
    try {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                filePaths.push_back(entry.path().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
        throw runtime_error("Failed to read directory: " + directoryPath);
    }
    if(inputLimitTest==0 || inputLimitTest>=filePaths.size()){
        return filePaths;
    }
    vector<string> reqFilePaths(filePaths.begin(), filePaths.begin() + inputLimitTest);
    return reqFilePaths;
}