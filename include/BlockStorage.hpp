#ifndef BLOCK_STORAGE_H
#define BLOCK_STORAGE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <string>

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
const int MAX_KEY_SIZE = 64;
const int MAX_BLOCK_CAPACITY = 400;

template <typename T>
struct StorageBlock
{
    int element_count;
    long next_block_ptr;
    long prev_block_ptr;

    char key_storage[MAX_BLOCK_CAPACITY][MAX_KEY_SIZE + 1]{};
    T value_storage[MAX_BLOCK_CAPACITY]{};

    StorageBlock() : element_count(0), next_block_ptr(-1), prev_block_ptr(-1)
    {
        for (int i = 0; i < MAX_BLOCK_CAPACITY; ++i)
        {
            key_storage[i][0] = '\0';
        }
    }

    bool is_full() const { return element_count >= MAX_BLOCK_CAPACITY; }
    bool is_sparse() const { return element_count < MAX_BLOCK_CAPACITY / 4 && element_count > 0; }

    int locate_key_position(const std::string &target_key)
    {
        int left = 0, right = element_count - 1;
        while (left <= right)
        {
            int middle = (left + right) / 2;
            int cmp = strcmp(key_storage[middle], target_key.c_str());
            if (cmp == 0)
                return middle;
            else if (cmp < 0)
                left = middle + 1;
            else
                right = middle - 1;
        }
        return left;
    }

    void add_element(int position, const std::string &new_key, const T &new_value)
    {
        for (int i = element_count - 1; i >= position; --i)
        {
            strcpy(key_storage[i + 1], key_storage[i]);
            value_storage[i + 1] = value_storage[i];
        }
        strncpy(key_storage[position], new_key.c_str(), MAX_KEY_SIZE);
        key_storage[position][MAX_KEY_SIZE] = '\0';
        value_storage[position] = new_value;
        element_count++;
    }

    void remove_element(int position)
    {
        for (int i = position; i < element_count - 1; ++i)
        {
            strcpy(key_storage[i], key_storage[i + 1]);
            value_storage[i] = value_storage[i + 1];
        }
        element_count--;
        if (element_count >= 0)
        {
            key_storage[element_count][0] = '\0';
        }
    }
};

template <typename T>
class BlockStorageSystem
{
private:
    std::fstream data_file;
    std::string file_name;
    long first_block_ptr;
    long last_block_ptr;
    long free_blocks_head;

    void read_storage_block(long position, StorageBlock<T> &block)
    {
        data_file.seekg(position);
        data_file.read(reinterpret_cast<char *>(&block), sizeof(block));
    }

    void write_storage_block(long position, StorageBlock<T> &block)
    {
        data_file.seekp(position);
        data_file.write(reinterpret_cast<char *>(&block), sizeof(block));
    }

    long allocate_new_block()
    {
        if (free_blocks_head != -1)
        {
            long pos = free_blocks_head;
            StorageBlock<T> buf;
            read_storage_block(pos, buf);
            free_blocks_head = buf.next_block_ptr;
            return pos;
        }
        data_file.seekp(0, std::ios::end);
        long pos = data_file.tellp();
        StorageBlock<T> new_block;
        write_storage_block(pos, new_block);
        return pos;
    }

    void deallocate_block(long position)
    {
        StorageBlock<T> buf;
        read_storage_block(position, buf);
        buf.next_block_ptr = free_blocks_head;
        free_blocks_head = position;
        write_storage_block(position, buf);
    }

    void split_storage_block(long position, StorageBlock<T> &block)
    {
        long new_pos = allocate_new_block();
        StorageBlock<T> new_block;
        int split_idx = block.element_count / 2;
        for (int i = split_idx; i < block.element_count; ++i)
        {
            strncpy(new_block.key_storage[new_block.element_count], block.key_storage[i], MAX_KEY_SIZE);
            new_block.key_storage[new_block.element_count][MAX_KEY_SIZE] = '\0';
            new_block.value_storage[new_block.element_count] = block.value_storage[i];
            new_block.element_count++;
        }
        block.element_count = split_idx;
        new_block.next_block_ptr = block.next_block_ptr;
        new_block.prev_block_ptr = position;
        if (block.next_block_ptr != -1)
        {
            StorageBlock<T> next_block;
            read_storage_block(block.next_block_ptr, next_block);
            next_block.prev_block_ptr = new_pos;
            write_storage_block(block.next_block_ptr, next_block);
        }
        block.next_block_ptr = new_pos;
        if (last_block_ptr == position)
            last_block_ptr = new_pos;
        write_storage_block(position, block);
        write_storage_block(new_pos, new_block);
    }

