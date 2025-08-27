#ifndef READER_HPP
#define READER_HPP

#include <string>
#include <vector>
using namespace std;

class Reader {
public:
    Reader() = default;

    static vector<string> getAllTradeFilePaths(const string& directoryPath,const int& inputLimitTest);
};

#endif