#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include "BlockStorage.hpp"
using std::string;
using std::vector;

struct FinanceLog
{
    int count;
    double money;
    bool type;
    std::string isbn;

    FinanceLog() { count = 0, money = 0.00, type = 0, isbn = ""; }

    FinanceLog(const int count, const double money,
               const bool type,
               const string &isbn) : count(count), money(money),
                                     type(type), isbn(isbn) {}

    FinanceLog &operator=(const FinanceLog &other)
    {
        count = other.count;
        money = other.money;
        type = other.type;
        isbn = other.isbn;
        return *this;
    }

    bool operator==(const FinanceLog &other) const
    {
        return count == other.count &&
               money == other.money &&
               type == other.type &&
               isbn == other.isbn;
    }

    bool operator<(const FinanceLog &other) const
    {
        return count < other.count;
    }

    void print()
    {
        if (type)
            std::cout << "+";
        else
            std::cout << "-";
        std::cout << " " << money << " ";
    }
};

// struct EmployeeLog
// {
//     char employeeid[31];
//     string oper;

//     EmployeeLog(const char *id,
//                 const string &oper) : oper(oper)
//     {
//         std::strncpy(employeeid, id, 30);
//     }

//     EmployeeLog &operator=(const EmployeeLog &other)
//     {
//         std::strncpy(employeeid, other.employeeid, 30);
//         oper = other.oper;
//     }

//     bool operator==(const EmployeeLog &other) const
//     {
//         return std::strcmp(employeeid, other.employeeid) == 0 &&
//                oper == other.oper;
//     }

//     bool operator<(const EmployeeLog &other) const
//     {
//         return std::strcmp(employeeid, other.employeeid) < 0;
//     }

//     void print()
//     {
//         std::cout << employeeid << " operation:" << oper << '\n';
//     }
// };

// struct Log
// {
//     long long count;
//     string userid;
//     string oper;

//     Log(long long count, const string &userid,
//         const string &oper) : count(count), oper(oper), userid(userid)
//     {
//     }

//     Log &operator=(const Log &other)
//     {
//         count = other.count;
//         oper = other.oper;
//         userid = other.userid;
//     }

//     bool operator==(const Log &other) const
//     {
//         return count == other.count &&
//                oper == other.oper &&
//                userid == other.userid;
//     }

//     bool operator<(const Log &other) const
//     {
//         return count < other.count;
//     }

//     void print()
//     {
//         std::cout << "operation_number: " << count
//                   << " user:" << userid
//                   << " operation:" << oper << '\n';
//     }
// };

class FinanceLogManager
{
    int finance_total_count;
    BlockStorageSystem<FinanceLog> finance_storage;

public:
    FinanceLogManager();
    void add_finance_record(const double &money,
                            const bool type,
                            const string &isbn);
    void view_finance_record(const int &number);
};

// class EmployeeLogManager
// {
//     BlockStorageSystem<EmployeeLog> employee_storage;

// public:
// };

// class Log
// {
//     long long total_count;
//     BlockStorageSystem<Log> entire_storage;

// public:
// };