    void attempt_block_merge(long left_pos, long right_pos)
    {
        if (left_pos == -1 || right_pos == -1)
            return;
        StorageBlock<T> left_block, right_block;
        read_storage_block(left_pos, left_block);
        read_storage_block(right_pos, right_block);
        if (left_block.element_count + right_block.element_count <= MAX_BLOCK_CAPACITY)
        {
            for (int i = 0; i < right_block.element_count; ++i)
            {
                strncpy(left_block.key_storage[left_block.element_count], right_block.key_storage[i], MAX_KEY_SIZE);
                left_block.key_storage[left_block.element_count][MAX_KEY_SIZE] = '\0';
                left_block.value_storage[left_block.element_count] = right_block.value_storage[i];
                left_block.element_count++;
            }
            left_block.next_block_ptr = right_block.next_block_ptr;
            if (right_block.next_block_ptr != -1)
            {
                StorageBlock<T> next_block;
                read_storage_block(right_block.next_block_ptr, next_block);
                next_block.prev_block_ptr = left_pos;
                write_storage_block(right_block.next_block_ptr, next_block);
            }
            if (last_block_ptr == right_pos)
                last_block_ptr = left_pos;
            if (first_block_ptr == right_pos)
                first_block_ptr = left_pos;
            write_storage_block(left_pos, left_block);
            deallocate_block(right_pos);
        }
    }

    long find_appropriate_block(const std::string &target_key)
    {
        if (first_block_ptr == -1)
            return -1;
        long current = first_block_ptr;
        while (current != -1)
        {
            StorageBlock<T> curr_block;
            read_storage_block(current, curr_block);
            if (curr_block.element_count > 0 && strcmp(target_key.c_str(), curr_block.key_storage[curr_block.element_count - 1]) <= 0)
            {
                return current;
            }
            if (curr_block.next_block_ptr == -1)
                return current;
            current = curr_block.next_block_ptr;
        }
        return last_block_ptr;
    }

public:
    BlockStorageSystem(const std::string &filename)
        : first_block_ptr(-1), last_block_ptr(-1), free_blocks_head(-1), file_name(filename) {}

    ~BlockStorageSystem()
    {
        if (data_file.is_open())
        {
            data_file.seekp(0);
            data_file.write(reinterpret_cast<char *>(&first_block_ptr), sizeof(long));
            data_file.write(reinterpret_cast<char *>(&last_block_ptr), sizeof(long));
            data_file.write(reinterpret_cast<char *>(&free_blocks_head), sizeof(long));
            data_file.close();
        }
    }

    void initialize_system()
    {
        data_file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);
        if (!data_file.is_open())
        {
            data_file.open(file_name, std::ios::binary | std::ios::out | std::ios::trunc);
            data_file.close();
            data_file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);

