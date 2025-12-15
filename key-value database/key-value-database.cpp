#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::vector;
const int MAX_KEY_SIZE = 64;
const int MAX_BLOCK_CAPACITY = 400;

struct StorageBlock {
    int element_count;
    
    long next_block_ptr;
    long prev_block_ptr;

    char key_storage[MAX_BLOCK_CAPACITY][MAX_KEY_SIZE + 1]{};
    int value_storage[MAX_BLOCK_CAPACITY]{};

    StorageBlock() : element_count(0), next_block_ptr(-1), prev_block_ptr(-1) {
        for (int i = 0; i < MAX_BLOCK_CAPACITY; ++i) {
            key_storage[i][0] = '\0';
            value_storage[i] = 0;
        }
    }

    bool is_full() const {
        return element_count >= MAX_BLOCK_CAPACITY;
    }

    bool is_sparse() const {
        return element_count < MAX_BLOCK_CAPACITY / 4 && element_count > 0;
    }

    int locate_key_position(const std::string& target_key) {
        int left = 0, right = element_count - 1;
        while (left <= right) {
            int middle = (left + right) / 2;
            int comparison_result = strcmp(key_storage[middle], target_key.c_str());
            if (comparison_result == 0) {
                return middle;
            } else if (comparison_result < 0) {
                left = middle + 1;
            } else {
                right = middle - 1;
            }
        }
        return left;
    }

    void add_element(int position, const std::string& new_key, int new_value) {
        for (int i = element_count - 1; i >= position; --i) {
            strcpy(key_storage[i + 1], key_storage[i]);
            value_storage[i + 1] = value_storage[i];
        }
        strcpy(key_storage[position], new_key.c_str());
        value_storage[position] = new_value;
        element_count++;
    }

    void remove_element(int position) {
        for (int i = position; i < element_count - 1; ++i) {
            strcpy(key_storage[i], key_storage[i + 1]);
            value_storage[i] = value_storage[i + 1];
        }
        element_count--;
        if (element_count >= 0) {
            key_storage[element_count][0] = '\0';
            value_storage[element_count] = 0;
        }
    }
};

class BlockStorageSystem {
private:
    fstream data_file;
    string file_name;
    long first_block_ptr;
    long last_block_ptr;
    long free_blocks_head;

    void read_storage_block(long position, StorageBlock& block) {
        data_file.seekg(position);
        data_file.read(reinterpret_cast<char*> (&block), sizeof(block));
    }

    void write_storage_block(long position, StorageBlock& block) {
        data_file.seekp(position);
        data_file.write(reinterpret_cast<char*> (&block), sizeof(block));
    }

    long allocate_new_block() {
        if (free_blocks_head != -1) {
            long position = free_blocks_head;
            StorageBlock block_buffer;
            read_storage_block(position, block_buffer);
            free_blocks_head = block_buffer.next_block_ptr;
            return position;
        }
        data_file.seekp(0, std::ios::end);
        long position = data_file.tellp();
        StorageBlock new_block;
        write_storage_block(position, new_block);
        return position;
    }

    void deallocate_block(long position) {
        StorageBlock block_buffer;
        read_storage_block(position, block_buffer);
        block_buffer.next_block_ptr = free_blocks_head;
        free_blocks_head = position;
        write_storage_block(position, block_buffer);
    }

    void split_storage_block(long position, StorageBlock& block) {
        long new_position = allocate_new_block();
        StorageBlock new_block;
        int split_index = block.element_count / 2;
        for (int i = split_index; i < block.element_count; ++i) {
            strcpy(new_block.key_storage[new_block.element_count], block.key_storage[i]);
            new_block.value_storage[new_block.element_count] = block.value_storage[i];
            new_block.element_count++;
        }
        block.element_count = split_index;
        new_block.next_block_ptr = block.next_block_ptr;
        new_block.prev_block_ptr = position;
        if (block.next_block_ptr != -1) {
            StorageBlock next_block;
            read_storage_block(block.next_block_ptr, next_block);
            next_block.prev_block_ptr = new_position;
            write_storage_block(block.next_block_ptr, next_block);
        }
        block.next_block_ptr = new_position;
        if (last_block_ptr == position) {
            last_block_ptr = new_position;
        }
        write_storage_block(position, block);
        write_storage_block(new_position, new_block);
    }

