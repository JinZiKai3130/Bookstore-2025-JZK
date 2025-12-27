#pragma once

#include "BlockStorage.hpp"
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
using std::string;
using std::vector;

struct FinanceLog {
  int count;
  char isbn[21];
  double moneyadd;
  double moneyminus;

  FinanceLog() { count = 0, isbn[0] = '\0', moneyadd = 0, moneyminus = 0; }

  FinanceLog(const int count, const char *isbn, const double moneyadd,
             const double moneyminus)
      : count(count), moneyadd(moneyadd), moneyminus(moneyminus) {
    std::strncpy(this->isbn, isbn, 20);
  }

  FinanceLog &operator=(const FinanceLog &other) {
    count = other.count;
    moneyadd = other.moneyadd;
    moneyminus = other.moneyminus;
    std::strncpy(this->isbn, other.isbn, 20);
    return *this;
  }

  bool operator==(const FinanceLog &other) const {
    return count == other.count;
  }

  bool operator<(const FinanceLog &other) const { return count < other.count; }
};

struct FinanceRecord {
  int count;
  char isbn[21];
  double income;
  double expense;
  char type;
  char userid[31];

  FinanceRecord() {
    count = 0;
    isbn[0] = '\0';
    income = 0;
    expense = 0;
    type = ' ';
    userid[0] = '\0';
  }

  FinanceRecord(int count, const char *isbn, double income, double expense,
                char type, const char *userid)
      : count(count), income(income), expense(expense), type(type) {
    std::strncpy(this->isbn, isbn, 20);
    this->isbn[20] = '\0';
    std::strncpy(this->userid, userid, 30);
    this->userid[30] = '\0';
  }

  FinanceRecord &operator=(const FinanceRecord &other) {
    count = other.count;
    income = other.income;
    expense = other.expense;
    type = other.type;
    std::strncpy(isbn, other.isbn, 20);
    isbn[20] = '\0';
    std::strncpy(userid, other.userid, 30);
    userid[30] = '\0';
    return *this;
  }

  bool operator==(const FinanceRecord &other) const {
    return count == other.count;
  }

  bool operator<(const FinanceRecord &other) const {
    return count < other.count;
  }

  void print() {
    std::cout << "count:" << count << " | ";
    if (type == 'I') {
      std::cout << "收入 +" << std::fixed << std::setprecision(2) << income;
      std::cout << " | ISBN: " << isbn;
    } else {
      std::cout << "支出 -" << std::fixed << std::setprecision(2) << expense;
      std::cout << " | ISBN: " << isbn;
    }
    std::cout << " | 操作员: " << userid << '\n';
  }
};

struct EmployeeLog {
  int count;
  char employeeid[31];
  char operation[201];

  EmployeeLog() {
    count = 0;
    employeeid[0] = '\0';
    operation[0] = '\0';
  }

  EmployeeLog(int count, const char *empid, const char *oper) : count(count) {
    std::strncpy(employeeid, empid, 30);
    employeeid[30] = '\0';
    std::strncpy(operation, oper, 200);
    operation[200] = '\0';
  }

  EmployeeLog &operator=(const EmployeeLog &other) {
    count = other.count;
    std::strncpy(employeeid, other.employeeid, 30);
    employeeid[30] = '\0';
    std::strncpy(operation, other.operation, 200);
    operation[200] = '\0';
    return *this;
  }

  bool operator==(const EmployeeLog &other) const {
    return count == other.count;
  }

  bool operator<(const EmployeeLog &other) const { return count < other.count; }

  void print() {
    std::cout << "员工：" << employeeid << "\t" << operation << '\n';
  }
};

struct SystemLog {
  int count;
  char userid[31];
  char operation[201];

  SystemLog() {
    count = 0;
    userid[0] = '\0';
    operation[0] = '\0';
  }

  SystemLog(int count, const char *uid, const char *oper) : count(count) {
    std::strncpy(userid, uid, 30);
    userid[30] = '\0';
    std::strncpy(operation, oper, 200);
    operation[200] = '\0';
  }

  SystemLog &operator=(const SystemLog &other) {
    count = other.count;
    std::strncpy(userid, other.userid, 30);
    userid[30] = '\0';
    std::strncpy(operation, other.operation, 200);
    operation[200] = '\0';
    return *this;
  }

  bool operator==(const SystemLog &other) const { return count == other.count; }

  bool operator<(const SystemLog &other) const { return count < other.count; }

  void print() {
    std::cout << "count:" << count << "\t" << "操作者：" << userid << "\t"
              << operation << '\n';
  }
};

class FinanceLogManager {
  int finance_total_count;
  int finance_record_count;
  BlockStorageSystem<FinanceLog> finance_storage;
  BlockStorageSystem<FinanceRecord> finance_record_storage;
  std::fstream finance_count_file;

public:
  FinanceLogManager();
  ~FinanceLogManager();
  void add_finance_record(const double &money, const bool type,
                          const char *isbn, const char *userid);
  void view_finance_record(const int &number);
  void report_finance();
};

class EmployeeLogManager {
  int employee_log_count;
  BlockStorageSystem<EmployeeLog> employee_storage;
  std::fstream employee_count_file;

public:
  EmployeeLogManager();
  ~EmployeeLogManager();
  void add_employee_log(const char *employeeid, const char *operation);
  void report_employee();
};

class SystemLogManager {
  int system_log_count;
  BlockStorageSystem<SystemLog> system_storage;
  std::fstream system_count_file;

public:
  SystemLogManager();
  ~SystemLogManager();
  void add_system_log(const char *userid, const char *operation);
  void show_log();
};