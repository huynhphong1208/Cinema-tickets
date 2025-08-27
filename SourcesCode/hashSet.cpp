#include <iostream>
#include <vector>
using namespace std;

class MyHashSet {
private:
    const int SIZE = 1000;
    vector<vector<int>> myBuckets;  // mảng 1000 vector<int>

    // Hàm băm
    int hashFunction(int key) {
        return key % SIZE;
    }

public:
    // Constructor  
    MyHashSet() {
        myBuckets.resize(SIZE); // khởi tạo 1000 bucket
    }

    // Thêm phần tử
    void add(int key) {
        int hashValue = hashFunction(key);
        // tránh thêm trùng
        for (int x : myBuckets[hashValue]) {
            if (x == key) return;
        }
        myBuckets[hashValue].push_back(key);
    }

    // Kiểm tra phần tử có tồn tại không
    bool contains(int key) {
        int hashValue = hashFunction(key);
        for (int x : myBuckets[hashValue]) {
            if (x == key) return true;
        }
        return false;
    }

    // Xóa phần tử
    void remove(int key) {
        int hashValue = hashFunction(key);
        auto& bucket = myBuckets[hashValue];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (*it == key) {
                bucket.erase(it);
                return;
            }
        }
    }


};

int main() {
    MyHashSet set;
    set.add(10);
    set.add(20);

    cout << set.contains(10) << endl; // 1 (true)
    cout << set.contains(30) << endl; // 0 (false)

    set.remove(10);
    cout << set.contains(10) << endl; // 0 (false)

    return 0;
}