    void attempt_block_merge(long left_position, long right_position) {
        if (left_position == -1 || right_position == -1) {
            return;
        }
        StorageBlock left_block, right_block;
        read_storage_block(left_position, left_block);
        read_storage_block(right_position, right_block);
        if (left_block.element_count + right_block.element_count <= MAX_BLOCK_CAPACITY) {
            for (int i = 0; i < right_block.element_count; ++i) {
                strcpy(left_block.key_storage[left_block.element_count], right_block.key_storage[i]);
                left_block.value_storage[left_block.element_count] = right_block.value_storage[i];
                left_block.element_count++;
            }
            left_block.next_block_ptr = right_block.next_block_ptr;
            if (right_block.next_block_ptr != -1) {
                StorageBlock next_block;
                read_storage_block(right_block.next_block_ptr, next_block);
                next_block.prev_block_ptr = left_position;
                write_storage_block(right_block.next_block_ptr, next_block);
            }
            if (last_block_ptr == right_position) {
                last_block_ptr = left_position;
            }
            if (first_block_ptr == right_position) {
                first_block_ptr = left_position;
            }
            write_storage_block(left_position, left_block);
            deallocate_block(right_position);
        }
    }

    long find_appropriate_block(const std::string& target_key) {
        if (first_block_ptr == -1) {
            return -1;
        }
        long current = first_block_ptr;
        while (current != -1) {
            StorageBlock current_block;
            read_storage_block(current, current_block);
            if (current_block.element_count > 0) {
                if (strcmp(target_key.c_str(), current_block.key_storage[current_block.element_count - 1]) <= 0) {
                    return current;
                }
            }
            if (current_block.next_block_ptr == -1) {
                return current;
            }
            current = current_block.next_block_ptr;
        }
        return last_block_ptr;
    }

public:
    BlockStorageSystem() : first_block_ptr(-1), last_block_ptr(-1), free_blocks_head(-1) {
        file_name = "Bookstore";
    }

    ~BlockStorageSystem() {
        if (data_file.is_open()) {
            data_file.seekp(0);
            data_file.write(reinterpret_cast<char*>(&first_block_ptr), sizeof(long));
            data_file.write(reinterpret_cast<char*>(&last_block_ptr), sizeof(long));
            data_file.write(reinterpret_cast<char*>(&free_blocks_head), sizeof(long));
            data_file.close();
        }
    }

    void initialize_system() {
        data_file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);
        if (!data_file.is_open()) {
            data_file.open(file_name, std::ios::binary | std::ios::out | std::ios::trunc);
            data_file.close();
            data_file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);

