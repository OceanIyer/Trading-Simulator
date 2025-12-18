#pragma once

#include <string>
#include <iostream>

class User
{
public:
    User(std::string name, std::string email, long long username, long long passwordHash);

    void setName(std::string name)
    {
        User::name = name;
    };
    void setEmail(std::string email)
    {
        User::email = email;
    };
    void setUsername(long long username)
    {
        User::username = username;
    };
    void setPassword(long long passwordHash)
    {
        User::passwordHash = passwordHash;
    };

    std::string getName() const
    {
        return User::name;
    };
    std::string getEmail() const
    {
        return User::email;
    };
    long long getUsername() const
    {
        return User::username;
    };
    long long getPassword() const
    {
        return User::passwordHash;
    };

    void disp()
    {
        std::cout << "Name: " << name << " Email: " << email << " Username: " << username << " PasswordHash: " << passwordHash << '\n';
    }

private:
    std::string name;
    std::string email;
    long long username;
    long long passwordHash;
};
