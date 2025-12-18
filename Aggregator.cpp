#include "Aggregator.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>

Aggregator::Aggregator(OrderBook &orderbook) : orderBook(orderbook)
{
}

std::string Aggregator::timeKey(std::string timestamp, std::string timeframe)
{
    if (timeframe == "daily")
    {
        return timestamp.substr(0, 10);
    }
    else if (timeframe == "monthly")
    {
        return timestamp.substr(0, 7);
    }
    else if (timeframe == "yearly")
    {
        return timestamp.substr(0, 4);
    }
    else if (timeframe == "minute")
    {
        return timestamp.substr(0, 16);
    }
    else
    {
        return timestamp;
    }
}

std::vector<CandleStick> Aggregator::getCandles(std::string product, OrderBookType type, std::string timeframe)
{
    std::vector<CandleStick> candles;

    const auto &orders = orderBook.getOrders(type, product, "");
    if (orders.empty())
        return candles;

    std::unordered_map<std::string, CandleStick> candleMap;

    for (auto &o : orders)
    {
        std::string timeKey;
        if (timeframe == "minute")
            timeKey = o.timestamp.substr(0, 16);
        else if (timeframe == "hour")
            timeKey = o.timestamp.substr(0, 13);
        else if (timeframe == "daily")
            timeKey = o.timestamp.substr(0, 10);
        else if (timeframe == "monthly")
            timeKey = o.timestamp.substr(0, 7);
        else if (timeframe == "yearly")
            timeKey = o.timestamp.substr(0, 4);
        else
            timeKey = o.timestamp;

        auto it = candleMap.find(timeKey);
        if (it == candleMap.end())
            candleMap.emplace(timeKey, CandleStick(timeKey, product, type, o.price));
        else
            it->second.addPrice(o.price);
    }

    candles.reserve(candleMap.size());
    for (auto &pair : candleMap)
        candles.push_back(pair.second);

    std::sort(candles.begin(), candles.end(), [](const CandleStick &a, const CandleStick &b)
              { return a.getTimestamp() < b.getTimestamp(); });

    return candles;
}