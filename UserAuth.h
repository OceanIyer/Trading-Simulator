#pragma once

#include <vector>
#include <string>
#include "User.h"
#include <optional>

class UserAuth
{
public:
    UserAuth();

    std::optional<User> registerUser();
    std::optional<User> loginUser();
    bool resetPassword();

private:
    long long hashPassword(std::string passwords);
    bool userExists(std::vector<User> const &user, std::string const &name, std::string const &email);
    long long generateUsername();
    User stringtoUser(std::vector<std::string> tokens);
    std::vector<User> readCredentials(std::string csvFilename);
    void writeCredentials(const std::string &filename, const std::vector<User> &users);
};