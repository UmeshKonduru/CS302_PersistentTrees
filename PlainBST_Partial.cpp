#include <memory>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>

using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

enum Mod {
    EMPTY, LEFT, RIGHT
};

struct Modification;
struct Node;
struct Tree;

struct Modification {

    int version;
    Mod type;
    shared_ptr<Node> node;

    Modification() : version(-1), type(EMPTY), node(nullptr) {}
};

struct Node {

    int key;
    shared_ptr<Node> left, right;
    shared_ptr<Node> parent;
    shared_ptr<Modification> mod;

    Node(int key) : key(key), left(nullptr), right(nullptr), parent(nullptr), mod(make_shared<Modification>()) {}
};

struct Tree {

    int latestVersion;
    map<int, shared_ptr<Node>> root;

    Tree() : latestVersion(0) {
        root[0] = nullptr;
    }

    shared_ptr<Node> clone(const shared_ptr<Node>& node) {
        auto newNode = make_shared<Node>(node->key);
        newNode->left = node->mod->type == LEFT ? node->mod->node : node->left;
        newNode->right = node->mod->type == RIGHT ? node->mod->node : node->right;
        newNode->parent = node->parent;
        if(newNode->left != nullptr) newNode->left->parent = newNode;
        if(newNode->right != nullptr) newNode->right->parent = newNode;
        return newNode;
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
        return getLeft(node, latestVersion);
    }

    shared_ptr<Node> getRight(const shared_ptr<Node>& node) {
        return getRight(node, latestVersion);
    }

    shared_ptr<Node> getRoot(int version) {
        auto it = --(root.upper_bound(version));
        return it->second;
    }

    shared_ptr<Node> getRoot() {
        auto it = root.rbegin();
        return it->second;
    }

    shared_ptr<Node> setLeft(const shared_ptr<Node>&, const shared_ptr<Node>&);
    shared_ptr<Node> setRight(const shared_ptr<Node>&, const shared_ptr<Node>&);

    void insert(int key) {

        ++latestVersion;

        auto node = getRoot();
        shared_ptr<Node> par = nullptr;

        while(node != nullptr) {
            par = node;
            if(key < node->key) node = getLeft(node);
            else node = getRight(node);
        }

        auto newNode = make_shared<Node>(key);

        if(par == nullptr) {
            root[latestVersion] = newNode;
            return;
        }

        if(key < par->key) par = setLeft(par, newNode);
        else par = setRight(par, newNode);

        newNode->parent = par;
    }

    bool count(int key, int version) {

        auto node = getRoot(version);

        while(node != nullptr) {
            if(node->key == key) return true;
            if(key < node->key) node = getLeft(node, version);
            else node = getRight(node, version);
        }

        return false;
    }
};

shared_ptr<Node> Tree::setLeft(const shared_ptr<Node>& node, const shared_ptr<Node>& left) {
    
    if(node->mod->type == EMPTY) {
        node->mod->version = latestVersion;
        node->mod->type = LEFT;
        node->mod->node = left;
        if(left != nullptr) left->parent = node;
        return node;
    }

    auto newNode = clone(node);
    newNode->left = left;
    if(left != nullptr) left->parent = newNode;
    
    auto parent = node->parent;

    if(parent == nullptr) {
        root[latestVersion] = newNode;
        return newNode;
    }

    if(getLeft(parent) == node) parent = setLeft(parent, newNode);
    else parent = setRight(parent, newNode);
    newNode->parent = parent;

    return newNode;
}

shared_ptr<Node> Tree::setRight(const shared_ptr<Node>& node, const shared_ptr<Node>& right) {
    
    if(node->mod->type == EMPTY) {
        node->mod->version = latestVersion;
        node->mod->type = RIGHT;
        node->mod->node = right;
        if(right != nullptr) right->parent = node;
        return node;
    }

    auto newNode = clone(node);
    newNode->right = right;
    if(right != nullptr) right->parent = newNode;
    
    auto parent = node->parent;

    if(parent == nullptr) {
        root[latestVersion] = newNode;
        return newNode;
    }

    if(getLeft(parent) == node) parent = setLeft(parent, newNode);
    else parent = setRight(parent, newNode);
    newNode->parent = parent;

    return newNode;
}

void test() {

    Tree tree;

    vector<int> keys(10);
    iota(keys.begin(), keys.end(), 1);
    shuffle(keys.begin(), keys.end(), rng);

    for(auto key : keys) {
        cout << "Inserting " << key << endl;
        tree.insert(key);
    }

    for(int i = 0; i <= 10; i++) {
        cout << "Version " << i << endl;
        for(int j = 1; j <= 10; j++) {
            cout << tree.count(j, i) << " ";
        }
        cout << endl;
    }
}

int main() {

    test();
}