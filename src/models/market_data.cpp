#include "market_data.hpp"
using namespace std;

MarketData::MarketData(const string& symbol, const string& timestamp, double price, int size, const string& exchange, const string& type)
    : symbol(symbol), timestamp(timestamp), price(price), size(size), exchange(exchange), type(type) {}

const string& MarketData::getSymbol() const {
    return symbol;
}

const string& MarketData::getTimestamp() const {
    return timestamp;
}

double MarketData::getPrice() const {
    return price;
}

int MarketData::getSize() const {
    return size;
}

const string& MarketData::getExchange() const {
    return exchange;
}

const string& MarketData::getType() const {
    return type;
}

bool MarketData::operator<(const MarketData& other) const {
    if (timestamp == other.timestamp) {
        return symbol < other.symbol;
    }
    return timestamp < other.timestamp;
}