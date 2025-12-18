#ifndef BOOK_H
#define BOOK_H

#include <cstring>
#include <string>
#include <vector>
using namespace vector;
using namespace string;

// ISBN-Book映射
struct Book {
    char ISBN[21];
    char name[61];
    char author[61];
    vector<std::string> keyword;
    double price;
    int quantity;

    Book() {
        ISBN[0] = '\0';
        name[0] = '\0';
        author[0] = '\0';
        price = 0.00;
        quantity = 0;
        
    }

    Book(const char* new_ISBN,
         const char* new_name,
         const char* new_author,
         const vector<string>& new_keyword,
         const double& new_price,
         const int& new_quantity) {
        std::strncpy(ISBN, new_ISBN, 20);
        ISBN[20] = '\0';
        std::strncpy(name, new_name, 60);
        name[60] = '\0';
        std::strncpy(author, new_author, 60);
        author[60] = '\0';
        keyword = new_keyword;
        price = new_price;
        quantity = new_quantity;
    }

    Book& operator=(const Book& other) {
        std::strncpy(ISBN, other.ISBN, 20); ISBN[20] = '\0';
        std::strncpy(name, other.name, 60); name[60] = '\0';
        std::strncpy(author, other.author, 60); author[60] = '\0';
        keyword = other.keyword;
        price = other.price;
        quantity = other.quantity;
        return *this;
    }

    bool operator==(const Book& other) const {
        return std::strcmp(ISBN, other.ISBN) == 0;
    }
    
    bool operator<(const Book& other) const {
        return std::strcmp(ISBN, other.ISBN) < 0;
    }

    void print() {
        std::cout << ISBN << "\t" << name <<"\t" << author << "\t";
        for (const auto& word : keyword) {
            std::cout << word << "\t";
        }
        std::cout << price << "\t" << quantity << "\n";
    }
};

class BookManager {
    BlockStorageSystem<Book> book_storage;
    BlockStorageSystem<char*> name_isbn;
    BlockStorageSystem<char*> author_isbn;
    BlockStorageSystem<char*> keyword_isbn;

public:
    BookManager();
    vector<char*>& f_by_name(const char* name);
    vector<char*>& f_by_author(const char* author);
    vector<char*>& f_by_keyword(const char* keywords);
    vector<Book>& f_by_isbn(const char* isbn);
    void buy(const char* isbn, const string& num);
    void select(const char* isbn);
    void insert(const char* isbn, const char* name,
                const char* author, const vector<string>& keywords,
                const double& pric, const int& num);
    void dele(const char* isbn);
    void impt(const int& num, const double& tot_cost);
    vector<string> parse_keywords(const string& keyword_str);
    bool check_isbn(const char* isbn);
    bool check_name(const char* name);
    bool check_keywords(const vector<string>& kwd);
    bool check_price(const string& pric);
    void show(const string& str);
    void modify(const string& str);
};

#endif // BOOK_H