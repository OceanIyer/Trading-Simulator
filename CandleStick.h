#pragma once

#include <string>
#include "OrderBookEntry.h"

class CandleStick
{
public:
    CandleStick(std::string timestamp, std::string product, OrderBookType type, double firstPrice);

    void addPrice(double price);

    std::string timestamp;
    std::string product;
    OrderBookType type;

    double getOpen() const { return open; }
    double getClose() const { return close; }
    double getHigh() const { return high; }
    double getLow() const { return low; }
    std::string getTimestamp() const { return timestamp; }

private:
    double firstPrice;
    double open;
    double close;
    double high;
    double low;
};