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

vector<char*> BookManager::f_by_name(const char* name) {
    return name_isbn.search_data(name);
}

vector<char*> BookManager::f_by_author(const char* author) {
    return author_isbn.search_data(author);
}

vector<char*> BookManager::f_by_keyword(const char* keyword) {
    return keyword_isbn.search_data(keyword);
}

vector<Book> BookManager::f_by_isbn(const char* isbn) {
    return book_storage.search_data(isbn);
}

void BookManager::buy(const char* isbn, const int& num) {
    
}

void BookManager::select(const char* isbn) {

}

void BookManager::impt(const int& num, const double& tot_cost) {

}