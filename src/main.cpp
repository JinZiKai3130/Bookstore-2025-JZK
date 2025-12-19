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
    while (true)
    {
        Users cur_user = user_magr.get_user();
        std::cin >> op;
        if (op == "quit" || op == "exit")
            break;
        getline(std::cin, line);
        line.erase(0, 1);
        if (op == "su")
        {
            std::istringstream iss(line);
            string id, password;
            iss >> id >> password;
            user_magr.login(id.c_str(), cur_user, password.c_str());
        }
        else if (op == "logout")
        {
            user_magr.logout();
        }
        else if (op == "register")
        {
            std::istringstream iss(line);
            string id, password, username;
            iss >> id >> password >> username;
            user_magr.regist(id.c_str(), password.c_str(), username.c_str());
        }
        else if (op == "passwd")
        {
            std::istringstream iss(line);
            string id, cur_pwd, new_pwd, username;
            string tmp;
            std::vector<std::string> words;
            while (iss >> tmp)
            {
                words.push_back(tmp);
            }
            if (words.size() == 2)
            {
                id = words[0];
                new_pwd = words[1];
            }
            else
            {
                id = words[0];
                cur_pwd = words[1];
                new_pwd = words[2];
            }
        }
        else if (op == "useradd")
        {
        }
        else if (op == "delete")
        {
        }
        else if (op == "show")
        {
        }
        else if (op == "buy")
        {
        }
        else if (op == "select")
        {
        }
        else if (op == "modify")
        {
        }
        else if (op == "import")
        {
        }
        else if (op == "log")
        {
        }
        else if (op == "report")
        {
        }
        else
        {
            std::cout << "Invalid\n";
        }
    }
    return 0;
}