#pragma once
#include <cstring>

struct Users {
    char UserID[31];
    char Password[31];
    char username[31];
    int privilege;

    Users() {
        UserID[0] = '\0';
        Password[0] = '\0';
        username[0] = '\0';
        privilege = 0;
    }

    Users(const char* id, const char* pwd, const char* name, int priv) {
        std::strncpy(UserID, id, 30);
        UserID[30] = '\0';
        std::strncpy(Password, pwd, 30);
        Password[30] = '\0';
        std::strncpy(username, name, 30);
        username[30] = '\0';
        privilege = priv;
    }

    Users& operator=(const Users& other) {
        if (this == &other) return *this;
        std::strncpy(UserID, other.UserID, 30); UserID[30] = '\0';
        std::strncpy(Password, other.Password, 30); Password[30] = '\0';
        std::strncpy(username, other.username, 30); username[30] = '\0';
        privilege = other.privilege;
        return *this;
    }

    bool operator==(const Users& other) const {
        return std::strcmp(UserID, other.UserID) == 0 &&
               std::strcmp(Password, other.Password) == 0 &&
               std::strcmp(username, other.username) == 0 &&
               privilege == other.privilege;
    }

    bool operator<(const Users& other) const {
        return std::strcmp(UserID, other.UserID) < 0;
    }
};

