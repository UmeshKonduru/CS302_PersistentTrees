#include <memory>
#include <map>
#include <vector>
#include <numeric>
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>

using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

enum Mod {
    LEFT, RIGHT, EMPTY
};

struct Modification;
struct Node;
struct Tree;

struct Modification {

    int version;
    Mod type;
    shared_ptr<Node> node;

    Modification() : version(0), type(EMPTY), node(nullptr) {} 
};

struct Node {

    int key;
    shared_ptr<Node> left, right;
    shared_ptr<Modification> mod;

    Node(int key) : key(key), left(nullptr), right(nullptr), mod(make_shared<Modification>()) {}
};

struct Tree {
    
    int currentVersion;
    map<int, shared_ptr<Node>> root;

    Tree() : currentVersion(0) { root[0] = nullptr; }

    shared_ptr<Node> clone(const shared_ptr<Node>& node) {
        auto newNode = make_shared<Node>(node->key);
        newNode->left = node->mod->type == LEFT ? node->mod->node : node->left;
        newNode->right = node->mod->type == RIGHT ? node->mod->node : node->right;
        return newNode;
    }

    shared_ptr<Node> getRoot() {
        auto it = root.rbegin();
        return it->second;
    }

    shared_ptr<Node> getLeft(const shared_ptr<Node>& node, int version) {
        if(node->mod->type == LEFT && node->mod->version <= version) return node->mod->node;
        return node->left;
    }

    shared_ptr<Node> getRight(const shared_ptr<Node>& node, int version) {
        if(node->mod->type == RIGHT && node->mod->version <= version) return node->mod->node;
        return node->right;
    }

    shared_ptr<Node> getLeft(const shared_ptr<Node>& node) {
        if(node->mod->type == LEFT) return node->mod->node;
        return node->left;
    }

    shared_ptr<Node> getRight(const shared_ptr<Node>& node) {
        if(node->mod->type == RIGHT) return node->mod->node;
        return node->right;
    }

    shared_ptr<Node> setLeft(const shared_ptr<Node>& node, const shared_ptr<Node>& left) {
        
        if(getLeft(node) == left) return node;

        if(node->mod->type == EMPTY) {
            node->mod->type = LEFT;
            node->mod->node = left;
            node->mod->version = currentVersion;
            return node;
        }

        auto newNode = clone(node);
        newNode->left = left;
        return newNode;
    }

    shared_ptr<Node> setRight(const shared_ptr<Node>& node, const shared_ptr<Node>& right) {
        
        if(getRight(node) == right) return node;

        if(node->mod->type == EMPTY) {
            node->mod->type = RIGHT;
            node->mod->node = right;
            node->mod->version = currentVersion;
            return node;
        }

        auto newNode = clone(node);
        newNode->right = right;
        return newNode;
    }

    shared_ptr<Node> insertKey(const shared_ptr<Node>& node, int key) {

        if(!node) return make_shared<Node>(key);

        if(key < node->key) {
            auto left = insertKey(getLeft(node), key);
            return setLeft(node, left);
        }

        if(key > node->key) {
            auto right = insertKey(getRight(node), key);
            return setRight(node, right);
        }

        return node;
    }

    shared_ptr<Node> deleteKey(const shared_ptr<Node>& node, int key) {

        if(!node) return nullptr;

        if(key < node->key) {
            auto left = deleteKey(getLeft(node), key);
            return setLeft(node, left);
        }

        if(key > node->key) {
            auto right = deleteKey(getRight(node), key);
            return setRight(node, right);
        }

        if(!getLeft(node)) return getRight(node);
        if(!getRight(node)) return getLeft(node);

        auto succ = getRight(node);
        while(getLeft(succ)) succ = getLeft(succ);

        auto newNode = make_shared<Node>(succ->key);
        newNode->left = getLeft(node);

        auto right = deleteKey(getRight(node), succ->key);
        newNode->right = right;

        return newNode;
    }

    bool find(int key, int version) {
        auto node = root[version];
        while(node) {
            if(node->key == key) return true;
            if(key < node->key) node = getLeft(node, version);
            else node = getRight(node, version);
        }
        return false;
    }

    bool find(int key) {
        auto node = getRoot();
        while(node) {
            if(node->key == key) return true;
            if(key < node->key) node = getLeft(node);
            else node = getRight(node);
        }
        return false;
    }

    void insert(int key) {
        currentVersion++;
        root[currentVersion] = insertKey(getRoot(), key);
    }

    void erase(int key) {
        currentVersion++;
        root[currentVersion] = deleteKey(getRoot(), key);
    }

    void inorder(const shared_ptr<Node>& node, int version) {
        if(!node) return;
        inorder(getLeft(node, version), version);
        cout << node->key << " ";
        inorder(getRight(node, version), version);
    }

    void inorder(int version) {
        inorder(root[version], version);
        cout << endl;
    }
};

void test() {

    Tree tree;

    vector<int> keys(20);
    iota(keys.begin(), keys.begin() + 10, 1);
    iota(keys.begin() + 10, keys.end(), 1);
    shuffle(keys.begin(), keys.end(), rng);

    for(int key : keys) {
        if(tree.find(key)) {
            cout << "Erasing key " << key << endl;
            tree.erase(key);
        } else {
            cout << "Inserting key " << key << endl;
            tree.insert(key);
        }
    }

    for(int i = 0; i <= 20; i++) {
        cout << "Version " << i << endl;
        tree.inorder(i);
    }
}

int main() {

    test();
}