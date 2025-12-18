#include "Wallet.h"
#include <iostream>
#include <fstream>
#include "CSVReader.h"
#include <iomanip>

Wallet::Wallet()
{
    username = 0;
}

void Wallet::insertCurrency(std::string type, double amount)
{
    double balance;
    if (amount < 0)
    {
        throw std::exception{};
    }
    if (currencies.count(type) == 0) // not there yet
    {
        balance = 0;
    }
    else
    { // is there
        balance = currencies[type];
    }
    balance += amount;
    currencies[type] = balance;
}

bool Wallet::removeCurrency(std::string type, double amount)
{
    if (amount < 0)
    {
        return false;
    }
    if (currencies.count(type) == 0) // not there yet
    {
        return false;
    }
    else
    {                                       // is there - do  we have enough
        if (containsCurrency(type, amount)) // we have enough
        {
            currencies[type] -= amount;
            return true;
        }
        else // they have it but not enough.
            return false;
    }
}

bool Wallet::containsCurrency(std::string type, double amount)
{
    if (currencies.count(type) == 0) // not there yet
        return false;
    else
        return currencies[type] >= amount;
}

void Wallet::linkToUser(long long username)
{
    this->username = username;
    std::cout << "Wallet linked to user: " << username << '\n';

    // Load the user's wallet
    loadWallet();
}

std::string Wallet::getFilename()
{
    return "wallets/wallet_" + std::to_string(username) + ".csv";
}

void Wallet::saveWallet()
{
    if (username == 0)
    {
        std::cout << "No user linked to wallet\n";
        return;
    }

    std::ofstream file(getFilename());

    if (!file.is_open())
    {
        std::cerr << "Could not save wallet\n";
        return;
    }

    // save currency,amount on each line
    for (const auto &pair : currencies)
    {
        file << pair.first << "," << pair.second << "\n";
    }

    file.close();
    std::cout << "Wallet saved.\n";
}

void Wallet::loadWallet()
{
    if (username == 0)
    {
        std::cout << "No user linked to wallet\n";
        return;
    }

    std::ifstream file(getFilename());

    if (!file.is_open())
    {
        // First time user
        std::cout << "New wallet created.\n";
        insertCurrency("BTC", 10.0);
        insertCurrency("USDT", 100000.0);
        insertCurrency("ETH", 50.0);
        insertCurrency("DOGE", 100000.0);
        saveWallet();
        return;
    }

    // Read the wallet file
    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> tokens = CSVReader::tokenise(line, ',');
        if (tokens.size() == 2)
        {
            try
            {
                std::string currency = tokens[0];
                double amount = std::stod(tokens[1]);
                currencies[currency] = amount;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error reading wallet line: " << line << "\n";
            }
        }
    }

    file.close();
    std::cout << "Wallet loaded.\n";
}

void Wallet::logTransactions(std::string currentTime, std::string type, std::string product, double price, double amount)
{

    std::ofstream file(getTransactionFilename(), std::ios::app);

    if (!file.is_open())
        return;

    file << currentTime << "," << type << "," << product << "," << price << "," << amount << '\n';

    file.close();
}

std::string Wallet::getTransactionFilename()
{
    return "wallets/transactions_" + std::to_string(username) + ".csv";
}

void Wallet::printRecentTransactions(int n)
{
    std::ifstream file(getTransactionFilename());
    if (!file.is_open())
        return;

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line))
        lines.push_back(line);

    int start = std::max(0, (int)lines.size() - n);

    // Print table header
    std::cout << std::left
              << std::setw(22) << "Timestamp"
              << std::setw(12) << "Type"
              << std::setw(15) << "Product"
              << std::setw(10) << "Price"
              << std::setw(10) << "Amount"
              << '\n';
    std::cout << std::string(67, '-') << '\n'; // separator

    for (int i = start; i < lines.size(); ++i)
    {
        std::stringstream ss(lines[i]);
        std::string timestamp, type, product, priceStr, amountStr;

        // Assuming CSV format: timestamp,type,product,price,amount
        std::getline(ss, timestamp, ',');
        std::getline(ss, type, ',');
        std::getline(ss, product, ',');
        std::getline(ss, priceStr, ',');
        std::getline(ss, amountStr, ',');

        std::cout << std::left
                  << std::setw(22) << timestamp.substr(0, 19)
                  << std::setw(12) << type
                  << std::setw(15) << product
                  << std::setw(10) << priceStr
                  << std::setw(10) << amountStr
                  << '\n';
    }
}

std::string Wallet::toString()
{
    std::string s;
    for (std::pair<std::string, double> pair : currencies)
    {
        std::string currency = pair.first;
        double amount = pair.second;
        s += currency + " : " + std::to_string(amount) + "\n";
    }
    return s;
}

bool Wallet::canFulfillOrder(OrderBookEntry order)
{
    std::vector<std::string> currs = CSVReader::tokenise(order.product, '/');
    // ask
    if (order.orderType == OrderBookType::ask)
    {
        double amount = order.amount;
        std::string currency = currs[0];
        std::cout << "Wallet::canFulfillOrder " << currency << " : " << amount << std::endl;

        return containsCurrency(currency, amount);
    }
    // bid
    if (order.orderType == OrderBookType::bid)
    {
        double amount = order.amount * order.price;
        std::string currency = currs[1];
        std::cout << "Wallet::canFulfillOrder " << currency << " : " << amount << std::endl;
        return containsCurrency(currency, amount);
    }

    return false;
}

void Wallet::processSale(OrderBookEntry &sale)
{
    std::vector<std::string> currs = CSVReader::tokenise(sale.product, '/');
    // ask
    if (sale.orderType == OrderBookType::asksale)
    {
        double outgoingAmount = sale.amount;
        std::string outgoingCurrency = currs[0];
        double incomingAmount = sale.amount * sale.price;
        std::string incomingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;

        // Save after each trade
        saveWallet();
    }
    // bid
    if (sale.orderType == OrderBookType::bidsale)
    {
        double incomingAmount = sale.amount;
        std::string incomingCurrency = currs[0];
        double outgoingAmount = sale.amount * sale.price;
        std::string outgoingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;

        // Save after each trade
        saveWallet();
    }

    logTransactions(sale.timestamp, sale.orderType == OrderBookType::asksale ? "Ask_sale" : "Bid_sale", sale.product, sale.price, sale.amount);
}

std::ostream &operator<<(std::ostream &os, Wallet &wallet)
{
    os << wallet.toString();
    return os;
}