#include "User.hpp"
#include <cstring>

UserManager::UserManager()
    : user_storage("./data/Users.dat")
{
    user_storage.initialize_system();
    auto check = user_storage.search_data("root");
    if (!check.empty())
        return;
    string id = "root";
    string pwd = "sjtu";
    Users user(id.c_str(), pwd.c_str(), id.c_str(), 7);
    user_storage.add_data(id, user);
}

bool UserManager::check_priv(const int &priv)
{
    if (priv == 1 || priv == 3 || priv == 7)
        return true;
    else
        return false;
}

bool UserManager::check_id(const char *str)
{
    string checkingid = str;
    // std::cout << checkingid << std::endl;
    int cnt = 0;
    if (checkingid == "")
        return false;
    if (checkingid.size() > 30)
        return false;
    while (cnt < checkingid.size())
    {
        if (checkingid[cnt] >= '0' && checkingid[cnt] <= '9')
        {
            cnt++;
            str++;
            continue;
        }
        if (checkingid[cnt] >= 'A' && checkingid[cnt] <= 'Z')
        {
            cnt++;
            str++;
            continue;
        }
        if (checkingid[cnt] >= 'a' && checkingid[cnt] <= 'z')
        {
            cnt++;
            str++;
            continue;
        }
        if (checkingid[cnt] == '_')
        {
            // std::cout << "____\n";
            cnt++;
            str++;
            continue;
        }
        // std::cout << "thisway = " << checkingid[cnt] << '\n';
        return false;
    }
    return true;
}

bool UserManager::check_name(const char *str)
{
    int cnt = 0;
    if (*str == '\0')
        return false;
    while (*str != '\0')
    {
        if (cnt > 30)
            return false;
        if (static_cast<int>(*str) > 32 && static_cast<int>(*str) <= 126)
        {
            str++;
            cnt++;
            continue;
        }
        return false;
    }
    return true;
}

void UserManager::useradd(const char *id, const char *pwd, int priv, const char *name, const int your_priv, const bool modify)
{
    // std::cout << "cur_priv = " << your_priv << '\n';
    if (!check_priv(priv) || !check_id(id) || !check_id(pwd) || !check_name(name))
    {
        // std::cout << "grammar" << std::endl;
        throw("Invalid\n");
    }
    if ((your_priv >= 3 && your_priv > priv) || modify)
    {
        std::vector<Users> cur_user = user_storage.search_data(id);
        if (cur_user.empty())
        { // 没有相同的id
            Users user(id, pwd, name, priv);
            user_storage.add_data(id, user);
        }
        else
        {
            // std::cout << "same name" << std::endl;
            throw("Invalid\n");
        }
    }
    else
    {
        // std::cout << "priv" << std::endl;
        throw("Invalid\n");
    }
}

void UserManager::userdelete(const char *id, int your_priv)
{
    if (!check_priv(your_priv) || !check_id(id))
    {
        // std::cout << "grammar" << std::endl;
        throw("Invalid\n");
    }
    if (your_priv == 7)
    {
        std::vector<Users> cur_user = user_storage.search_data(id);
        if (!cur_user.empty())
        { // 有相同的id，正确删除
            user_storage.remove_data(id, cur_user[0]);
        }
        else
        {
            // std::cout << "no user\n";
            throw("Invalid\n");
        }
    }
    else
    {
        // std::cout << "Invlid here" << std::endl;
        throw("Invalid\n");
    }
}

std::vector<Users> UserManager::finduser(const char *id)
{
    return user_storage.search_data(id);
}

void UserManager::login(const char *id, const char *pwd)
{
    int your_priv;
    (user_stack.empty()) ? (your_priv = 0) : (your_priv = user_stack.back().privilege);

    std::vector<Users> cur_user = finduser(id);
    // std::cout << "id = " << id << " password = " << pwd << '\n';
    if (your_priv >= 0)
    {
        if (cur_user.empty())
        { // 没有该用户
            // std::cout << "no user" << std::endl;
            throw("Invalid\n");
        }
        if (your_priv > cur_user[0].privilege && strlen(pwd) == 0) // 神了
        {
            user_stack.push_back(cur_user[0]);
        }
        else if (strcmp(pwd, cur_user[0].Password) == 0)
        {
            // 成功登录
            user_stack.push_back(cur_user[0]);
        }
        else
        { // 密码错误
            throw("Invalid\n");
        }
    }
    else
    { // 权限完全错误
        throw("Invalid\n");
    }
}

void UserManager::logout()
{
    if (!user_stack.empty())
    {
        user_stack.pop_back();
    }
    else
    {
        throw("Invalid\n");
    }
}

void UserManager::regist(const char *id, const char *pwd, const char *name)
{
    useradd(id, pwd, 1, name, 3, 1);
}

void UserManager::delt(const char *id)
{
    std::vector<Users> cur_user = finduser(id);
    if (cur_user.empty())
    {
        throw("Invalid\n");
    }
    for (const auto &cur_user : user_stack)
    {
        if (strcmp(cur_user.UserID, id) == 0)
        {
            throw("Invalid\n");
        }
    }
    user_storage.remove_data(id, cur_user[0]);
}

void UserManager::change_pwd(const char *id, const char *new_pwd, int your_priv, const char *pre_pwd)
{
    if (!check_id(id) || !check_id(new_pwd))
    {
        throw("Invalid\n");
    }
    if (strlen(pre_pwd) > 0 && !check_id(pre_pwd))
    {
        throw("Invalid\n");
    }
    // std::cout << "1" << std::endl;
    if (!check_priv(your_priv))
    {
        // std::cout << "priv" << std::endl;
        throw("Invalid\n");
    }
    // std::cout << "2" << std::endl;
    std::vector<Users> cur_user = user_storage.search_data(id);
    if (cur_user.empty())
    {
        // std::cout << "no user" << std::endl;
        throw("Invalid\n");
    }
    // std::cout << "3" << std::endl;
    Users &current = user_stack.back();
    if (your_priv == 7)
    {
        // std::cout << "7" << std::endl;
        strcpy(cur_user[0].Password, new_pwd);
    }
    else
    {
        if (strlen(pre_pwd) == 0)
        {
            throw("Invalid\n");
        }
        if (strcmp(pre_pwd, cur_user[0].Password) == 0)
        {
            strcpy(cur_user[0].Password, new_pwd);
        }
        else
        {
            // std::cout << "invalid password" << std::endl;
            throw("Invalid\n");
        }
    }
    // std::cout << "4" << std::endl;
    UserManager::userdelete(id, 7);
    UserManager::useradd(id, new_pwd, cur_user[0].privilege, cur_user[0].username, 7, 1);
}

void UserManager::select_book(const char *isbn, const char *origin_isbn)
{
    // std::cout << "origin_isbn = " << origin_isbn << '\n';
    for (auto it = user_stack.begin(); it != user_stack.end(); it++)
    {
        // std::cout << "it->selected_book = " << it->selected_book << '\n';
        if (strcmp(it->selected_book, origin_isbn) == 0)
        {
            strcpy(it->selected_book, isbn);
        }
    }
    strcpy(user_stack.back().selected_book, isbn);
    // std::cout << "selected-this " << user_stack.back().selected_book << '\n';
}

char *UserManager::get_select()
{
    if (user_stack.back().selected_book)
        return user_stack.back().selected_book;
}

Users UserManager::get_user()
{
    if (user_stack.empty())
    {
        throw("Invalid\n");
    }
    return user_stack.back();
}