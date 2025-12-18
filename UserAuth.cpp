#include "UserAuth.h"
#include "CSVReader.h"
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <optional>

UserAuth::UserAuth()
{
}

std::optional<User> UserAuth::registerUser()
{
    std::string fullName;
    std::cout << "Enter your full name: ";
    std::getline(std::cin, fullName);

    std::string email;
    std::cout << "Enter your email: ";
    std::getline(std::cin, email);

    std::string password;
    std::cout << "Enter your password: ";
    std::getline(std::cin, password);

    if (userExists(readCredentials("Credentials.csv"), fullName, email))
    {
        std::cout << "User already exists, please login" << '\n';
        return std::nullopt;
    }

    long long hashedPassword = hashPassword(password);
    long long username = generateUsername();

    std::ofstream outputFile;

    outputFile.open("Credentials.csv", std::ios::out | std::ios::app);

    if (!outputFile.is_open())
    {
        std::cerr << "Could not open file" << '\n';
        return std::nullopt;
    };

    outputFile << fullName << "," << email << "," << username << "," << hashedPassword << '\n';
    outputFile.close();

    std::cout << "CSV file 'Credentials.csv' created successfully." << std::endl;
    std::cout << "Your Username: " << username << std::endl;

    User newUser{fullName, email, username, hashedPassword};
    return newUser;
}

std::optional<User> UserAuth::loginUser()
{

    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    std::string password;
    std::cout << "Enter your password: ";
    std::getline(std::cin, password);

    long long hashedPassword{hashPassword(password)};

    std::vector<User> user{readCredentials("Credentials.csv")};

    try
    {
        long long _username = std::stoll(username);

        for (const User &u : user)
        {
            if (_username == u.getUsername() && hashedPassword == u.getPassword())
            {
                std::cout << "Logged in as " << u.getName() << '\n';
                return u;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Invalid username" << '\n';
        return std::nullopt;
    }

    std::cout << "Incorrect username or password" << '\n';
    return std::nullopt;
}

bool UserAuth::resetPassword()
{
    std::cout << "Reset Password" << '\n';
    std::cout << "============== " << '\n';

    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    try
    {
        long long _username = std::stoll(username);
        std::vector<User> user{readCredentials("Credentials.csv")};
        bool found{false};

        for (auto &u : user)
        {
            if (_username == u.getUsername())
            {
                std::string newPassword{};
                std::cout << "Type in a new password: " << '\n';
                std::getline(std::cin, newPassword);

                u.setPassword(hashPassword(newPassword));
                found = true;
                break;
            }
        }

        if (found)
        {
            writeCredentials("Credentials.csv", user);
            std::cout << "Password reset successful" << '\n';
            return true;
        }
        else
        {
            std::cout << "Username not found" << '\n';
            return false;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Invalid Username" << '\n';
        return false;
    }
}

void UserAuth::writeCredentials(const std::string &filename, const std::vector<User> &users)
{
    std::ofstream file(filename);

    for (const auto &u : users)
    {
        file << u.getName() << "," << u.getEmail() << "," << u.getUsername() << "," << u.getPassword() << '\n';
    }
}

bool UserAuth::userExists(std::vector<User> const &user, std::string const &name, std::string const &email)
{

    for (const User &u : user)
    {
        if (name == u.getName() && email == u.getEmail())
        {
            std::cout << "User already exists, please login" << '\n';
            return 1;
        }
    }

    return 0;
}

std::vector<User> UserAuth::readCredentials(std::string csvFilename)
{
    std::ifstream inputFile{csvFilename};
    std::vector<User> users;
    std::string line;

    if (!inputFile.is_open())
        return users;

    while (std::getline(inputFile, line))
    {
        if (line.empty())
            continue;

        auto tokens = CSVReader::tokenise(line, ',');

        if (tokens.size() != 4)
        {
            std::cout << "Skipping malformed line: " << '\n';
            continue;
        }

        try
        {
            users.push_back(stringtoUser(tokens));
        }
        catch (...)
        {
            std::cout << "Skipping corrupt credential entry" << '\n';
        }
    }

    std::cout << "Read: " << users.size() << " entries" << '\n';
    return users;
}

User UserAuth::stringtoUser(std::vector<std::string> tokens)
{
    try
    {
        long long username = std::stoll(tokens[2]);
        long long hashedPassword = std::stoll(tokens[3]);
        return User{tokens[0], tokens[1], username, hashedPassword};
    }
    catch (...)
    {
        throw;
    }
}

long long UserAuth::hashPassword(std::string password)
{
    std::hash<std::string> p;
    return p(password);
}

long long UserAuth::generateUsername()
{
    const long long MIN_VAL = 1000000000ll;
    const long long MAX_VAL = 9999999999ll;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::mt19937 gen(seed);

    std::uniform_int_distribution<long long> dis(MIN_VAL, MAX_VAL);

    long long randomUsername = dis(gen);

    return randomUsername;
}
