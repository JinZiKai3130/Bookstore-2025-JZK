#include "User.hpp"
#include <cstring>

UserManager::UserManager()
    : user_storage("./data/Bookstore_Users")
{ // 拼接路径：./data/Bookstore_Users
    user_storage.initialize_system();
    // 初始化
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
    int cnt = 0;
    if (checkingid == "")
        return false;
    if (checkingid.size() > 30)
        return false;
    while (cnt < checkingid.size())
    {
        if (*str >= '0' && *str <= '9')
        {
            cnt++;
            str++;
            continue;
        }
        if (*str >= 'A' && *str <= 'Z')
        {
            cnt++;
            str++;
            continue;
        }
        if (*str >= 'a' && *str <= 'z')
        {
            cnt++;
            str++;
            continue;
        }
        if (*str == '_')
        {
            cnt++;
            str++;
            continue;
        }
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
        if (static_cast<int>(*str) >= 32 && static_cast<int>(*str) <= 126)
        {
            str++;
            cnt++;
            continue;
        }
        return false;
    }
    return true;
}

void UserManager::useradd(const char *id, const char *pwd, int priv, const char *name, const int your_priv)
{
    if (!check_priv(priv) || priv == 7 || !check_id(id) || !check_id(pwd) || !check_name(name))
    {
        throw("Invalid\n");
    }
    if (your_priv >= 3 && your_priv >= priv)
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
        throw("Invalid\n");
    }
}

void UserManager::userdelete(const char *id, int your_priv)
{
    if (!check_priv(your_priv) || !check_id(id))
    {
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
            throw("Invalid\n");
        }
    }
    else
    {
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
            std::cout << "no user" << std::endl;
            throw("Invalid\n");
        }
        if (your_priv > cur_user[0].privilege)
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
    useradd(id, pwd, 1, name, 3);
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
        if (cur_user.UserID == id)
        {
            throw("Invalid\n");
        }
    }
    user_storage.remove_data(id, cur_user[0]);
}

void UserManager::change_pwd(const char *id, const char *new_pwd, int your_priv, const char *pre_pwd)
{
    if (!check_priv(your_priv))
    {
        throw("Invalid\n");
    }
    std::vector<Users> cur_user = user_storage.search_data(id);
    if (cur_user.empty())
    {
        throw("Invalid\n");
    }
    if (your_priv == 7)
    {
        strcpy(cur_user[0].Password, new_pwd);
    }
    else
    {
        if (strcmp(pre_pwd, cur_user[0].Password) == 0)
        {
            strcpy(cur_user[0].Password, new_pwd);
        }
        else
        {
            throw("Invalid\n");
        }
    }
    UserManager::userdelete(id, 7);
    UserManager::useradd(id, new_pwd, cur_user[0].privilege, cur_user[0].username, 7);
}

void UserManager::select_book(const char *isbn)
{
    strcpy(user_stack.back().selected_book, isbn);
}

char *UserManager::get_select()
{
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