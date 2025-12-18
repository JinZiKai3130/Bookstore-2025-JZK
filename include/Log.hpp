#pragma once

#include <iostream>
#include <vector>
#include <cstring>
using namespace string;
using namespace vector;


struct FinanceLog {
    int count;
    double money;
    bool type;
    std::string bookname;

    FinanceLog (const int count, const double money
                const bool type, 
                const string& bookname) : 
                count(count), money(money),
                type(type), bookname(bookname) {}
    
    FinanceLog& operator=(const FinanaceLog& other) {
        count = other.count;
        money = other.money;
        type = other.type;
        bookname = other.bookname;
        return *this;
    }

    bool operator==(const FinanceLog& other) const {
        return count == other.count &&
               money == other.money &&
               type == other.type &&
               bookname == other.bookname;
    }

    bool operator<(const Users& other) const {
        return count < other.count;
    }

    void std_print() {
        if (type) std::cout << "+";
        else std::cout << "-";
        std::cout << " " << money << " ";
    }
};

struct EmployeeLog {
    char employeeid[31];
    string oper;

    EmployeeLog (const char* id, 
               const string& oper) : oper(oper) {
                    std::strncpy(employeeid, id);
    }
    
    EmployeeLog& operator=(const EmployeeLog& other) {
        std::strncpy(employeeid, other.employeeid);
        oper = other.oper;
    }

    bool operator==(const EmployeeLog& other) const {
        return std::strncpy(employeeid, other.employeeid) == 0 &&
               oper == other.oper;
    }

    bool operator<(const EmployeeLog& other) const {
        return std::strncpy(employeeid, other.employeeid) < 0;
    }

    void print() {
        std::cout << employeeid << " operation:" << oper << '\n';
    }
};

struct Log {
    long long count;
    string userid;
    string oper;

    Log (long long count, const string& userid
         const string& oper) :
         count(count), oper(oper), userid(userid) {
    }
    
    Log& operator=(const Log& other) {
        count = other.count;
        oper = other.oper;
        userid = other.userid;
    }

    bool operator==(const Log& other) const {
        return count == other.count &&
               oper == other.oper &&
               userid == other.userid;
    }

    bool operator<(const EmployeeLog& other) const {
        return count < other.count;
    }

    void print() {
        std::cout << "operation_number: "<< count
                  << " user:" << userid
                  << " operation:" << oper << '\n';
    }
}

class FinanceLogManager {
    int finance_total_count;
    BlockStorageSystem<FinanceLog> finance_storage;

public:

};

class EmployeeLogManager {
    BlockStorageSystem<EmployeeLog> employee_storage;

public:

};

class Log {
    long long total_count;
    BlockStorageSystem<Log> entire_storage;

public:

}