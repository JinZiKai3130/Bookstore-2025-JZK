#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

const int max_block_size = 1024;

template <typename T> struct Entry {
  char index[65];
  T value;
  Entry() {
    value = 0;
    memset(index, 0, sizeof(index));
  }
  Entry(const char *new_idx, T val) : value(val) {
    memset(index, 0, sizeof(index));
    strncpy(index, new_idx, 64);
  }

  bool operator<(const Entry &other) const {
    if (strcmp(index, other.index) != 0)
      return strcmp(index, other.index) < 0;
    return value < other.value;
  }

  bool operator==(const Entry &other) const {
    return (strcmp(index, other.index) == 0 && value == other.value);
  }
};

template <typename T> struct Blockhead {
  int next_block = -1;
  int count = 0;
  Entry<T> max_entry;
  Blockhead() {}
};

template <typename T> struct Block {
  int next_block = -1;
  int count = 0;
  Entry<T> max_entry;
  Entry<T> entries[max_block_size];
  Block() {}
};

template <typename T> class StorageBlockSystem {
private:
  std::fstream file;
  std::string file_path;
  int first_block;
  int block_sum;

  void writeBlock(int pos, const Block<T> &block) {
    file.seekp(pos * sizeof(Block<T>));
    file.write(reinterpret_cast<const char *>(&block), sizeof(Block<T>));
  }

  void readBlock(int pos, Block<T> &block) {
    file.seekg(pos * sizeof(Block<T>));
    file.read(reinterpret_cast<char *>(&block), sizeof(Block<T>));
  }

  void readBlockhead(int pos, Blockhead<T> &block_head) {
    file.seekg(pos * sizeof(Block<T>));
    file.read(reinterpret_cast<char *>(&block_head), sizeof(Blockhead<T>));
  }

  void splitBlock(int block_pos, Block<T> &block) {
    Block<T> new_block;
    int mid = block.count / 2;

    new_block.count = block.count - mid;
    for (int i = 0; i < new_block.count; i++) {
      new_block.entries[i] = block.entries[mid + i];
    }
    block.count = mid;
    block.max_entry = block.entries[block.count - 1];
    new_block.max_entry = new_block.entries[new_block.count - 1];

    new_block.next_block = block.next_block;
    block.next_block = block_sum;
    writeBlock(block_pos, block);
    writeBlock(block_sum, new_block);
    block_sum++;
  }

public:
  StorageBlockSystem(std::string file_path)
      : first_block(0), block_sum(1), file_path(file_path) {
    std::ifstream test(file_path);
    bool exist = test.good();
    test.close();
    if (exist) {
      file.open(file_path, std::ios::in | std::ios::out | std::ios::binary);
      file.seekg(0);
      Block<T> first;
      readBlock(0, first);
      file.seekg(0, std::ios::end);
      long long file_size = file.tellg();
      block_sum = file_size / sizeof(Block<T>);
    } else {
      file.open(file_path, std::ios::in | std::ios::out | std::ios::binary |
                               std::ios::trunc);
      Block<T> first;
      writeBlock(0, first);
    }
  }

  ~StorageBlockSystem() {
    if (file.is_open()) {
      file.close();
    }
  }

  void add_data(const char *index, T value) {
    Entry<T> new_entry(index, value);

    int cur_pos = 0;
    Blockhead<T> block_head;
    readBlockhead(cur_pos, block_head);

    while (true) {
      bool flag = false;

      if (block_head.count == 0) {
        flag = true;
      } else if (new_entry < block_head.max_entry) {
        flag = true;
      } else if (block_head.next_block == -1) {
        flag = true;
      }

      if (flag) {
        Block<T> cur_block;
        readBlock(cur_pos, cur_block);
        for (int i = 0; i < cur_block.count; i++) {
          if (cur_block.entries[i] == new_entry) {
            return; // 重复的直接return
          }
        }
        int insert_pos = cur_block.count;
        for (int i = 0; i < cur_block.count; i++) {
          if (new_entry < cur_block.entries[i]) {
            insert_pos = i;
            break;
          }
        }
        for (int i = cur_block.count; i > insert_pos; i--) {
          cur_block.entries[i] = cur_block.entries[i - 1]; // move backwards
        }
        cur_block.entries[insert_pos] = new_entry;
        cur_block.count++;
        cur_block.max_entry = cur_block.entries[cur_block.count - 1];

        if (cur_block.count >= max_block_size - 2) { // 不能过大，否则裂
          splitBlock(cur_pos, cur_block);
        } else {
          writeBlock(cur_pos, cur_block);
        }
        return;
      }

      cur_pos = block_head.next_block;
      readBlockhead(cur_pos, block_head);
    }
  }

  void remove_data(const char *index, T value) {
    Entry target(index, value);
    int current_pos = 0;

    while (current_pos != -1) {
      Blockhead<T> block_head;
      readBlockhead(current_pos, block_head);

      if (block_head.count > 0 && block_head.max_entry < target) {
        current_pos = block_head.next_block;
        continue;
      }
      // cout << "xxx\n";
      Block<T> cur_block;
      readBlock(current_pos, cur_block);

      for (int i = 0; i < cur_block.count; i++) {
        if (cur_block.entries[i] == target) {
          for (int j = i; j < cur_block.count - 1; j++) {
            cur_block.entries[j] = cur_block.entries[j + 1];
          }
          cur_block.count--;

          if (cur_block.count > 0) {
            cur_block.max_entry = cur_block.entries[cur_block.count - 1];
          } else {
            cur_block.max_entry = Entry<T>();
          }

          writeBlock(current_pos, cur_block);
          return;
        }
      }
      current_pos = cur_block.next_block;
    }
  }

  std::vector<T> search_data(const char *index) {
    std::vector<T> results;
    int pos = 0;
    bool flag = false;
    while (pos != -1) {
      Blockhead<T> block_head;
      readBlockhead(pos, block_head);
      if (block_head.count > 0 &&
          strcmp(index, block_head.max_entry.index) > 0) {
        pos = block_head.next_block;
        continue;
      }
      Block<T> cur_block;
      readBlock(pos, cur_block);
      for (int i = 0; i < cur_block.count; i++) {
        if (strcmp(cur_block.entries[i].index, index) == 0) {
          results.push_back(cur_block.entries[i].value);
        } else if (strcmp(cur_block.entries[i].index, index) > 0) {
          flag = true;
          break;
        }
      }
      pos = cur_block.next_block;
    }
    std::sort(results.begin(), results.end());
    return results;
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  int n;
  std::cin >> n;
  StorageBlockSystem<int> dat("testdata.dat");
  for (int i = 1; i <= n; i++) {
    std::string op;
    std::cin >> op;
    if (op == "insert") {
      std::string index;
      int value;
      std::cin >> index >> value;
      dat.add_data(index.c_str(), value);
    } else if (op == "delete") {
      std::string index;
      int value;
      std::cin >> index >> value;
      dat.remove_data(index.c_str(), value);
    } else if (op == "find") {
      std::string index;
      std::cin >> index;
      std::vector<int> result = dat.search_data(index.c_str());
      if (result.empty()) {
        std::cout << "null";
      } else {
        for (size_t i = 0; i < result.size(); i++) {
          if (i > 0)
            std::cout << " ";
          std::cout << result[i];
        }
      }
      std::cout << "\n";
    }
  }

  return 0;
}