#include "Log.hpp"

FinanceLogManager::FinanceLogManager() : finance_storage("./data/finance_storage.dat")
{
    finance_total_count = 0;
    finance_storage.initialize_system();
}

void FinanceLogManager::add_finance_record(const double &money, const bool type, const string &isbn)
{
    finance_total_count++;
    FinanceLog new_log(finance_total_count, money, type, isbn);
    std::string string_count = std::to_string(finance_total_count);
    finance_storage.add_data(string_count, new_log);
}

void FinanceLogManager::view_finance_record(const int &number)
{
    std::string index = std::to_string(finance_total_count - number);
    finance_storage.traverse(index);
}