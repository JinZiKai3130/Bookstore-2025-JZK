#include "Book.hpp"
#include "User.hpp"

BookManager::BookManager() : book_storage("book_storage.dat"),
                             name_isbn("name_isbn.dat"),
                             author_isbn("author_isbn.dat"),
                             keyword_isbn("keyword_isbn.dat")
{
    book_storage.initialize_system();
    name_isbn.initialize_system();
    author_isbn.initialize_system();
    keyword_isbn.initialize_system();
}

vector<name_author_isbn> BookManager::f_by_name(const char *name)
{
    return name_isbn.search_data(name);
}

vector<name_author_isbn> BookManager::f_by_author(const char *author)
{
    return author_isbn.search_data(author);
}

vector<keyword_isbn_> BookManager::f_by_keyword(const char *keyword)
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
    // std::cout << "number = " << num << '\n';
    if (num.empty())
        throw("Invalid\n");
    if (num[0] == '0' && num.length() >= 1)
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
        throw("Invalid\n");
    }
    if (quest_num > cur_book[0].quantity)
    { // book insufficient
        throw("Invalid\n");
    }
    cur_book[0].quantity -= quest_num;
    BookManager::dele(isbn);
    BookManager::insert(cur_book[0]);
    double tot = static_cast<double>(quest_num) * cur_book[0].price;
    std::cout << std::fixed << std::setprecision(2) << tot << '\n';
}

void BookManager::select(const char *isbn)
{
    // UserManager::select_book(isbn);
    // 选中图书计入user在main.cpp中实现
    vector<Book> cur_book = book_storage.search_data(isbn);
    if (!cur_book.empty())
    {
        // std::cout << "same name\n";
        return;
    }

    Book new_book;
    if (!check_isbn(isbn))
        throw("Invalid\n");
    // std::cout << "insert a book\n";
    strncpy(new_book.ISBN, isbn, 20);
    // new_book.print();
    insert(new_book);
}

void BookManager::impt(const string &num, const Users &cur_user)
{
    if (!check_quantity(num))
    {
        throw("Invalid\n");
    }
    int qty = std::stoi(num);

    if (cur_user.selected_book[0] == '\0' || qty == 0)
    {
        throw("Invalid\n");
    }
    vector<Book> cur_book = f_by_isbn(cur_user.selected_book);
    cur_book[0].quantity += qty;
    // cur_book[0].print();
    dele(cur_book[0].ISBN);
    // cur_book[0].print();
    insert(cur_book[0]);
}

void BookManager::insert(const Book &new_book)
{
    vector<Book> cur_book = book_storage.search_data(new_book.ISBN);
    if (!cur_book.empty())
    {
        throw("Invalid\n");
    }
    // std::cout << "inserting\n";
    book_storage.add_data(new_book.ISBN, new_book);
    // std::cout << "insert ISBN = " << new_book.ISBN << "\n";
    // std::cout << "insert name = " << new_book.name << "\n";
    // std::cout << "insert author = " << new_book.author << "\n";
    name_author_isbn new_name(new_book.name, new_book.ISBN);
    name_author_isbn new_author(new_book.author, new_book.ISBN);
    if (strlen(new_book.name) != 0)
        name_isbn.add_data(new_book.name, new_name);
    // std::cout << "insert author\n";
    if (strlen(new_book.author) != 0)
        author_isbn.add_data(new_book.author, new_author);
    // std::cout << "insert keywords\n";
    if (strlen(new_book.keyword) != 0)
    {
        std::vector<std::array<char, 61>> kwds = parse_keywords(new_book.keyword);
        for (const auto &kwd : kwds)
        {
            char new_keyword[61];
            std::copy_n(kwd.begin(), 60, new_keyword);
            keyword_isbn.add_data(new_keyword, keyword_isbn_(new_keyword, new_book.ISBN));
        }
    }
}

