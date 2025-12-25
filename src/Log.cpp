#include "Log.hpp"
#include <iomanip>

FinanceLogManager::FinanceLogManager() : finance_storage("finance_storage.dat")
{
    finance_storage.initialize_system();

    finance_count_file.open("finance_count.dat", std::ios::binary | std::ios::in | std::ios::out);
    if (!finance_count_file.is_open())
    {
        finance_count_file.clear();
        finance_count_file.open("finance_count.dat", std::ios::binary | std::ios::out | std::ios::trunc);
        finance_count_file.close();
        finance_count_file.open("finance_count.dat", std::ios::binary | std::ios::in | std::ios::out);
    }

    finance_count_file.seekg(0);
    if (!finance_count_file.read(reinterpret_cast<char *>(&finance_total_count), sizeof(finance_total_count)))
    {
        finance_total_count = 0;
        finance_storage.add_data("0", FinanceLog());
        finance_count_file.clear();
        finance_count_file.seekp(0);
        finance_count_file.write(reinterpret_cast<char *>(&finance_total_count), sizeof(finance_total_count));
    }
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

FinanceLogManager::~FinanceLogManager()
{
    if (finance_count_file.is_open())
    {
        finance_count_file.seekp(0);
        finance_count_file.write(reinterpret_cast<char *>(&finance_total_count), sizeof(finance_total_count));
        finance_count_file.close();
    }
}

void FinanceLogManager::view_finance_record(const int &number)
{
    // std::string index = std::to_string(finance_total_count - number);
    int number_ = number;
    if (number_ == 0)
        number_ = finance_total_count;
    if (number_ > finance_total_count)
    {
        throw("Invalid\n");
    }
    std::vector<FinanceLog> taillog = finance_storage.search_data(std::to_string(finance_total_count));
    std::vector<FinanceLog> headlog = finance_storage.search_data(std::to_string(finance_total_count - number_));
    std::cout << std::fixed << std::setprecision(2) << "+ " << taillog[0].moneyadd - headlog[0].moneyadd << " - " << taillog[0].moneyminus - headlog[0].moneyminus << '\n';
}