#include "Book.hpp"
#include "User.hpp"

BookManager::BookManager() : book_storage("./data/Bookstore_Users/book_storage"),
                             name_isbn("./data/Bookstore_Users/name_isbn"),
                             author_isbn("./data/Bookstore_Users/author_isbn"),
                             keyword_isbn("./data/Bookstore_Users/keyword_isbn")
{
    book_storage.initialize_system();
    name_isbn.initialize_system();
    author_isbn.initialize_system();
    keyword_isbn.initialize_system();
}

vector<string> BookManager::f_by_name(const char *name)
{
    return name_isbn.search_data(name);
}

vector<string> BookManager::f_by_author(const char *author)
{
    return author_isbn.search_data(author);
}

vector<string> BookManager::f_by_keyword(const char *keyword)
{
    return keyword_isbn.search_data(keyword);
}

vector<Book> BookManager::f_by_isbn(const char *isbn)
{
    return book_storage.search_data(isbn);
}

void BookManager::buy(const char *isbn, const std::string &num)
{
    vector<Book> cur_book = book_storage.search_data(isbn);
    if (num.empty())
        throw("Invalid\n");
    if (num[0] == '0' && num.length() > 1)
        throw("Invalid\n");
    for (char c : num)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            throw("Invalid\n");
        }
    }
    int quest_num = std::stoi(num);
    if (cur_book.empty())
    {
        return;
    }
    if (quest_num > cur_book[0].quantity)
    { // book insufficient
        throw("Invalid\n");
    }
    cur_book[0].quantity -= quest_num;
    BookManager::dele(isbn);
    BookManager::insert(cur_book[0]);
}

void BookManager::select(const char *isbn)
{
    // UserManager::select_book(isbn);
    // 选中图书计入user在main.cpp中实现
    vector<Book> cur_book = book_storage.search_data(isbn);
    if (!cur_book.empty())
        return;

    Book new_book;
    if (!check_isbn(isbn))
        throw("Invalid\n");

    strncpy(new_book.ISBN, isbn, 20);
    insert(new_book);
}

void BookManager::impt(const string &num, const string &tot_cost, const Users &cur_user)
{
    if (!check_quantity(num) || !check_price(tot_cost))
    {
        throw("Invalid\n");
    }
    int qty = std::stoi(num);

    if (cur_user.selected_book[0] == '\0')
    {
        throw("Invalid\n");
    }
    vector<Book> cur_book = f_by_isbn(cur_user.selected_book);
    cur_book[0].quantity += qty;

    BookManager::dele(cur_book[0].ISBN);

    BookManager::insert(cur_book[0]);
}

void BookManager::insert(const Book &new_book)
{
    vector<Book> cur_book = book_storage.search_data(new_book.ISBN);
    if (!cur_book.empty())
    {
        throw("Invalid\n");
    }
    book_storage.add_data(new_book.ISBN, new_book);
    name_isbn.add_data(new_book.name, new_book.ISBN);
    author_isbn.add_data(new_book.author, new_book.ISBN);
    for (const auto &kywd : new_book.keyword)
    {
        keyword_isbn.add_data(kywd, new_book.ISBN);
    }
}

void BookManager::dele(const char *isbn)
{
    vector<Book> cur_book = book_storage.search_data(isbn);
    if (cur_book.empty())
    {
        throw("Invalid\n");
    }
    book_storage.remove_data(isbn, cur_book[0]);
    name_isbn.remove_data(cur_book[0].name, cur_book[0].ISBN);
    author_isbn.remove_data(cur_book[0].author, cur_book[0].ISBN);
    for (const auto &kywd : cur_book[0].keyword)
    {
        keyword_isbn.remove_data(kywd, cur_book[0].ISBN);
    }
}

bool BookManager::check_isbn(const char *isbn)
{
    size_t len = strlen(isbn);
    if (len == 0 || len > 20)
        return false;

    for (size_t i = 0; i < len; ++i)
    {
        unsigned char c = static_cast<unsigned char>(isbn[i]);
        if (c < 32 || c > 126)
        {
            return false; // 不可见字符
        }
    }
    return true;
}

bool BookManager::check_name(const char *name)
{
    size_t len = strlen(name);
    if (len == 0 || len > 60)
        return false;

    for (size_t i = 0; i < len; ++i)
    {
        unsigned char c = static_cast<unsigned char>(name[i]);
        if (c < 32 || c > 126 || c == 34)
        {
            return false;
        }
    }
    return true;
}

bool BookManager::check_keywords(const vector<string> &kwd)
{
    if (kwd.empty())
        return false;

    for (const auto &keyword : kwd)
    {
        size_t len = keyword.length();
        if (len == 0 || len > 60)
            return false;
        for (char c : keyword)
        {
            unsigned char uc = static_cast<unsigned char>(c);
            if (uc < 32 || uc > 126 || uc == 34)
            {
                return false;
            }
        }
    }

    for (size_t i = 0; i < kwd.size(); ++i)
    {
        for (size_t j = i + 1; j < kwd.size(); ++j)
        {
            if (kwd[i] == kwd[j])
            {
                return false;
            }
        }
    }
    return true;
}

vector<string> parse_keywords(const string &keyword_str)
{
    vector<string> keywords;
    if (keyword_str.empty())
        return keywords;

    size_t start = 0;
    size_t end = keyword_str.find('|');

    while (true)
    {
        string keyword;
        if (end == std::string::npos)
        {
            keyword = keyword_str.substr(start);
        }
        else
        {
            keyword = keyword_str.substr(start, end - start);
        }
        keywords.push_back(keyword);
        if (end == std::string::npos)
            break;
        start = end + 1;
        end = keyword_str.find('|', start);
    }
    return keywords;
}