            data_file.seekp(0);
            data_file.write(reinterpret_cast<char*>(&first_block_ptr), sizeof(long));
            data_file.write(reinterpret_cast<char*>(&last_block_ptr), sizeof(long));
            data_file.write(reinterpret_cast<char*>(&free_blocks_head), sizeof(long));
        } else {
            data_file.seekg(0);
            data_file.read(reinterpret_cast<char*>(&first_block_ptr), sizeof(long));
            data_file.read(reinterpret_cast<char*>(&last_block_ptr), sizeof(long));
            data_file.read(reinterpret_cast<char*>(&free_blocks_head), sizeof(long));
        }
    }

    void add_data(const std::string &key, int value) {
        if (first_block_ptr == -1) {
            first_block_ptr = allocate_new_block();
            last_block_ptr = first_block_ptr;
            StorageBlock new_block;
            strcpy(new_block.key_storage[0], key.c_str());
            new_block.value_storage[0] = value;
            new_block.element_count = 1;
            write_storage_block(first_block_ptr, new_block);
            return;
        }
        long position = find_appropriate_block(key);
        if (position == -1) {
            position = last_block_ptr;
        }
        StorageBlock target_block;
        read_storage_block(position, target_block);
        for (int i = 0; i < target_block.element_count; i++) {
            if (strcmp(target_block.key_storage[i], key.c_str()) == 0 && target_block.value_storage[i] == value) {
                return;
            }
        }
        int insert_position = target_block.locate_key_position(key);
        target_block.add_element(insert_position, key, value);
        if (target_block.is_full()) {
            write_storage_block(position, target_block);
            split_storage_block(position, target_block);
        } else {
            write_storage_block(position, target_block);
            if (target_block.is_sparse() && target_block.prev_block_ptr != -1) {
                attempt_block_merge(target_block.prev_block_ptr, position);
            }
        }
    }

    void remove_data(const std::string &key, int value) {
        if (first_block_ptr == -1) {
            return;
        }
        long current = first_block_ptr;
        bool found = false;
        while (current != -1 && !found) {
            StorageBlock current_block;
            read_storage_block(current, current_block);
            for (int i = 0; i < current_block.element_count; ++i) {
                if (strcmp(current_block.key_storage[i], key.c_str()) == 0 && current_block.value_storage[i] == value) {
                    current_block.remove_element(i);
                    write_storage_block(current, current_block);
                    found = true;
                    if (current_block.is_sparse()) {
                        if (current_block.prev_block_ptr != -1) {
                            attempt_block_merge(current_block.prev_block_ptr, current);
                        } else if (current_block.next_block_ptr != -1) {
                            StorageBlock next_block;
                            read_storage_block(current_block.next_block_ptr, next_block);
                            if (next_block.is_sparse()) {
                                attempt_block_merge(current, current_block.next_block_ptr);
                            }
                        }
                    }
                    break;
                }
            }
            current = current_block.next_block_ptr;
        }
    }

    std::vector<int> search_data(const std::string &key) {
        std::vector<int> results;
        if (first_block_ptr == -1) {
            return results;
        }
        long current = first_block_ptr;
        while (current != -1) {
            StorageBlock current_block;
            read_storage_block(current, current_block);
            if (current_block.element_count > 0 && strcmp(current_block.key_storage[0], key.c_str()) > 0) {
                break;
            }
            for (int i = 0; i < current_block.element_count; ++i) {
                if (strcmp(current_block.key_storage[i], key.c_str()) == 0) {
                    results.push_back(current_block.value_storage[i]);
                } else if (strcmp(current_block.key_storage[i], key.c_str()) > 0) {
                    break;
                }
            }
            current = current_block.next_block_ptr;
        }
        std::sort(results.begin(), results.end());
        return results;
    }
};

int main() {
    BlockStorageSystem storage_manager;
    storage_manager.initialize_system();
    int command_count;
    std::cin >> command_count;
    for (int i = 0; i < command_count; ++i) {
        std::string operation;
        std::cin >> operation;
        if (operation == "insert") {
            std::string key;
            int value;
            std::cin >> key >> value;
            storage_manager.add_data(key, value);
        } else if (operation == "delete") {
            std::string key;
            int value;
            std::cin >> key >> value;
            storage_manager.remove_data(key, value);
        } else if (operation == "find") {
            std::string key;
            std::cin >> key;
            std::vector<int> results = storage_manager.search_data(key);
            if (results.empty()) {
                std::cout << "null";
            } else {
                for (int j = 0; j < results.size(); ++j) {
                    if (j != results.size() - 1) {
                        std::cout << results[j] << " ";
                    } else {
                        std::cout << results[j];
                    }
                }
            }
            std::cout << std::endl;
        }
    }
    return 0;
}