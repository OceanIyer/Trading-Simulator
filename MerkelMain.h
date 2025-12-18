#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "Aggregator.h"
#include "UserAuth.h"
#include <optional>

class MerkelMain
{
public:
    MerkelMain();
    /** Call this to start the sim */
    void init();

private:
    void printMenu();
    void printHelp();
    void printMarketStats();
    void enterAsk();
    void enterBid();
    void printWallet();
    void gotoNextTimeframe();
    int getUserOption();
    void processUserOption(int userOption);
    void printCandleStickData();
    void loginOrRegister();
    void depositFunds();
    void withdrawFunds();

    std::string currentTime;

    OrderBook orderBook{"20200601.csv"};
    Aggregator aggregator{orderBook};
    Wallet wallet;
    UserAuth auth;
    std::optional<User> currentUser;
};