bool BookManager::check_quantity(const std::string &quantity)
{
    size_t len = quantity.length();
    if (len == 0 || len > 10)
        return false;
    for (char c : quantity)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }

    try
    {
        long long val = std::stoll(quantity);
        if (val > 2147483647LL)
            return false;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool BookManager::check_price(const string &pric)
{
    size_t len = pric.length();

    if (len == 0 || len > 13)
        return false;

    int dot_count = 0;
    for (size_t i = 0; i < len; ++i)
    {
        char c = pric[i];
        if (c == '.')
        {
            dot_count++;
            if (dot_count > 1)
                return false;
        }
        else if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }
    if (pric[0] == '.' || pric[len - 1] == '.')
    {
        return false;
    }

    if (dot_count == 1)
    {
        size_t dot_pos = pric.find('.');
        if (len - dot_pos - 1 > 2)
        {
            return false;
        }
    }

    try
    {
        double price = std::stod(pric);
        if (price < 0)
            return false;
        if (dot_count == 1 && pric.find('.') != std::string::npos)
        {
            size_t dot_pos = pric.find('.');
            std::string decimal_part = pric.substr(dot_pos + 1);
            if (decimal_part.length() > 2)
                return false;
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

void BookManager::show(const string &str)
{
    // 假定str已经只有一个搜索条件
    if (str.empty())
    {
        book_storage.traverse();
    }

    string s; // 分类
    if (str[0] != '-')
    {
        throw("Invalid\n");
    }
    int pos = 0;
    for (int i = 1; i < str.length(); i++)
    {
        if (str[i] == '=')
        {
            pos = i;
            break;
        }
        s += str[i];
    }

    if (pos == 0 || pos + 1 == str.length())
    {
        throw("Invalid\n");
    }
    vector<Book> found_book;
    vector<string> isbn_of_book;
    if (s == "ISBN")
    {
        found_book = f_by_isbn(str.substr(pos + 1).c_str());
        if (found_book.empty())
        {
            std::cout << '\n';
        }
        else
        {
            found_book[0].print();
        }
        return;
    }

    if (s == "name")
    {
        isbn_of_book = f_by_name(str.substr(pos + 1).c_str());
    }
    if (s == "author")
    {
        isbn_of_book = f_by_author(str.substr(pos + 1).c_str());
    }
    if (s == "keyword")
    {
        for (int i = pos + 1; i < str.length(); i++)
        {
            if (str[i] == '|')
            {
                throw("Invalid\n");
            }
        }
        isbn_of_book = f_by_keyword(str.substr(pos + 1).c_str());
    }

    if (isbn_of_book.empty())
    {
        std::cout << '\n';
        return;
    }

    for (int i = 0; i < isbn_of_book.size(); i++)
    {
        found_book = f_by_isbn(isbn_of_book[i].c_str());
        found_book[0].print();
        found_book.clear();
    }
}

void BookManager::modify(const string &str, string &selected_isbn)
{
    std::istringstream iss(str);
    std::string parameter;
    // 未选中放在外面判断
    vector<Book> tmp = f_by_isbn(selected_isbn.c_str());
    Book selected_book = tmp[0];
    string s;
    bool vis[5]{};
    while (iss >> parameter)
    {
        if (parameter[0] != '-')
        {
            throw("Invalid\n");
        }

        int pos = 0;
        for (int i = 1; i < str.length(); i++)
        {
            if (str[i] == '=')
            {
                pos = i;
                break;
            }
            s += str[i];
        }

        if (pos == 0 || pos + 1 == str.length())
        {
            // 没有等号或者参数为空
            throw("Invalid\n");
        }

        if (s == "ISBN")
        {
            if (vis[0])
            {
                throw("Invalid\n");
            }

            string new_isbn;
            iss >> new_isbn;
            if (new_isbn == selected_isbn)
            {
                throw("Invalid\n");
            }
            strncpy(selected_book.ISBN, new_isbn.c_str(), 20);
            dele(selected_isbn.c_str());
            insert(selected_book);

            selected_isbn = new_isbn; // 传回当前用户选择的书
            vis[0] = true;
            break;
        }
        if (s == "name")
        {
            if (vis[1])
            {
                throw("Invalid\n");
            }
            string new_name;
            iss >> new_name;
            strncpy(selected_book.name, new_name.c_str(), 60);
            dele(selected_isbn.c_str());
            insert(selected_book);

            vis[1] = true;
        }
        if (s == "author")
        {
            if (vis[2])
            {
                throw("Invalid\n");
            }
            string new_author;
            iss >> new_author;
            strncpy(selected_book.name, new_author.c_str(), 60);
            dele(selected_isbn.c_str());
            insert(selected_book);

            vis[2] = true;
        }
        if (s == "keyword")
        {
            if (vis[3])
            {
                throw("Invalid\n");
            }
            vector<string> new_keywords;
            int cnt = 0;
            for (int i = pos + 1; i < str.length(); i++)
            {
                iss >> new_keywords[cnt];
                if (str[i] == '|')
                {
                    for (int j = 0; j < cnt; j++)
                    {
                        if (new_keywords[cnt] == new_keywords[j])
                        {
                            // 相同的关键词
                            throw("Invalid\n");
                        }
                    }
                    cnt++;
                }
            }
            selected_book.keyword = new_keywords;
            dele(selected_isbn.c_str());
            insert(selected_book);
            vis[3] = true;
        }
    }
}