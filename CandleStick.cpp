#include "CandleStick.h"
#include "OrderBookEntry.h"

CandleStick::CandleStick(std::string timestamp, std::string product, OrderBookType type, double firstPrice)
    : timestamp(timestamp),
      product(product), type(type), firstPrice(firstPrice), open(firstPrice), high(firstPrice), low(firstPrice), close(firstPrice)
{
}

void CandleStick::addPrice(double price)
{
    if (price > high)
    {
        high = price;
    }

    if (price < low)
    {
        low = price;
    }

    close = price;
}