void BookManager::dele(const char *isbn)
{
    vector<Book> cur_book = book_storage.search_data(isbn);
    if (cur_book.empty() || cur_book.size() > 1)
    {
        throw("Invalid\n");
    }
    // std::cout << "ok here1\n";
    book_storage.remove_data(isbn, cur_book[0]);
    // std::cout << "cur_book[0].name = " << cur_book[0].name << "end" << std::endl;
    name_author_isbn cur_name(cur_book[0].name, cur_book[0].ISBN);
    name_author_isbn cur_author(cur_book[0].author, cur_book[0].ISBN);
    if (strlen(cur_book[0].name) != 0)
        name_isbn.remove_data(cur_book[0].name, cur_name);
    // std::cout << "cur_book[0].author = " << cur_book[0].author << "end" << std::endl;
    if (strlen(cur_book[0].author) != 0)
        author_isbn.remove_data(cur_book[0].author, cur_author);
    // std::cout << "cur_book[0].keyword = " << cur_book[0].keyword.empty() << " end" << std::endl;
    if (strlen(cur_book[0].keyword) != 0)
    {
        std::vector<std::array<char, 61>> kwds = parse_keywords(cur_book[0].keyword);
        for (const auto &kwd : kwds)
        {
            char new_keyword[61];
            std::copy_n(kwd.begin(), 61, new_keyword);
            keyword_isbn.remove_data(new_keyword, keyword_isbn_(new_keyword, cur_book[0].ISBN));
        }
    }
    // std::cout << "go here\n";
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

vector<std::array<char, 61>> BookManager::parse_keywords(const string &keyword_str)
{
    vector<std::array<char, 61>> keywords;

    if (keyword_str.empty())
        return keywords;

    if (keyword_str.front() == '|' || keyword_str.back() == '|' ||
        keyword_str.find("||") != string::npos)
    {
        throw "Invalid\n";
    }

    size_t start = 0;
    size_t end = 0;
    vector<string> temp_keywords;

    while (end <= keyword_str.length())
    {
        if (end == keyword_str.length() || keyword_str[end] == '|')
        {
            if (end == start)
            {
                throw("Invalid\n");
            }

            string keyword = keyword_str.substr(start, end - start);

            if (keyword.length() == 0 || keyword.length() > 60)
            {
                throw("Invalid\n");
            }

            for (char c : keyword)
            {
                if (c < 0 || c >= 128)
                {
                    throw("Invalid\n");
                }
                if (c <= 31 || c == 34)
                {
                    throw("Invalid\n");
                }
            }

            if (find(temp_keywords.begin(), temp_keywords.end(), keyword) != temp_keywords.end())
            {
                throw("Invalid\n");
            }

            temp_keywords.push_back(keyword);
            start = end + 1;
        }
        end++;
    }

    for (const auto &kw : temp_keywords)
    {
        std::array<char, 61> keyword_array;
        memset(keyword_array.data(), 0, 61);
        strncpy(keyword_array.data(), kw.c_str(), 60);
        keywords.push_back(keyword_array);
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
        return;
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
    vector<keyword_isbn_> book_by_keywords;
    vector<name_author_isbn> book_by_name;
    vector<name_author_isbn> book_by_author;
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
        string new_name = str.substr(pos + 1);
        if (new_name[0] != '\"' || new_name[new_name.length() - 1] != '\"')
        {
            throw("Invalid\n");
        }
        new_name.erase(new_name.size() - 1);
        new_name.erase(new_name.begin());
        book_by_name = f_by_name(new_name.c_str());
        for (const auto &tmp : book_by_name)
        {
            isbn_of_book.push_back(tmp.ISBN);
        }
    }
    if (s == "author")
    {
        string new_author = str.substr(pos + 1);
        if (new_author[0] != '\"' || new_author[new_author.length() - 1] != '\"')
        {
            throw("Invalid\n");
        }
        new_author.erase(new_author.size() - 1);
        new_author.erase(new_author.begin());
        book_by_author = f_by_author(new_author.c_str());
        for (const auto &tmp : book_by_author)
        {
            isbn_of_book.push_back(tmp.ISBN);
        }
    }
    if (s == "keyword")
    {
        for (int i = pos + 1; i < str.length(); i++)
        { // no many keywords together
            if (str[i] == '|')
            {
                throw("Invalid\n");
            }
        }
        string new_kwd = str.substr(pos + 1);
        if (new_kwd[0] != '\"' || new_kwd[new_kwd.length() - 1] != '\"')
        {
            throw("Invalid\n");
        }
        new_kwd.erase(new_kwd.size() - 1);
        new_kwd.erase(new_kwd.begin());
        book_by_keywords = f_by_keyword(new_kwd.c_str());

        for (const auto &tmp : book_by_keywords)
        {
            isbn_of_book.push_back(tmp.ISBN);
        }
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
    // std::cout << "selected_isbn = " << selected_isbn << '\n';
    vector<Book> tmp = f_by_isbn(selected_isbn.c_str());
    Book selected_book = tmp[0];
    string s;
    bool vis[5]{};
    string newed_isbn = selected_isbn;
    // std::cout << "str = " << str << '\n';
    while (iss >> parameter)
    {
        // std::cout << "parameter = " << parameter << std::endl;
        if (parameter[0] != '-')
        {
            // std::cout << "--------\n";
            throw("Invalid\n");
        }
        s.clear();
        int pos = 0;
        for (int i = 1; i < parameter.length(); i++)
        {
            if (parameter[i] == '=')
            {
                pos = i;
                break;
            }
            s += parameter[i];
        }
        string new_element = "";

        // std::cout << s << std::endl;
        if (s == "ISBN")
        {
            if (vis[0])
            {
                throw("Invalid\n");
            }

            if (pos + 1 == parameter.length())
            {
                throw("Invalid\n");
            }
            for (int i = pos + 1; i < parameter.length(); i++)
            {
                new_element += parameter[i];
            }
            string new_isbn = new_element;
            if (new_isbn == selected_isbn)
            {
                throw("Invalid\n");
            }
            // std::cout << "before copy\n";
            strncpy(selected_book.ISBN, new_isbn.c_str(), 20);
            newed_isbn = new_isbn;
            vis[0] = true;
            break;
        }
        if (s == "name")
        {
            if (vis[1])
            {
                throw("Invalid\n");
            }
            if (parameter[pos + 1] != '\"' || parameter[parameter.length() - 1] != '\"')
            {
                throw("Invalid\n");
            }
            if (pos + 1 == parameter.length() - 2)
            {
                throw("Invalid\n");
            }
            for (int i = pos + 2; i < parameter.length() - 1; i++)
            {
                new_element += parameter[i];
            }
            string new_name = new_element;
            strncpy(selected_book.name, new_name.c_str(), 60);

            vis[1] = true;
        }
        if (s == "author")
        {
            if (vis[2])
            {
                throw("Invalid\n");
            }
            if (parameter[pos + 1] != '\"' || parameter[parameter.length() - 1] != '\"')
            {
                throw("Invalid\n");
            }
            if (pos + 1 == parameter.length() - 2)
            {
                throw("Invalid\n");
            }
            for (int i = pos + 2; i < parameter.length() - 1; i++)
            {
                new_element += parameter[i];
            }
            string new_author = new_element;
            strncpy(selected_book.author, new_author.c_str(), 60);

            vis[2] = true;
        }
        if (s == "keyword")
        {
            // std::cout << "new element = " << new_element << '\n';
            if (vis[3])
            {
                throw("Invalid\n");
            }
            if (parameter[pos + 1] != '\"' || parameter[parameter.length() - 1] != '\"')
            {
                throw("Invalid\n");
            }
            if (pos + 1 == parameter.length() - 2)
            {
                throw("Invalid\n");
            }
            for (int i = pos + 2; i < parameter.length() - 1; i++)
            {
                new_element += parameter[i];
            }

            vector<std::array<char, 61>> new_keywords = parse_keywords(new_element);

            strncpy(selected_book.keyword, new_element.c_str(), 60);
            vis[3] = true;
        }

        if (s == "price")
        {
            // std::cout << "changeprice\n";
            if (vis[4])
            {
                throw("Invalid");
            }
            if (pos == parameter.length() - 1)
            {
                throw("Invalid\n");
            }
            for (int i = pos + 1; i < parameter.length(); i++)
            {
                new_element += parameter[i];
            }
            if (!check_price(new_element))
            {
                throw("Invalid\n");
            }
            selected_book.price = std::stod(new_element);

            vis[4] = true;
        }
    }
    dele(selected_isbn.c_str());
    insert(selected_book);
    selected_isbn = newed_isbn;
}