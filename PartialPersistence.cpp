// Partially Persistent Red-Black Tree

#include <memory>
#include <map>
#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

enum Color {
    RED, BLACK
};

enum ModType {
    EMPTY, LEFT, RIGHT
};

struct Modification;
struct Node;
struct RedBlackTree;

struct Modification {

    ModType type;
    int version;
    shared_ptr<Node> node;

    Modification() : 
        type(EMPTY),
        version(-1),
        node(nullptr) {}
};

struct Node {

    int key;
    Color color;
    shared_ptr<Node> left, right;
    shared_ptr<Node> parent;
    shared_ptr<Modification> mod;

    Node(int key, shared_ptr<Node> parent = nullptr) :
        key(key), 
        color(RED),
        left(nullptr),
        right(nullptr),
        parent(parent),
        mod(make_shared<Modification>()) {}

    shared_ptr<Node> copy() {

        auto node = make_shared<Node>(key, parent);
        node->color = color;
        node->left = (mod->type == LEFT) ? mod->node : left;
        node->right = (mod->type == RIGHT) ? mod->node : right;

        return node;
    }

    shared_ptr<Node> getLeft(int version) {
        if(mod->type == LEFT && mod->version <= version) return mod->node;
        return left;
    }

    shared_ptr<Node> getRight(int version) {
        if(mod->type == RIGHT && mod->version <= version) return mod->node;
        return right;
    }
};

struct RedBlackTree {
    
    map<int, shared_ptr<Node>> root;
    int latestVersion;

    RedBlackTree() : latestVersion(0) {
        root[0] = nullptr;
    }

    shared_ptr<Node> getRoot(int version) {
        auto it = --(root.upper_bound(version));
        return it->second;
    }

    shared_ptr<Node> getRoot() {
        auto it = root.rbegin();
        return it->second;
    }

    shared_ptr<Node> getLeft(const shared_ptr<Node>& node, int version) {
        return node == nullptr ? nullptr : node->getLeft(version);
    }

    shared_ptr<Node> getRight(const shared_ptr<Node>& node, int version) {
        return node == nullptr ? nullptr : node->getRight(version);
    }

    shared_ptr<Node> getLeft(const shared_ptr<Node>& node) {
        return getLeft(node, latestVersion);
    }

    shared_ptr<Node> getRight(const shared_ptr<Node>& node) {
        return getRight(node, latestVersion);
    }

    void setLeft(shared_ptr<Node>&, shared_ptr<Node>);
    void setRight(shared_ptr<Node>&, shared_ptr<Node>);
    void leftRotate(shared_ptr<Node>&);
    void rightRotate(shared_ptr<Node>&);
    void fixInsert(shared_ptr<Node>);

    void insert(int key) {

        latestVersion++;

        if(getRoot() == nullptr) {
            root[latestVersion] = make_shared<Node>(key);
            getRoot()->color = BLACK;
            return;
        }

        shared_ptr<Node> node = getRoot(), parent = nullptr;

        while(node != nullptr) {
            parent = node;
            if(key < node->key) node = getLeft(node);
            else node = getRight(node);
        }

        node = make_shared<Node>(key, parent);
        if(key < parent->key) setLeft(parent, node);
        else setRight(parent, node);

        // fixInsert(node);
    }

    bool count(int key, int version) {

        shared_ptr<Node> node = getRoot(version);

        while(node != nullptr) {
            if(node->key == key) return true;
            if(key < node->key) node = getLeft(node, version);
            else node = getRight(node, version);
        }

        return false;
    }
};

void RedBlackTree::setLeft(shared_ptr<Node> &node, shared_ptr<Node> left) {

    if(node->mod->type == EMPTY || (node->mod->type == LEFT && node->mod->version == latestVersion)) {
        node->mod->type = LEFT;
        node->mod->version = latestVersion;
        node->mod->node = left;
        if(left != nullptr) left->parent = node;
        return;
    }

    auto newNode = node->copy();
    newNode->left = left;
    if(left != nullptr) left->parent = newNode;
    if(newNode->right != nullptr) newNode->right->parent = newNode;

    if(node->parent == nullptr) {
        root[latestVersion] = newNode;
        node = newNode;
        return;
    }

    if(getLeft(node->parent) == node) setLeft(node->parent, newNode);
    else setRight(node->parent, newNode);

    newNode->parent = node->parent;
    node = newNode;
}

void RedBlackTree::setRight(shared_ptr<Node> &node, shared_ptr<Node> right) {

    if(node->mod->type == EMPTY || (node->mod->type == RIGHT && node->mod->version == latestVersion)) {
        node->mod->type = RIGHT;
        node->mod->version = latestVersion;
        node->mod->node = right;
        if(right != nullptr) right->parent = node;
        return;
    }

    auto newNode = node->copy();
    newNode->right = right;
    if(right != nullptr) right->parent = newNode;
    if(newNode->left != nullptr) newNode->left->parent = newNode;

    if(node->parent == nullptr) {
        root[latestVersion] = newNode;
        node = newNode;
        return;
    }

    if(getLeft(node->parent) == node) setLeft(newNode->parent, newNode);
    else setRight(newNode->parent, newNode);

    newNode->parent = node->parent;
    node = newNode;
}

void RedBlackTree::leftRotate(shared_ptr<Node> &node) {

    if(node == nullptr || getRight(node) == nullptr) return;

    auto right = getRight(node);
    
    setRight(node, getLeft(right));

    if(node->parent == nullptr) {
        root[latestVersion] = right;
        right->parent = nullptr;
    } else if(getLeft(node->parent) == node) {
        setLeft(node->parent, right);
    } else {
        setRight(node->parent, right);
    }

    setLeft(right, node);
}

void RedBlackTree::rightRotate(shared_ptr<Node> &node) {

    if(node == nullptr || getLeft(node) == nullptr) return;

    auto left = node->getLeft(latestVersion);

    setLeft(node, getRight(left));

    if(node->parent == nullptr) {
        root[latestVersion] = left;
        left->parent = nullptr;
    } else if(getLeft(node->parent) == node) {
        setLeft(node->parent, left);
    } else {
        setRight(node->parent, left);
    }

    setRight(left, node);
}

void RedBlackTree::fixInsert(shared_ptr<Node> node) {

    while(node->parent != nullptr && node->parent->color == RED) {

        if(node->parent == getLeft(node->parent->parent)) {
            auto uncle = getRight(node->parent->parent);
            if(uncle != nullptr && uncle->color == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if(node == getRight(node->parent)) {
                    node = node->parent;
                    leftRotate(node);
                }
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                rightRotate(node->parent->parent);
            }
        } else {
            auto uncle = getLeft(node->parent->parent);
            if(uncle != nullptr && uncle->color == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if(node == getLeft(node->parent)) {
                    node = node->parent;
                    rightRotate(node);
                }
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                leftRotate(node->parent->parent);
            }
        }
    }

    getRoot()->color = BLACK;
}

void testInsert() {

    RedBlackTree tree;

    vector<int> keys(10);
    iota(keys.begin(), keys.end(), 1);
    shuffle(keys.begin(), keys.end(), rng);

    for(auto key : keys) {
        cout << "Inserting " << key << endl;
        tree.insert(key);
    }

    for(int i = 1; i <= 10; i++) {
        cout << "Version " << i << ": ";
        for(int j = 1; j <= 10; j++) {
            cout << tree.count(j, i) << " ";
        }
        cout << endl;
    }
}

int main() {

    testInsert();

    return 0;
}