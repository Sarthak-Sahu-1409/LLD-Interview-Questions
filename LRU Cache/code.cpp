#include <iostream>
#include <unordered_map>
#include <mutex>
using namespace std;

/*
1. Node
   - Represents one cache entry
   - Stores key, value
   - Maintains prev and next pointers for DLL

2. LRUCache
   - Core cache implementation
   - Data structures used:
     a) unordered_map<int, Node*> for O(1) access
     b) Doubly Linked List to maintain usage order
   - Thread-safe using mutex

FLOW 
GET(key):
1. Check key in map
2. If not present -> return -1
3. If present:
   - Move node to head (MRU)
   - Return value

PUT(key, value):
1. If key already exists:
   - Update value
   - Move node to head
2. If key does not exist:
   - If cache is full:
       a) Remove tail node 
       b) Delete from map
   - Insert new node at head
*/


/*
 * Node of Doubly Linked List
 */
class Node {
public:
    int key;
    int value;
    Node* prev;
    Node* next;

    Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
};


class LRUCache {
private:
    int capacity;
    unordered_map<int, Node*> cache;

    Node* head;   // Dummy head (MRU side)
    Node* tail;   // Dummy tail (LRU side)

    mutex mtx;

    void addToHead(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void moveToHead(Node* node) {
        removeNode(node);
        addToHead(node);
    }

    Node* removeTail() {
        Node* lru = tail->prev;
        removeNode(lru);
        return lru;
    }

public:
    LRUCache(int cap) : capacity(cap) {
        head = new Node(-1, -1);
        tail = new Node(-1, -1);
        head->next = tail;
        tail->prev = head;
    }

    int get(int key) {
        lock_guard<mutex> lock(mtx);

        if (cache.find(key) == cache.end())
            return -1;

        Node* node = cache[key];
        moveToHead(node);
        return node->value;
    }

    void put(int key, int value) {
        lock_guard<mutex> lock(mtx);

        if (cache.find(key) != cache.end()) {
            Node* node = cache[key];
            node->value = value;
            moveToHead(node);
        } else {
            if (cache.size() == capacity) {
                Node* lru = removeTail();
                cache.erase(lru->key);
                delete lru;
            }

            Node* newNode = new Node(key, value);
            cache[key] = newNode;
            addToHead(newNode);
        }
    }
};


class LRUCacheDemo {
public:
    static void run() {
        LRUCache cache(3);

        cache.put(1, 10);
        cache.put(2, 20);
        cache.put(3, 30);

        cout << cache.get(1) << endl; // 10

        cache.put(4, 40);             // Evicts key 2

        cout << cache.get(2) << endl; // -1
        cout << cache.get(3) << endl; // 30
        cout << cache.get(4) << endl; // 40
    }
};


int main() {
    LRUCacheDemo::run();
    return 0;
}
