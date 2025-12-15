#ifndef BOOK_H
#define BOOK_H

#include <cstring>
#include <string>

// ISBN-Book映射
struct Book {
    char ISBN[21];
    char name[61];
    char author[61];
    char keyword[61];
    double price;
    int quantity;

    Book() {
        ISBN[0] = '\0';
        name[0] = '\0';
        author[0] = '\0';
        keyword[0] = '\0';
        price = 0.00;
        quantity = 0;
        
    }

    Book(const char* new_ISBN,
         const char* new_name,
         const char* new_author,
         const char* new_keyword,
         const double& new_price,
         const int& new_quantity) {
        std::strncpy(ISBN, new_ISBN, 20);
        ISBN[20] = '\0';
        std::strncpy(name, new_name, 60);
        name[60] = '\0';
        std::strncpy(author, new_author, 60);
        author[60] = '\0';
        std::strncpy(keyword, new_keyword, 60);
        key_word[60] = '\0';
        price = new_price;
        quantity = new_quantity;
    }

    Book& operator=(const Book& other) {
        std::strncpy(ISBN, other.ISBN, 20); ISBN[20] = '\0';
        std::strncpy(name, other.name, 60); name[60] = '\0';
        std::strncpy(author, other.author, 60); author[60] = '\0';
        std::strncpy(keyword, other.keyword, 60); keyword[60] = '\0';
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
};

#endif // BOOK_H