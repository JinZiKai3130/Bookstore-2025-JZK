#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <vector>
const int max_count = 400;
template <typename T>
struct StorageBlock
{
    int count;
    int64_t nexptr;
    int64_t preptr;

    char key[max_count][64];
    T value[max_count];

    StorageBlock()
    {
        count = 0;
        nexptr = -1;
        preptr = -1;
    }

    int find_pos(const std::string key)
    {
        for (int i = 0; i < count; i++)
        {
            int cmp = strcmp(this->key[i], key.c_str());
            if (cmp == 0 || cmp > 0)
            {
                return i;
            }
        }
        return count;
    }

    int64_t insert(const std::string key, const T &value)
    {
        if (count >= max_count)
        {
            return -1;
        }

        int pos = find_pos(key);
        for (int i = count; i >= pos; i--)
        {
            strncpy(this->key[i], this->key[i - 1], 64);
            this->value[i] = this->value[i - 1];
        }
        strncpy(this->key[pos], key.c_str(), 64);
        this->value[pos] = value;
        count++;
        return pos;
    }

    int64_t remove(const std::string key)
    {
        int pos = find_pos(key);
        if (pos == count || strcmp(this->key[pos], key.c_str()) != 0)
        {
            return -1;
        }
        for (int i = pos; i < count - 1; i++)
        {
            strncpy(this->key[i], this->key[i + 1], 64);
            this->value[i] = this->value[i + 1];
        }
        count--;
        return pos;
    }

    bool find(const std::string key, T &value_out)
    {
        int pos = find_pos(key);
        if (pos == count || strcmp(this->key[pos], key.c_str()) != 0)
        {
            return false; // Key not found
        }
        value_out = this->value[pos];
        return true;
    }
};

template <typename T>
class BlockStorageSystem
{
private:
    std::fstream data_file;
    std::string file_name;
    int64_t first_ptr;
    int64_t last_ptr;

    void read_storage_block(int64_t position, StorageBlock<T> &block)
    {
        data_file.seekg(position);
        data_file.read(reinterpret_cast<char *>(&block), sizeof(block));
    }

    void write_storage_block(int64_t position, StorageBlock<T> &block)
    {
        data_file.seekp(position);
        data_file.write(reinterpret_cast<char *>(&block), sizeof(block));
    }

    int64_t allocate_new_block()
    {
        data_file.seekp(0, std::ios::end);
        int64_t pos = data_file.tellp();
        StorageBlock<T> new_block;
        write_storage_block(pos, new_block);
        return data_file.tellp();
    }

    void split_storage_block(int64_t position, StorageBlock<T> &block)
    {
        StorageBlock<T> new_block;
        int mid = block.count / 2;

        for (int i = mid; i < block.count; i++)
        {
            new_block.insert(block.key[i], block.value[i]);
        }
        block.count = mid;

        new_block.preptr = position;
        new_block.nexptr = block.nexptr;
        block.nexptr = allocate_new_block();
        if (new_block.nexptr != -1)
        {
            StorageBlock<T> next_block;
            read_storage_block(new_block.nexptr, next_block);
            next_block.preptr = block.nexptr;
            write_storage_block(new_block.nexptr, next_block);
        }
        write_storage_block(position, block);
        write_storage_block(block.nexptr, new_block);
    }

