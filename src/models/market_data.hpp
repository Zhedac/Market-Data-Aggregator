#ifndef MARKET_DATA_HPP
#define MARKET_DATA_HPP

#include <string>
using namespace std;

class MarketData {
public:
    MarketData(const string& symbol, const string& timestamp, double price, int size, const string& exchange, const string& type);

    const string& getSymbol() const;
    const string& getTimestamp() const;
    double getPrice() const;
    int getSize() const;
    const string& getExchange() const;
    const string& getType() const;
    bool operator<(const MarketData& other) const;

private:
    string symbol;
    string timestamp;
    double price;
    int size;
    string exchange;
    string type;
};

#endif // MARKET_DATA_HPP