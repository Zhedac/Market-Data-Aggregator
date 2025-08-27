#ifndef WRITER_HPP
#define WRITER_HPP

#include <fstream>
#include <string>
#include <vector>
#include "../models/market_data.hpp"
using namespace std;

class Writer {
public:
    Writer(const string& outputFile, bool writeHeader = true);
    ~Writer();

    void writeData(const vector<MarketData>& marketDataEntries);
    void writeData(const MarketData& marketDataEntry);

private:
    ofstream outputStream;
};

#endif // WRITER_HPP