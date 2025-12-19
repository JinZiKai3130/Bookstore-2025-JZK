#pragma once
#include <cstring>
#include "BlockStorage.hpp"
#include <vector>

struct Users
{
    char UserID[31];
    char Password[31];
    char username[31];
    char selected_book[21];
    int privilege;

    Users()
    {
        UserID[0] = '\0';
        Password[0] = '\0';
        username[0] = '\0';
        selected_book[0] = '\0';
        privilege = 0;
    }

    Users(const char *id, const char *pwd, const char *name, int priv)
    {
        std::strncpy(UserID, id, 30);
        UserID[30] = '\0';
        std::strncpy(Password, pwd, 30);
        Password[30] = '\0';
        std::strncpy(username, name, 30);
        username[30] = '\0';
        selected_book[0] = '\0';
        privilege = priv;
    }

    Users &operator=(const Users &other)
    {
        if (this == &other)
            return *this;
        std::strncpy(UserID, other.UserID, 30);
        UserID[30] = '\0';
        std::strncpy(Password, other.Password, 30);
        Password[30] = '\0';
        std::strncpy(username, other.username, 30);
        username[30] = '\0';
        privilege = other.privilege;
        return *this;
    }

    bool operator==(const Users &other) const
    {
        return std::strcmp(UserID, other.UserID) == 0 &&
               std::strcmp(Password, other.Password) == 0 &&
               std::strcmp(username, other.username) == 0 &&
               privilege == other.privilege;
    }

    bool operator<(const Users &other) const
    {
        return std::strcmp(UserID, other.UserID) < 0;
    }
};

class UserManager
{
private:
    BlockStorageSystem<Users> user_storage;
    std::vector<Users> user_stack;
    // User& find_user(const char* id);
    bool check_priv(const int &priv);
    bool check_id(const char *str);
    bool check_name(const char *str);

public:
    UserManager();
    std::vector<Users> finduser(const char *id);
    void useradd(const char *id, const char *pwd, int priv, const char *name, const int your_priv);
    void userdelete(const char *id, int your_priv);
    std::vector<Users> search_users(const char *id);
    void login(const char *id, const char *pwd = "");
    void logout();
    void delt(const char *id);
    void regist(const char *id, const char *pwd, const char *name);
    void change_pwd(const char *id, const char *new_pwd, int your_priv, const char *pre_pwd = "");
    void select_book(const char *isbn);
    char *get_select();
    Users get_user();
};