            data_file.seekp(0);
            data_file.write(reinterpret_cast<char *>(&first_block_ptr), sizeof(long));
            data_file.write(reinterpret_cast<char *>(&last_block_ptr), sizeof(long));
            data_file.write(reinterpret_cast<char *>(&free_blocks_head), sizeof(long));
        }
        else
        {
            data_file.seekg(0);
            data_file.read(reinterpret_cast<char *>(&first_block_ptr), sizeof(long));
            data_file.read(reinterpret_cast<char *>(&last_block_ptr), sizeof(long));
            data_file.read(reinterpret_cast<char *>(&free_blocks_head), sizeof(long));
        }
    }

    void add_data(const std::string &key, const T &value)
    {
        if (first_block_ptr == -1)
        {
            first_block_ptr = allocate_new_block();
            last_block_ptr = first_block_ptr;
            StorageBlock<T> new_block;
            strncpy(new_block.key_storage[0], key.c_str(), MAX_KEY_SIZE);
            new_block.key_storage[0][MAX_KEY_SIZE] = '\0';
            new_block.value_storage[0] = value;
            new_block.element_count = 1;
            write_storage_block(first_block_ptr, new_block);
            return;
        }
        long pos = find_appropriate_block(key);
        if (pos == -1)
            pos = last_block_ptr;
        StorageBlock<T> target_block;
        read_storage_block(pos, target_block);

        for (int i = 0; i < target_block.element_count; ++i)
        {
            if (strcmp(target_block.key_storage[i], key.c_str()) == 0 && target_block.value_storage[i] == value)
            {
                return;
            }
        }

        int insert_pos = target_block.locate_key_position(key);
        target_block.add_element(insert_pos, key, value);
        if (target_block.is_full())
        {
            write_storage_block(pos, target_block);
            split_storage_block(pos, target_block);
        }
        else
        {
            write_storage_block(pos, target_block);
            if (target_block.is_sparse() && target_block.prev_block_ptr != -1)
            {
                attempt_block_merge(target_block.prev_block_ptr, pos);
            }
        }
    }

    void remove_data(const std::string &key, const T &value)
    {
        if (first_block_ptr == -1)
            return;
        long current = first_block_ptr;
        bool found = false;
        while (current != -1 && !found)
        {
            StorageBlock<T> curr_block;
            read_storage_block(current, curr_block);
            for (int i = 0; i < curr_block.element_count; ++i)
            {
                if (strcmp(curr_block.key_storage[i], key.c_str()) == 0 && curr_block.value_storage[i] == value)
                {
                    curr_block.remove_element(i);
                    write_storage_block(current, curr_block);
                    found = true;
                    if (curr_block.is_sparse())
                    {
                        if (curr_block.prev_block_ptr != -1)
                        {
                            attempt_block_merge(curr_block.prev_block_ptr, current);
                        }
                        else if (curr_block.next_block_ptr != -1)
                        {
                            StorageBlock<T> next_block;
                            read_storage_block(curr_block.next_block_ptr, next_block);
                            if (next_block.is_sparse())
                            {
                                attempt_block_merge(current, curr_block.next_block_ptr);
                            }
                        }
                    }
                    break;
                }
            }
            current = curr_block.next_block_ptr;
        }
    }

    std::vector<T> search_data(const std::string &key)
    {
        std::vector<T> results;
        if (first_block_ptr == -1)
            return results;
        long current = first_block_ptr;
        while (current != -1)
        {
            StorageBlock<T> curr_block;
            read_storage_block(current, curr_block);
            if (curr_block.element_count > 0 && strcmp(curr_block.key_storage[0], key.c_str()) > 0)
            {
                break;
            }
            for (int i = 0; i < curr_block.element_count; ++i)
            {
                if (strcmp(curr_block.key_storage[i], key.c_str()) == 0)
                {
                    results.push_back(curr_block.value_storage[i]);
                }
                else if (strcmp(curr_block.key_storage[i], key.c_str()) > 0)
                {
                    break;
                }
            }
            current = curr_block.next_block_ptr;
        }
        std::sort(results.begin(), results.end());
        return results;
    }

    void traverse(const string &condition = "")
    {
        if (first_block_ptr == -1)
        {
            std::cout << '\n';
            return;
        }
        long current = first_block_ptr;
        int total_elements = 0;
        int block_count = 0;

        while (current != -1)
        {
            StorageBlock<T> curr_block;
            read_storage_block(current, curr_block);
            for (int i = 0; i < curr_block.element_count; ++i)
            {
                if (condition.length() == 0)
                {
                    // 遍历书本or遍历全部finance_log
                    curr_block.value_storage[i].print();
                }
                else
                {
                    // 遍历number + 1 到最后的所有finance_log
                    int number = std::stoi(condition);
                    int cur_num = std::stoi(curr_block.key_storage[i]);
                    if (number < cur_num)
                    {
                        curr_block.value_storage[i].print();
                    }
                }
                // curr_block.value_storage[i].print();
            }
            total_elements += curr_block.element_count;
            block_count++;
            current = curr_block.next_block_ptr;
        }
    }
};

#endif // BLOCK_STORAGE_H