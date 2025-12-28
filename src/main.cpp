#include "BlockStorage.hpp"
#include "Book.hpp"
#include "Error.hpp"
#include "Log.hpp"
#include "User.hpp"
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <vector>

using std::string;
using std::vector;

int main() {
  UserManager user_magr;
  BookManager book_magr;
  FinanceLogManager finance_magr;
  EmployeeLogManager employee_magr;
  SystemLogManager system_magr;

  string op, line;
  int tmp_cnt = 0;
  while (getline(std::cin, line)) {
    tmp_cnt++;
    if (line.empty()) {
      continue;
    }
    bool invalid_char = false;
    for (int i = 0; i < line.size(); i++) {
      if (line[i] != ' ' &&
          !std::isgraph(static_cast<unsigned char>(line[i]))) {
        invalid_char = true;
        break;
      }
    }
    if (invalid_char) {
      std::cout << "Invalid\n";
      continue;
    }
    std::istringstream iss(line);
    iss >> op;
    if (op == "quit" || op == "exit")
      break;
    try {
      if (op == "su") {
        string id, password = "";
        if (!(iss >> id)) {
          throw("Invalid\n");
        }
        iss >> password;
        // std::cout << id << " " << password << '\n';
        user_magr.login(id.c_str(), password.c_str());
        string log_msg = "su " + id;
        system_magr.add_system_log(id.c_str(), log_msg.c_str());
      } else if (op == "logout") {
        Users cur_user = user_magr.get_user();
        string log_msg = "logout";
        system_magr.add_system_log(cur_user.UserID, log_msg.c_str());
        user_magr.logout();
      } else if (op == "register") {
        string id, password, username;
        if (!(iss >> id >> password >> username)) {
          throw("Invalid\n");
        }
        string extra;
        if (iss >> extra) {
          throw("Invalid\n");
        }
        user_magr.regist(id.c_str(), password.c_str(), username.c_str());
        string log_msg = "register " + id;
        system_magr.add_system_log(id.c_str(), log_msg.c_str());
      } else if (op == "passwd") {
        Users cur_user = user_magr.get_user();
        string id, cur_pwd = "", new_pwd, username;
        string tmp;
        std::vector<std::string> words;
        while (iss >> tmp) {
          words.push_back(tmp);
        }
        if (words.size() < 2 || words.size() > 3) {
          throw("Invalid\n");
        }
        if (words.size() == 2) {
          if (cur_user.privilege != 7) {
            throw("Invalid\n");
          }
          id = words[0];
          new_pwd = words[1];
        } else {
          id = words[0];
          cur_pwd = words[1];
          new_pwd = words[2];
        }
        // std::cout << "start pwd change" << std::endl;
        user_magr.change_pwd(id.c_str(), new_pwd.c_str(), cur_user.privilege,
                             cur_pwd.c_str());
        string log_msg = "passwd " + id;
        system_magr.add_system_log(cur_user.UserID, log_msg.c_str());
      } else if (op == "useradd") {
        Users cur_user = user_magr.get_user();
        if (cur_user.privilege < 3) {
          throw("Invalid\n");
        }
        string id, pwd, username, priv_str;
        if (!(iss >> id >> pwd >> priv_str >> username)) {
          throw("Invalid\n");
        }
        string extra;
        if (iss >> extra) {
          throw("Invalid\n");
        }
        if (priv_str.empty() || priv_str.length() > 1) {
          throw("Invalid\n");
        }
        for (char c : priv_str) {
          if (!std::isdigit(static_cast<unsigned char>(c))) {
            throw("Invalid\n");
          }
        }
        int privilege = std::stoi(priv_str);
        user_magr.useradd(id.c_str(), pwd.c_str(), privilege, username.c_str(),
                          cur_user.privilege, 0);
        string log_msg =
            "useradd " + id + " privilege=" + std::to_string(privilege);
        employee_magr.add_employee_log(cur_user.UserID, log_msg.c_str());
        system_magr.add_system_log(cur_user.UserID, log_msg.c_str());
      } else if (op == "delete") {
        Users cur_user = user_magr.get_user();
        if (cur_user.privilege != 7) {
          throw("Invalid\n");
        }
        string id;
        if (!(iss >> id)) {
          throw("Invalid\n");
        }
        string extra;
        if (iss >> extra) {
          throw("Invalid\n");
        }
        user_magr.delt(id.c_str());
        string log_msg = "delete user " + id;
        employee_magr.add_employee_log(cur_user.UserID, log_msg.c_str());
        system_magr.add_system_log(cur_user.UserID, log_msg.c_str());
      } else if (op == "show") {
        Users cur_user = user_magr.get_user();
        if (cur_user.privilege < 1) {
          throw("Invalid\n");
        }
        string tmp = "";
        if (!(iss >> tmp)) {
          book_magr.show("");
          continue;
        }
        if (tmp == "finance") {
          if (cur_user.privilege < 7) {
            throw("Invalid\n");
          }
          string oper_num;
          int count = 0;
          while (iss >> oper_num) {
            count++;
            if (count > 1) {
              throw("Invalid\n");
            }
            if (oper_num.empty() || oper_num.length() > 10) {
              throw("Invalid\n");
            }
            if (oper_num.length() > 1 && oper_num[0] == '0') {
              throw("Invalid\n");
            }
            for (char const &c : oper_num) {
              if (!std::isdigit(c)) {
                throw("Invalid\n");
              }
            }
            try {
              long long val = std::stoll(oper_num);
              if (val > 2147483647LL) {
                throw("Invalid\n");
              }
            } catch (...) {
              throw("Invalid\n");
            }
          }
          if (oper_num.empty())
            finance_magr.view_finance_record(0);
          else
            finance_magr.view_finance_record(std::stoi(oper_num));
          string log_msg = "show finance";
          system_magr.add_system_log(cur_user.UserID, log_msg.c_str());
        } else {
          if (iss >> tmp) {
            throw("Invalid\n");
          }
          book_magr.show(tmp);
        }
      } else if (op == "buy") {
        Users cur_user = user_magr.get_user(); // 无法get则说明未登录
        string isbn, quantity;
        if (!(iss >> isbn >> quantity)) {
          throw("Invalid\n");
        }
        string extra;
        if (iss >> extra) {
          throw("Invalid\n");
        }
        if (quantity.empty() || quantity.length() > 10) {
          throw("Invalid\n");
        }
        if (quantity.length() > 10 || std::stoll(quantity) > 2147483647LL) {
          throw("Invalid\n");
        }
        book_magr.buy(isbn.c_str(), quantity);
        vector<Book> cur_book = book_magr.f_by_isbn(isbn.c_str());
        double tot_money =
            cur_book[0].price * static_cast<double>(std::stoi(quantity));
        finance_magr.add_finance_record(tot_money, 0, isbn.c_str(),
                                        cur_user.UserID);
        string log_msg = "buy " + isbn + " quantity=" + quantity;
        system_magr.add_system_log(cur_user.UserID, log_msg.c_str());
      } else if (op == "select") {
        Users cur_user = user_magr.get_user();
        if (cur_user.privilege < 3) {
          throw("Invalid\n");
        }
        string isbn;
        iss >> isbn;
        if (!book_magr.check_isbn(isbn.c_str())) {
          // std::cout << "Invalid1\n";
          throw("Invalid\n");
        }
        if (iss >> isbn) {
          // std::cout << "Invalid2\n";
          throw("Invalid\n");
        }
        string emptystring = "";
        user_magr.select_book(isbn.c_str(), emptystring.c_str());
        book_magr.select(isbn.c_str());
        string log_msg = "select " + isbn;
        employee_magr.add_employee_log(cur_user.UserID, log_msg.c_str());
        system_magr.add_system_log(cur_user.UserID, log_msg.c_str());
      } else if (op == "modify") {
        // std::cout << "this modify\n";
        Users cur_user = user_magr.get_user();
        if (cur_user.privilege < 3) {
          throw("Invalid\n");
        }
        // std::cout << "getuser\n";
        if (strlen(cur_user.selected_book) == 0) {
          //   std::cout << "no select book\n";
          throw("Invalid\n");
        }
        string tmp = cur_user.selected_book;
        string origintmp = tmp;
        string modify_args;
        size_t pos = line.find("modify");
        modify_args = line.substr(pos + 6);
        // std::cout << "modifystat\n";
        book_magr.modify(modify_args, tmp);
        // std::cout << "modifyend\n";
        user_magr.select_book(tmp.c_str(), origintmp.c_str());
        string log_msg = "modify book " + origintmp + " " + modify_args;
        employee_magr.add_employee_log(cur_user.UserID, log_msg.c_str());
        system_magr.add_system_log(cur_user.UserID, log_msg.c_str());
      } else if (op == "import") {
        // std::cout << "start import\n";
        Users cur_user = user_magr.get_user();
        if (cur_user.privilege < 3) {
          throw("Invalid\n");
        }
        string number, tot_cost;
        if (!(iss >> number >> tot_cost)) {
          throw("Invalid\n");
        }
        string extra;
        if (iss >> extra) {
          throw("Invalid\n");
        }
        if (!book_magr.check_price(tot_cost)) {
          throw("Invalid\n");
        }
        double tot_money = std::stod(tot_cost);
        if (tot_money <= 0) {
          throw("Invalid\n");
        }
        book_magr.impt(number, cur_user);

        finance_magr.add_finance_record(tot_money, 1, cur_user.selected_book,
                                        cur_user.UserID);
        string log_msg = "import " + string(cur_user.selected_book) +
                         " quantity=" + number + " cost=" + tot_cost;
        // std::cout << "end import\n";
        employee_magr.add_employee_log(cur_user.UserID, log_msg.c_str());
        system_magr.add_system_log(cur_user.UserID, log_msg.c_str());

        //   } else if (op == "log") {
        //     Users cur_user = user_magr.get_user();
        //     if (cur_user.privilege != 7) {
        //       throw("Invalid\n");
        //     }
        //     system_magr.show_log();
        //   } else if (op == "report") {
        //     Users cur_user = user_magr.get_user();
        //     if (cur_user.privilege != 7) {
        //       throw("Invalid\n");
        //     }
        //     string report_type;
        //     iss >> report_type;
        //     if (report_type == "finance") {
        //       finance_magr.report_finance();
        //     } else if (report_type == "employee") {
        //       employee_magr.report_employee();
        //     } else {
        //       throw("Invalid\n");
        //     }
      } else {
        throw("Invalid\n");
      }
    } catch (const char *e) {
      // std::cout << "Line" << tmp_cnt << std::endl;
      std::cout << e;
    } catch (const std::exception &e) {
      std::cout << "Invalid\n";
    } catch (...) {
      std::cout << "Invalid\n";
    }
  }
  return 0;
}