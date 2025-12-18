#pragma once

#include <vector>
#include "CandleStick.h"
#include "OrderBook.h"
#include <map>

class Aggregator
{
public:
    Aggregator(OrderBook &orderbook);

    std::vector<CandleStick> getCandles(std::string product, OrderBookType type, std::string timeframe);

private:
    OrderBook &orderBook;
    std::string timeKey(std::string timetamp, std::string timeframe);
    std::map<std::string, CandleStick> candlesMap;
};
