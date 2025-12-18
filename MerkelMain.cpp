#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "CSVReader.h"
#include <iomanip>

MerkelMain::MerkelMain()
{
}

void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();

    bool continueRunning{true};

    while (continueRunning)
    {
        loginOrRegister();

        while (true)
        {

            printMenu();
            input = getUserOption();
            if (input == -1)
                break;
            else if (input == 11)
            {
                wallet.saveWallet();
                std::cout << "Logging Out" << '\n';
                break;
            }
            processUserOption(input);
        }
    }

    wallet.saveWallet();
    std::cout << "Thank you!" << '\n';
}

void MerkelMain::printMenu()
{

    std::cout << "============== " << std::endl;
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // print table for candles
    std::cout << "6: Show candles" << std::endl;
    // Deposit Funds
    std::cout << "7: Deposit funds" << std::endl;
    // Withdraw funds
    std::cout << "8: Withdraw funds" << std::endl;
    // View recent transactions
    std::cout << "9: View recent transactions" << std::endl;
    // 6 continue
    std::cout << "10: Continue " << std::endl;
    // Log user out
    std::cout << "11: Logout " << std::endl;

    std::cout << "============== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (std::string const &p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                                                  p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    }
}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else
    {
        try
        {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::ask);
            obe.username = currentUser.value().getUsername();
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else
            {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else
    {
        try
        {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::bid);
            obe.username = currentUser.value().getUsername();

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else
            {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}

void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry &sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << " user " << sale.username << std::endl;
            if (currentUser && sale.username == currentUser.value().getUsername())
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
    }

    currentTime = orderBook.getNextTime(currentTime);
}

int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-8" << std::endl;
    std::getline(std::cin, line);

    if (line == "exit")
    {
        std::cout << "Exiting the program...." << '\n';
        return -1;
    }

    try
    {
        userOption = std::stoi(line);
    }
    catch (const std::exception &e)
    {
        std::cout << "Invalid input, please try again." << '\n';
        return 0;
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::printCandleStickData()
{
    std::string product;
    std::cout << "Enter the product (e.g. ETH/BTC): ";
    std::getline(std::cin, product);

    std::string typeStr;
    std::cout << "Enter order type (ask or bid): ";
    std::getline(std::cin, typeStr);

    OrderBookType type;
    if (typeStr == "ask")
        type = OrderBookType::ask;
    else if (typeStr == "bid")
        type = OrderBookType::bid;
    else
    {
        std::cout << "Invalid order type\n";
        return;
    }

    std::string timeframe;
    std::cout << "Enter timeframe (minute, hour, daily, monthly, yearly): ";
    std::getline(std::cin, timeframe);

    std::vector<CandleStick> candles = aggregator.getCandles(product, type, timeframe);

    if (candles.empty())
    {
        std::cout << "No data found." << '\n';
        return;
    }

    std::cout << std::fixed << std::setprecision(7);

    std::cout << std::setw(20) << "Timestamp" << std::setw(15) << "Open" << std::setw(15) << "High" << std::setw(15) << "Low" << std::setw(15) << "Close" << '\n';

    for (auto &c : candles)
    {
        std::cout << std::setw(20) << c.timestamp << std::setw(15) << c.getOpen() << std::setw(15)
                  << c.getHigh() << std::setw(15) << c.getLow() << std::setw(15) << c.getClose() << '\n';
    }
}

void MerkelMain::loginOrRegister()
{

    while (true)
    {
        std::cout << "============== " << std::endl;
        std::cout << "1: Login " << std::endl;
        std::cout << "2: Register" << std::endl;
        std::cout << "3: Reset Password" << std::endl;
        std::cout << "4: Exit Program" << std::endl;
        std::cout << "============== " << std::endl;

        int option{};
        std::string line{};
        std::getline(std::cin, line);

        try
        {
            option = std::stoi(line);
        }
        catch (const std::exception &e)
        {
            std::cout << "Invalid input, please try again." << '\n';
            continue;
        }

        if (option == 1)
        {
            currentUser = auth.loginUser();
            if (currentUser)
            {
                wallet.linkToUser(currentUser.value().getUsername());
                std::cout << "Logged in successfully as " << currentUser.value().getName() << '\n';
                break;
            }
        }
        else if (option == 2)
        {
            currentUser = auth.registerUser();
            if (currentUser)
            {
                wallet.linkToUser(currentUser.value().getUsername());
                std::cout << "Registration Successful. Logged in as " << currentUser.value().getName() << '\n';
                break;
            }
            else
            {
                std::cout << "Registration failed or user already exists. Try again." << '\n';
            }
        }
        else if (option == 3)
        {
            auth.resetPassword();
        }
        else if (option == 4)
        {
            std::cout << "Exiting Program" << '\n';
            exit(0);
        }
    }
}

void MerkelMain::depositFunds()
{
    std::string currency;
    double amount;

    std::cout << "Currency: ";
    std::getline(std::cin, currency);

    std::cout << "Amount: ";
    std::cin >> amount;
    std::cin.ignore();

    wallet.insertCurrency(currency, amount);
    wallet.logTransactions(currentTime, "Deposit", currency, 0, amount);
    wallet.saveWallet();
}

void MerkelMain::withdrawFunds()
{
    std::string currency;
    double amount;

    std::cout << "Currency: ";
    std::getline(std::cin, currency);

    std::cout << "Amount: ";
    std::cin >> amount;
    std::cin.ignore();

    if (wallet.removeCurrency(currency, amount))
    {
        wallet.logTransactions(currentTime, "Withdraw", currency, 0, amount);
        wallet.saveWallet();
    }
    else
    {
        std::cout << "Insufficient funds" << '\n';
    }
}

void MerkelMain::processUserOption(int userOption)
{
    switch (userOption)
    {
    default:
        std::cout << "Invalid Choice. Choose 1-7" << std::endl;
        break;
    case 1:
        printHelp();
        break;
    case 2:
        printMarketStats();
        break;
    case 3:
        enterAsk();
        break;
    case 4:
        enterBid();
        break;
    case 5:
        printWallet();
        break;
    case 6:
        printCandleStickData();
        break;
    case 7:
        depositFunds();
        break;
    case 8:
        withdrawFunds();
        break;
    case 9:
        wallet.printRecentTransactions(5);
        break;
    case 10:
        gotoNextTimeframe();
        break;
    case 11:
        break;
    }
}