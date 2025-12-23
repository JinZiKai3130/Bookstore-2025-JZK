#ifndef BOOK_H
#define BOOK_H

#include <cstring>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <iomanip>
#include "User.hpp"
using std::string;
using std::vector;

// ISBN-Book映射
struct Book
{
    char ISBN[21];
    char name[61];
    char author[61];
    char keyword[61];
    double price;
    int quantity;

    Book()
    {
        ISBN[0] = '\0';
        name[0] = '\0';
        author[0] = '\0';
        keyword[0] = '\0';
        price = 0.00;
        quantity = 0;
    }

    Book(const char *new_ISBN,
         const char *new_name,
         const char *new_author,
         const char *new_keyword,
         const double &new_price,
         const int &new_quantity)
    {
        std::strncpy(ISBN, new_ISBN, 20);
        ISBN[20] = '\0';
        std::strncpy(name, new_name, 60);
        name[60] = '\0';
        std::strncpy(author, new_author, 60);
        author[60] = '\0';
        std::strncpy(keyword, new_keyword, 60);
        keyword[60] = '\0';
        price = new_price;
        quantity = new_quantity;
    }

    Book &operator=(const Book &other)
    {
        std::strncpy(ISBN, other.ISBN, 20);
        ISBN[20] = '\0';
        std::strncpy(name, other.name, 60);
        name[60] = '\0';
        std::strncpy(author, other.author, 60);
        author[60] = '\0';
        std::strncpy(keyword, other.keyword, 60);
        keyword[60] = '\0';
        price = other.price;
        quantity = other.quantity;
        return *this;
    }

    bool operator==(const Book &other) const
    {
        return std::strcmp(ISBN, other.ISBN) == 0;
    }

    bool operator<(const Book &other) const
    {
        return std::strcmp(ISBN, other.ISBN) < 0;
    }

    void print()
    {
        std::cout << ISBN << '\t' << name << '\t' << author << '\t' << keyword << '\t'
                  << std::fixed << std::setprecision(2) << price << '\t' << quantity << '\n';
    }
};

struct name_author_isbn
{
    char ISBN[21];
    char index[61];

    name_author_isbn()
    {
        ISBN[0] = '\0';
        index[0] = '\0';
    }

    name_author_isbn(const char *new_index, const char *new_ISBN)
    {
        std::strncpy(ISBN, new_ISBN, 20);
        ISBN[20] = '\0';
        std::strncpy(index, new_index, 60);
        index[60] = '\0';
    }

    name_author_isbn &operator=(const name_author_isbn &other)
    {
        if (this != &other)
        {
            std::strncpy(ISBN, other.ISBN, 20);
            ISBN[20] = '\0';
            std::strncpy(index, other.index, 60);
            index[60] = '\0';
        }
        return *this;
    }

    bool operator==(const name_author_isbn &other) const
    {
        return std::strcmp(ISBN, other.ISBN) == 0 &&
               std::strcmp(index, other.index) == 0;
    }

    bool operator<(const name_author_isbn &other) const
    {
        int cmp = std::strcmp(index, other.index);
        if (cmp != 0)
        {
            return cmp < 0;
        }
        return std::strcmp(ISBN, other.ISBN) < 0;
    }
};

struct keyword_isbn_
{
    char index[61];
    char ISBN[21];

    keyword_isbn_()
    {
        index[0] = '\0';
        ISBN[0] = '\0';
    }

    keyword_isbn_(const char *new_index, const char *new_ISBN)
    {
        std::strncpy(index, new_index, 60);
        index[60] = '\0';
        std::strncpy(ISBN, new_ISBN, 20);
        ISBN[20] = '\0';
    }

    keyword_isbn_ &operator=(const keyword_isbn_ &other)
    {
        if (this != &other)
        {
            std::strncpy(index, other.index, 60);
            index[60] = '\0';
            std::strncpy(ISBN, other.ISBN, 20);
            ISBN[20] = '\0';
        }
        return *this;
    }

    bool operator==(const keyword_isbn_ &other) const
    {
        return std::strcmp(index, other.index) == 0 &&
               std::strcmp(ISBN, other.ISBN) == 0;
    }

    bool operator<(const keyword_isbn_ &other) const
    {
        int cmp = std::strcmp(index, other.index);
        if (cmp != 0)
        {
            return cmp < 0;
        }
        return std::strcmp(ISBN, other.ISBN) < 0;
    }
};

class BookManager
{
    BlockStorageSystem<Book> book_storage;
    BlockStorageSystem<name_author_isbn> name_isbn;
    BlockStorageSystem<name_author_isbn> author_isbn;
    BlockStorageSystem<keyword_isbn_> keyword_isbn;

public:
    BookManager();
    vector<name_author_isbn> f_by_name(const char *name);
    vector<name_author_isbn> f_by_author(const char *author);
    vector<keyword_isbn_> f_by_keyword(const char *keywords);
    vector<Book> f_by_isbn(const char *isbn);
    void buy(const char *isbn, const string &num);
    void select(const char *isbn);
    void insert(const Book &new_book);
    void dele(const char *isbn);
    void impt(const string &num, const Users &cur_user);
    vector<std::array<char, 61>> parse_keywords(const string &keyword_str);
    bool check_isbn(const char *isbn);
    bool check_name(const char *name);
    // bool check_keywords(const vector<string> &kwd);
    bool check_quantity(const std::string &quantity);
    bool check_price(const string &pric);
    void show(const string &str);
    void modify(const string &str, string &selected_isbn);
};

#endif // BOOK_H