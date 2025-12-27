#include "Log.hpp"
#include <iomanip>

FinanceLogManager::FinanceLogManager()
    : finance_storage("finance_storage.dat"),
      finance_record_storage("finance_record.dat") {
  finance_count_file.open("finance_count.dat",
                          std::ios::binary | std::ios::in | std::ios::out);
  if (!finance_count_file.is_open()) {
    finance_count_file.clear();
    finance_count_file.open("finance_count.dat",
                            std::ios::binary | std::ios::out | std::ios::trunc);
    finance_count_file.close();
    finance_count_file.open("finance_count.dat",
                            std::ios::binary | std::ios::in | std::ios::out);
  }

  finance_count_file.seekg(0);
  if (!finance_count_file.read(reinterpret_cast<char *>(&finance_total_count),
                               sizeof(finance_total_count))) {
    finance_total_count = 0;
    finance_record_count = 0;
    finance_storage.add_data("0", FinanceLog());
    finance_count_file.clear();
    finance_count_file.seekp(0);
    finance_count_file.write(reinterpret_cast<char *>(&finance_total_count),
                             sizeof(finance_total_count));
    finance_count_file.write(reinterpret_cast<char *>(&finance_record_count),
                             sizeof(finance_record_count));
  } else {
    finance_count_file.read(reinterpret_cast<char *>(&finance_record_count),
                            sizeof(finance_record_count));
  }
}

void FinanceLogManager::add_finance_record(const double &money, const bool type,
                                           const char *isbn,
                                           const char *userid) {
  std::vector<FinanceLog> lastlog =
      finance_storage.search_data(std::to_string(finance_total_count));
  finance_total_count++;
  finance_record_count++;

  FinanceRecord record;
  record.count = finance_record_count;
  std::strncpy(record.isbn, isbn, 20);
  record.isbn[20] = '\0';
  std::strncpy(record.userid, userid, 30);
  record.userid[30] = '\0';

  if (!type) {
    lastlog[0].moneyadd += money;
    record.income = money;
    record.expense = 0;
    record.type = 'I';
  } else {
    lastlog[0].moneyminus += money;
    record.income = 0;
    record.expense = money;
    record.type = 'E';
  }

  std::string string_count = std::to_string(finance_total_count);
  finance_storage.add_data(string_count, lastlog[0]);

  std::string record_id = std::to_string(finance_record_count);
  finance_record_storage.add_data(record_id, record);
}

FinanceLogManager::~FinanceLogManager() {
  if (finance_count_file.is_open()) {
    finance_count_file.seekp(0);
    finance_count_file.write(reinterpret_cast<char *>(&finance_total_count),
                             sizeof(finance_total_count));
    finance_count_file.write(reinterpret_cast<char *>(&finance_record_count),
                             sizeof(finance_record_count));
    finance_count_file.close();
  }
}

void FinanceLogManager::view_finance_record(const int &number) {
  // std::string index = std::to_string(finance_total_count - number);
  int number_ = number;
  if (number_ == 0)
    number_ = finance_total_count;
  if (number_ > finance_total_count) {
    throw("Invalid\n");
  }
  std::vector<FinanceLog> taillog =
      finance_storage.search_data(std::to_string(finance_total_count));
  std::vector<FinanceLog> headlog = finance_storage.search_data(
      std::to_string(finance_total_count - number_));
  std::cout << std::fixed << std::setprecision(2) << "+ "
            << taillog[0].moneyadd - headlog[0].moneyadd << " - "
            << taillog[0].moneyminus - headlog[0].moneyminus << '\n';
}

void FinanceLogManager::report_finance() {
  std::cout << "-----------------------------------------------------------\n";
  std::cout << "收入支出\n";
  std::cout << "-----------------------------------------------------------\n";
  if (finance_record_count == 0) {
    return;
  }
  double total_income = 0.00;
  double total_expense = 0.00;

  std::cout << "交易详情:\n";
  std::cout << "-----------------------------------------------------------\n";

  for (int i = 1; i <= finance_record_count; i++) {
    std::vector<FinanceRecord> records =
        finance_record_storage.search_data(std::to_string(i));
    if (!records.empty()) {
      records[0].print();
      total_income += records[0].income;
      total_expense += records[0].expense;
    }
  }

  std::cout << "-----------------------------------------------------------\n";
  std::cout << "总共交易：" << finance_record_count << "次\n";
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "总收入：" << total_income << "\n";
  std::cout << "总支出：" << total_expense << "\n";
  std::cout << "净利润：" << (total_income - total_expense) << "\n";
  std::cout << "-----------------------------------------------------------\n";
}

