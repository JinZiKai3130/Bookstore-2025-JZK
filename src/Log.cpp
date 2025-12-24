#include "Log.hpp"
#include <iomanip>

FinanceLogManager::FinanceLogManager() : finance_storage("./data/finance_storage.dat")
{
    finance_total_count = 0;
    finance_storage.initialize_system();
    finance_storage.add_data("0", FinanceLog());
}

void FinanceLogManager::add_finance_record(const double &money, const bool type, const char *isbn)
{
    std::vector<FinanceLog> lastlog = finance_storage.search_data(std::to_string(finance_total_count));
    finance_total_count++;
    if (!type)
    {
        lastlog[0].moneyadd += money;
    }
    else
    {
        lastlog[0].moneyminus += money;
    }
    std::string string_count = std::to_string(finance_total_count);
    finance_storage.add_data(string_count, lastlog[0]);
}

void FinanceLogManager::view_finance_record(const int &number)
{
    // std::string index = std::to_string(finance_total_count - number);
    int number_ = number;
    if (number_ == 0)
        number_ = finance_total_count;
    std::vector<FinanceLog> taillog = finance_storage.search_data(std::to_string(finance_total_count));
    std::vector<FinanceLog> headlog = finance_storage.search_data(std::to_string(finance_total_count - number_));
    std::cout << std::fixed << std::setprecision(2) << "+ " << taillog[0].moneyadd - headlog[0].moneyadd << " - " << taillog[0].moneyminus - headlog[0].moneyminus << '\n';
}