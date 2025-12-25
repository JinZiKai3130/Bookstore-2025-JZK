#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include "User.hpp"
#include "Book.hpp"
#include "Log.hpp"
#include "Error.hpp"
#include "BlockStorage.hpp"

using std::string;
using std::vector;

int main()
{
    UserManager user_magr;
    BookManager book_magr;
    FinanceLogManager finance_magr;

    string op, line;
    int tmp_cnt = 0;
    while (std::cin >> op)
    {
        tmp_cnt++;
        if (op == "quit" || op == "exit")
            break;
        getline(std::cin, line);
        // std::cin.ignore();
        for (int i = 0; i < line.size(); i++)
        {
            if (line[i] == '\t') // 神了
            {
                throw("Invalid\n");
            }
        }
        if (!line.empty()) line.erase(0, 1);
        std::istringstream iss(line);
        try
        {
            if (op == "su")
            {
                string id, password = "";
                iss >> id >> password;
                // std::cout << id << " " << password << '\n';
                user_magr.login(id.c_str(), password.c_str());
            }
            else if (op == "logout")
            {
                Users cur_user = user_magr.get_user();
                user_magr.logout();
            }
            else if (op == "register")
            {
                string id, password, username;
                iss >> id >> password >> username;
                if (iss >> username)
                {
                    // std::cout << username << std::endl;
                    throw("Invalid\n");
                }
                user_magr.regist(id.c_str(), password.c_str(), username.c_str());
            }
            else if (op == "passwd")
            {
                Users cur_user = user_magr.get_user();
                string id, cur_pwd = "", new_pwd, username;
                string tmp;
                std::vector<std::string> words;
                while (iss >> tmp)
                {
                    words.push_back(tmp);
                }
                if (words.size() < 2 || words.size() > 3)
                {
                    throw("Invalid\n");
                }
                if (words.size() == 2)
                {
                    if (cur_user.privilege != 7)
                    {
                        throw("Invalid\n");
                    }
                    id = words[0];
                    new_pwd = words[1];
                }
                else
                {
                    id = words[0];
                    cur_pwd = words[1];
                    new_pwd = words[2];
                }
                // std::cout << "start pwd change" << std::endl;
                user_magr.change_pwd(id.c_str(), new_pwd.c_str(), cur_user.privilege, cur_pwd.c_str());
            }
            else if (op == "useradd")
            {
                Users cur_user = user_magr.get_user();
                if (cur_user.privilege < 3)
                {
                    throw("Invalid\n");
                }
                string id, pwd, username;
                int privilege;
                iss >> id >> pwd >> privilege >> username;
                if (iss >> username)
                {
                    // std::cout << username << std::endl;
                    throw("Invalid\n");
                }
                user_magr.useradd(id.c_str(), pwd.c_str(), privilege, username.c_str(), cur_user.privilege, 0);
            }
            else if (op == "delete")
            {
                Users cur_user = user_magr.get_user();
                if (cur_user.privilege != 7)
                {
                    throw("Invalid\n");
                }
                string id;
                iss >> id;
                user_magr.delt(id.c_str());
            }
            else if (op == "show")
            {
                Users cur_user = user_magr.get_user();
                if (cur_user.privilege < 1)
                {
                    throw("Invalid\n");
                }
                string tmp = "";
                int cnt = 0;
                if (line.empty())
                {
                    book_magr.show("");
                    continue;
                }
                iss >> tmp;
                if (tmp == "finance")
                {
                    if (cur_user.privilege < 7)
                    {
                        throw("Invalid\n");
                    }
                    string oper_num;
                    while (iss >> oper_num)
                    {
                        for (char const &c : oper_num)
                        {
                            if (!std::isdigit(c))
                            {
                                throw("Invalid\n");
                            }
                        }
                    }
                    if (oper_num.empty())
                        finance_magr.view_finance_record(0);
                    else
                        finance_magr.view_finance_record(std::stoi(oper_num));
                }
                else
                {
                    if (iss >> tmp)
                    {
                        throw("Invalid\n");
                    }
                    book_magr.show(tmp);
                }
            }
            else if (op == "buy")
            {
                Users cur_user = user_magr.get_user(); // 无法get则说明未登录
                string isbn, quantity;
                iss >> isbn >> quantity;
                if (iss >> isbn)
                {
                    throw("Invalid\n");
                }
                book_magr.buy(isbn.c_str(), quantity);
                vector<Book> cur_book = book_magr.f_by_isbn(isbn.c_str());
                double tot_money = cur_book[0].price * static_cast<double>(std::stoi(quantity));
                finance_magr.add_finance_record(tot_money, 0, isbn.c_str());
            }
            else if (op == "select")
            {
                Users cur_user = user_magr.get_user();
                if (cur_user.privilege < 3)
                {
                    throw("Invalid\n");
                } // 新增
                string isbn;
                iss >> isbn;
                if (!book_magr.check_isbn(isbn.c_str()))
                {
                    // std::cout << "Invalid1\n";
                    throw("Invalid\n");
                }
                if (iss >> isbn)
                {
                    // std::cout << "Invalid2\n";
                    throw("Invalid\n");
                }
                string emptystring = "";
                user_magr.select_book(isbn.c_str(), emptystring.c_str());
                book_magr.select(isbn.c_str());
            }
            else if (op == "modify")
            {
                // std::cout << "this modify\n";
                Users cur_user = user_magr.get_user();
                if (cur_user.privilege < 3)
                {
                    throw("Invalid\n");
                } // 新增
                // std::cout << "getuser\n";
                if (strlen(cur_user.selected_book) == 0)
                {
                    // std::cout << "no select book\n";
                    throw("Invalid\n");
                }
                string tmp = cur_user.selected_book;
                string origintmp = tmp;
                // std::cout << "modifystat\n";
                book_magr.modify(line, tmp);
                // std::cout << "modifyend\n";
                user_magr.select_book(tmp.c_str(), origintmp.c_str());
            }
            else if (op == "import")
            {
                // std::cout << "start import\n";
                Users cur_user = user_magr.get_user();
                if (cur_user.privilege < 3)
                {
                    throw("Invalid\n");
                } // 建议新增
                string number, tot_cost;
                iss >> number >> tot_cost;
                if (iss >> number)
                {
                    throw("Invalid\n");
                }
                book_magr.impt(number, cur_user);

                double tot_money = std::stod(tot_cost);
                if (tot_money == 0)
                {
                    throw("Invalid\n");
                }
                finance_magr.add_finance_record(tot_money, 1, cur_user.selected_book);
                // std::cout << "end import\n";
            }
            // else if (op == "log")
            // {
            //     Users cur_user = user_magr.get_user();

            // }
            // else if (op == "report")
            // {
            //     Users cur_user = user_magr.get_user();
            // }
            else
            {
                throw("Invalid\n");
            }
        }
        catch (const char *e)
        {
            // std::cout << "Line" << tmp_cnt << std::endl;
            std::cout << e;
        }
    }
    return 0;
}