EmployeeLogManager::EmployeeLogManager()
    : employee_storage("employee_log.dat") {
  employee_count_file.open("employee_count.dat",
                           std::ios::binary | std::ios::in | std::ios::out);
  if (!employee_count_file.is_open()) {
    employee_count_file.clear();
    employee_count_file.open("employee_count.dat", std::ios::binary |
                                                       std::ios::out |
                                                       std::ios::trunc);
    employee_count_file.close();
    employee_count_file.open("employee_count.dat",
                             std::ios::binary | std::ios::in | std::ios::out);
  }

  employee_count_file.seekg(0);
  if (!employee_count_file.read(reinterpret_cast<char *>(&employee_log_count),
                                sizeof(employee_log_count))) {
    employee_log_count = 0;
    employee_count_file.clear();
    employee_count_file.seekp(0);
    employee_count_file.write(reinterpret_cast<char *>(&employee_log_count),
                              sizeof(employee_log_count));
  }
}

EmployeeLogManager::~EmployeeLogManager() {
  if (employee_count_file.is_open()) {
    employee_count_file.seekp(0);
    employee_count_file.write(reinterpret_cast<char *>(&employee_log_count),
                              sizeof(employee_log_count));
    employee_count_file.close();
  }
}

void EmployeeLogManager::add_employee_log(const char *employeeid,
                                          const char *operation) {
  employee_log_count++;
  EmployeeLog log(employee_log_count, employeeid, operation);
  employee_storage.add_data(employeeid, log);
}

void EmployeeLogManager::report_employee() {
  std::cout << "员工工作情况报告\n";
  std::cout << "-----------------------------------------------------------\n";

  if (employee_log_count == 0) {
    return;
  }

  employee_storage.traverse();
  std::cout << "-----------------------------------------------------------\n";
  std::cout << "一共" << employee_log_count << "次员工操作\n";
  std::cout << "-----------------------------------------------------------\n";
}

SystemLogManager::SystemLogManager() : system_storage("system_log.dat") {
  system_count_file.open("system_count.dat",
                         std::ios::binary | std::ios::in | std::ios::out);
  if (!system_count_file.is_open()) {
    system_count_file.clear();
    system_count_file.open("system_count.dat",
                           std::ios::binary | std::ios::out | std::ios::trunc);
    system_count_file.close();
    system_count_file.open("system_count.dat",
                           std::ios::binary | std::ios::in | std::ios::out);
  }

  system_count_file.seekg(0);
  if (!system_count_file.read(reinterpret_cast<char *>(&system_log_count),
                              sizeof(system_log_count))) {
    system_log_count = 0;
    system_count_file.clear();
    system_count_file.seekp(0);
    system_count_file.write(reinterpret_cast<char *>(&system_log_count),
                            sizeof(system_log_count));
  }
}

SystemLogManager::~SystemLogManager() {
  if (system_count_file.is_open()) {
    system_count_file.seekp(0);
    system_count_file.write(reinterpret_cast<char *>(&system_log_count),
                            sizeof(system_log_count));
    system_count_file.close();
  }
}

void SystemLogManager::add_system_log(const char *userid,
                                      const char *operation) {
  system_log_count++;
  // std::cout << "system_log_count = " << system_log_count << '\n';
  SystemLog log(system_log_count, userid, operation);
  std::string id_str = std::to_string(system_log_count);
  system_storage.add_data(id_str, log);
}

void SystemLogManager::show_log() {
  std::cout << "系统日志\n";
  std::cout << "-----------------------------------------------------------\n";

  if (system_log_count == 0) {
    return;
  }

  for (int i = 1; i <= system_log_count; i++) {
    std::vector<SystemLog> logs = system_storage.search_data(std::to_string(i));
    if (!logs.empty()) {
      logs[0].print();
    }
  }
  // system_storage.traverse();
  std::cout << "-----------------------------------------------------------\n";
  std::cout << "一共" << system_log_count << "条日志\n";
  std::cout << "-----------------------------------------------------------\n";
}