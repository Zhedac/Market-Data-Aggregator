#include "writer.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

Writer::Writer(const string& outputFile, bool writeHeader) : outputStream(outputFile) {
    if (!outputStream.is_open()) {
        throw runtime_error("Could not open output file: " + outputFile);
    }
    if (writeHeader) {
        outputStream << "Symbol,Timestamp,Price,Size,Exchange,Type\n";
    }
}

Writer::~Writer() {
    if (outputStream.is_open()) {
        outputStream.close();
    }
}

void Writer::writeData(const vector<MarketData>& marketDataEntries) {
    for (const auto& data : marketDataEntries) {
        outputStream << data.getSymbol() << ","
                    << data.getTimestamp() << ","
                    << fixed << setprecision(6) << data.getPrice() << ","
                    << data.getSize() << ","
                    << data.getExchange() << ","
                    << data.getType() << "\n";
    }
    outputStream.flush();
}

void Writer::writeData(const MarketData& data){
    outputStream << data.getSymbol() << ","
                    << data.getTimestamp() << ","
                    << fixed << setprecision(6) << data.getPrice() << ","
                    << data.getSize() << ","
                    << data.getExchange() << ","
                    << data.getType() << "\n";
    outputStream.flush();
}