    int64_t find_appropriate_block(const std::string &key)
    {
        if (first_ptr == -1)
            return -1;
        int64_t current = first_ptr;
        int64_t last_valid = -1;
        while (current != -1)
        {
            StorageBlock<T> curr_block;
            read_storage_block(current, curr_block);
            if (curr_block.count > 0 && strcmp(curr_block.key[0], key.c_str()) > 0)
            {
                break;
            }
            last_valid = current;
            current = curr_block.nexptr;
        }
        return last_valid;
    }

public:
    BlockStorageSystem(const std::string &filename) : file_name(filename), first_ptr(-1), last_ptr(-1)
    {
        data_file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!data_file)
        {
            data_file.open(file_name, std::ios::out | std::ios::binary);
            data_file.close();
            data_file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        }
        data_file.seekg(0);
        data_file.read(reinterpret_cast<char *>(&first_ptr), sizeof(int64_t));
        data_file.read(reinterpret_cast<char *>(&last_ptr), sizeof(int64_t));
    }

    ~BlockStorageSystem()
    {
        if (data_file.is_open())
        {
            data_file.seekp(0);
            data_file.write(reinterpret_cast<char *>(&first_ptr), sizeof(int64_t));
            data_file.write(reinterpret_cast<char *>(&last_ptr), sizeof(int64_t));
            data_file.close();
        }
    }

    void add_data(const std::string &key, const T &value)
    {
        // Implementation of adding data to the block storage system
        if (first_ptr == -1)
        {
            StorageBlock<T> new_block;
            first_ptr = allocate_new_block();
            last_ptr = first_ptr;
            new_block.insert(key, value);
            write_storage_block(first_ptr, new_block);
            return;
        }
        int64_t pos = find_appropriate_block(key);
        if (pos == -1)
            pos = last_ptr;
        StorageBlock<T> target_block;
        read_storage_block(pos, target_block);
        // Check for existing key-value pair to avoid duplicates
        for (int i = 0; i < target_block.count; ++i)
        {
            if (strcmp(target_block.key[i], key.c_str()) == 0 && target_block.value[i] == value)
            {
                return;
            }
        }
        target_block.insert(key, value);
        if (target_block.count >= max_count)
        {
            split_storage_block(pos, target_block);
        }
        else
            write_storage_block(pos, target_block);
    }

    void remove_data(const std::string &key, const T &value)
    {
        if (first_ptr == -1)
            return;
        int64_t cur = first_ptr;
        bool found = false;
        while (cur != -1 && !found)
        {
            StorageBlock<T> curr_block;
            read_storage_block(cur, curr_block);
            for (int i = 0; i < curr_block.count; i++)
            {
                if (strcmp(curr_block.key[i], key.c_str()) == 0 && curr_block.value[i] == value)
                {
                    curr_block.remove(key);
                    write_storage_block(cur, curr_block);
                    found = true;
                    break;
                }
            }
            cur = curr_block.nexptr;
        }
    }

    std::vector<T> find_data(const std::string &key)
    {
        std::vector<T> results;
        if (first_ptr == -1)
        {
            return results;
        }
        int64_t cur = first_ptr;
        while (cur != -1)
        {
            StorageBlock<T> curr_block;
            read_storage_block(cur, curr_block);
            for (int i = 0; i < curr_block.count; i++)
            {
                if (strcmp(curr_block.key[i], key.c_str()) == 0)
                {
                    results.push_back(curr_block.value[i]);
                }
            }
            cur = curr_block.nexptr;
        }
        std::sort(results.begin(), results.end());
        return results;
    }
};

int main()
{
    BlockStorageSystem<int> storage_manager("testing.dat");
    int command_count;
    std::cin >> command_count;

    for (int i = 0; i < command_count; ++i)
    {
        std::string operation;
        std::cin >> operation;

        if (operation == "insert")
        {
            std::string key;
            int value;
            std::cin >> key >> value;
            storage_manager.add_data(key, value);
        }
        else if (operation == "delete")
        {
            std::string key;
            int value;
            std::cin >> key >> value;
            storage_manager.remove_data(key, value);
        }
        else if (operation == "find")
        {
            std::string key;
            std::cin >> key;
            std::vector<int> results = storage_manager.find_data(key);

            if (results.empty())
            {
                std::cout << "null";
            }
            else
            {
                for (size_t j = 0; j < results.size(); ++j)
                {
                    if (j != results.size() - 1)
                    {
                        std::cout << results[j] << " ";
                    }
                    else
                    {
                        std::cout << results[j];
                    }
                }
            }
            std::cout << std::endl;
        }
    }
    return 0;
}