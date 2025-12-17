#include "User.h"

UserManager::UserManager() 
    : user_storage("Bookstore_Users") {  // 拼接路径：./data/Bookstore_Users
    user_storage.initialize_system();

    // 初始化
    Users user("root", "sjtu", 7, "root");
    user_storage.add_data(id, user);
}

bool UserManager::check_priv(int& priv) {
    if (priv == 1 || priv == 3 || priv == 7) return true;
    else return false;
}


bool UserManager::check_id(const char* str) {
    int cnt = 0;
    if (*str == '\0') return false;
    while (*str != '\0') {
        str++;
        cnt++;
        if (cnt > 30) return false;
        if (*str >= '0' && *str <= '9') {
            continue;
        }
        if (*str >= 'A' && *str <= 'Z') {
            continue;
        }
        if (*str >= 'a' && *str <= 'z') {
            continue;
        }
        if (*str == '_') {
            continue;
        }
        return false;
    }
    return true;
}

bool UserManager::check_name(const char* str) {
    int cnt = 0;
    if (*str == '\0') return false;
    while (*str != '\0') {
        str++;
        cnt++;
        if (cnt > 30) return false;
        if (static_cast<int>(*str) >= 32 && static_cast<int>(*str) <= 126) {
            continue;
        }
        return false;
    }
    return true;
}


void UserManager::useradd(const char* id, const char* pwd, int priv, const char* name, int your_priv) {
    if (!check_priv(priv) || priv == 7 || !check_id(id) || !check_id(pwd) || !check_name(name)) {
        throw("Invalid\n");
    }
    if (your_priv >= 3 && your_priv >= priv) {
        std::vector<Users> cur_user = user_storage.search_data(id);
        if (cur_user.empty()) { // 没有相同的id
            Users user(id, pwd, priv, name);
            user_storage.add_data(id, user);
        }
        else {
            throw("Invalid\n");
        }
    }
    else {
        throw("Invalid\n");
    }
}

void UserManager::userdelete(const char* id, int your_priv) {
    if (!check_priv(priv) || !check_id(id)) {
        throw("Invalid\n");
    }
    if (your_priv == 7) {
        std::vector<Users> cur_user = user_storage.search_data(id);
        if (!cur_user.empty()) { // 有相同的id，正确删除
            user_storage.remove_data(id, cur_user[0]);
        }
        else {
            throw("Invalid\n");
        }
    }
    else {
        throw("Invalid\n");
    }
}

std::vector<Users> UserManager::finduser(const char* id) {
    return user_storage.search_data(id);
}

void UserManager::login(const int your_priv, const char* id, const char* pwd) {
    if (your_priv >= 0) {
        std::vector<Users> cur_user = user_storage.search_data(id);
        if (cur_user.empty()) { // 没有该用户
            throw("Invalid\n");
        }
        if (strcmp(pwd, cur_user[0].Password) == 0) {
            // 成功登录
            user_stack.push_back(cur_user);
        }
        else { // 密码错误
            throw("Invalid\n");
        }
    }
    else { // 权限完全错误
        throw("Invalid\n");
    }
}

void UserManager::logout() {
    if (!user_stack.empty()) {
        user_stack.pop_back();
    }
    else {
        throw("Invalid\n");
    }
}

void UserManager::regist(const char* id, const char* pwd, const char* name) {
    UserManager::useradd(id, pwd, 1, name, 3);
}

void UserManager::change_pwd(const char* id, const char* pre_pwd, const char* new_pwd, int your_priv) {
    if (!check_priv(your_priv)) {
        throw("Invalid\n");
    }
    std::vector<Users> cur_user = user_storage.search_data(id);
    if (cur_user.empty()) {
        throw("Invalid\n");
    }
    if (your_priv == 7) {
        strcpy(cur_user[0].Password, new_pwd);
    }
    else {
        if (strcmp(pre_pwd, cur_user[0].Password) == 0) {
            strcpy(cur_user[0].Password, new_pwd);
        }
        else {
            throw("Invalid\n");
        }
    }
    UserManager::userdelete(id, 7);
    UserManager::useradd(id, new_pwd, cur_user[0].privilege, cur_user[0].name, 7);
}

void UserManager::select_book(const char* isbn) {
    strcpy(user_stack.back().selected_book, isbn);
}