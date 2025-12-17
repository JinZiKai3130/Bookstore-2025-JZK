#include "Book.hpp"
#include "User.hpp"

BookManager::BookManager() : 
                book_storage("book_storage"), 
                name_isbn("name_isbn"), 
                author_isbn("author_isbn"), 
                keyword_isbn("keyword_isbn") {
    book_storage.initialize_system();
    name_isbn.initialize_system();
    author_isbn.initialize_system();
    keyword_isbn.initialize_system();
}

vector<char*>& BookManager::f_by_name(const char* name) {
    return name_isbn.search_data(name);
}

vector<char*>& BookManager::f_by_author(const char* author) {
    return author_isbn.search_data(author);
}

vector<char*>& BookManager::f_by_keyword(const char* keyword) {
    return keyword_isbn.search_data(keyword);
}

vector<Book>& BookManager::f_by_isbn(const char* isbn) {
    return book_storage.search_data(isbn);
}

void BookManager::buy(const char* isbn, const std::string& num) {
    vector<Book> cur_book = book_storage.search_data(isbn);
    if (num.empty()) throw("Invalid\n");
    if (num[0] == '0' && num.length() > 1) throw("Invalid\n");
    for (char c : str) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            throw("Invalid\n");
        }
    }
    int quest_num = std::stoi(num);
    if (cur_book.empty()) {
        return;
    }
    if (quest_num > cur_book[0].quantity) { // book insufficient
        throw("Invalid\n");
    }
    cur_book[0].quantity -= quest_num;
    BookManager::dele(isbn);
    BookManager::insert(cur_book[0]);
}

void BookManager::select(const char* isbn) {
    user_stack.back().select_book(isbn);
    vector<Book> cur_book = book_storage.search_data(isbn);
    if (!cur_book.empty()) return;

    Book new_book();
    if (!check_isbn(isbn)) throw("Invalid\n");
    
    strcpy(new_book.ISBN, isbn);
    insert(new_book);
}

void BookManager::impt(const string& num, const string& tot_cost) {
    if (!check_quantity(num) || !check_price(tot_cost)) {
        throw("Invalid\n");
    }
    int qty = std::stoi(num);
    double tot_cost = std::stod(tot_cost);

    if (user_stack.back().selected_book[0] == '\0') {
        throw("Invalid\n");
    }
    vector<Book> cur_book = f_by_isbn(user_stack.back().selected_book);
    cur_book[0].quantity += qty;
    BookManager::dele(cur_book[0].isbn);
    BookManager::insert(cur_book[0]);
}

void BookManager::insert(const Book& new_book) {
    vector<Book> cur_book = book_storage.search_data(new_book.isbn);
    if (!cur_book.empty()) {
        throw("Invalid\n");
    }
    book_storage.add_data(new_book.ISBN, new_book);
    name_isbn.add_data(new_book.name, new_book.ISBN);
    author_isbn.add_data(new_book.author, new_book.ISBN);
    for (const auto& kywd : new_book.keyword) {
        keyword_isbn.add_data(kywd, new_book.ISBN);
    }
}

void BookManager::dele(const char* isbn) {
    vector<Book> cur_book = book_storage.search_data(isbn);
    if (cur_book.empty()) {
        throw("Invalid\n");
    }
    book_storage.remove_data(isbn);
    name_isbn.remove_data(cur_book[0].name);
    author_isbn.remove_data(cur_book[0].author);
    for (const auto& kywd : cur_book[0].keyword) {
        keyword_isbn.add_data(kywd);
    }
}

bool BookManager::check_isbn(const char* isbn) {
    size_t len = strlen(isbn);
    if (len == 0 || len > 20) return false;
        
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = static_cast<unsigned char>(isbn[i]);
        if (c < 32 || c > 126) {
            return false; // 不可见字符
        }
    }
    return true;
}

bool BookManager::check_name(const char* name) {
    size_t len = strlen(name);
    if (len == 0 || len > 60) return false;

    for (size_t i = 0; i < len; ++i) {
        unsigned char c = static_cast<unsigned char>(name[i]);
        if (c < 32 || c > 126 || c == 34) {
            return false;
        }
    }
    return true;
}

bool BookManager::check_keywords(const vector<string>& kwd) {
    if (kwd.empty()) return false;
        
    for (const auto& keyword : kwd) {
        size_t len = keyword.length();
        if (len == 0 || len > 60) return false;
        for (char c : keyword) {
            unsigned char uc = static_cast<unsigned char>(c);
            if (uc < 32 || uc > 126 || uc == 34) {
                return false;
            }
        }
    }

    for (size_t i = 0; i < kwd.size(); ++i) {
        for (size_t j = i + 1; j < kwd.size(); ++j) {
            if (kwd[i] == kwd[j]) {
                return false;
            }
        }
    }
    return true;
}

vector<string>& parse_keywords(const string& keyword_str) {
    vector<string> keywords;
    if (keyword_str.empty()) return keywords;
        
    size_t start = 0;
    size_t end = keyword_str.find('|');
        
    while (true) {
        string keyword;
        if (end == std::string::npos) {
            keyword = keyword_str.substr(start);
        }
        else {
            keyword = keyword_str.substr(start, end - start);
        }
        keywords.push_back(keyword);
        if (end == std::string::npos) break;
        start = end + 1;
        end = keyword_str.find('|', start);
    }
    return keywords;
}

bool BookManager::check_quantity(const std::string& quantity) {
    size_t len = quantity.length();
    if (len == 0 || len > 10) return false;
    for (char c : quantity) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }

    try {
        long long val = std::stoll(quantity);
        if (val > 2147483647LL) return false;
    } catch (...) {
        return false;
    }
    return true;
}

bool BookManager::check_price(const string& pric) {
    size_t len = price_str.length();
    
    if (len == 0 || len > 13) return false;

    int dot_count = 0;
    for (size_t i = 0; i < len; ++i) {
        char c = price_str[i];
        if (c == '.') {
            dot_count++;
            if (dot_count > 1) return false;
        }
        else if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    if (price_str[0] == '.' || price_str[len - 1] == '.') {
        return false;
    }

    if (dot_count == 1) {
        size_t dot_pos = price_str.find('.');
        if (len - dot_pos - 1 > 2) {
            return false;
        }
    }
    
    try {
        double price = std::stod(price_str);
        if (price < 0) return false;
        if (dot_count == 1 && price_str.find('.') != std::string::npos) {
            size_t dot_pos = price_str.find('.');
            std::string decimal_part = price_str.substr(dot_pos + 1);
            if (decimal_part.length() > 2) return false;
        }
    } catch (...) {
        return false;
    }
    return true;
}

void BookManager::show(const string& str) {
    
}

void BookManager::modify(const string& str) {

}