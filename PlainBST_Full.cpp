#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <set>

using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

enum Mod {
    LEFT, RIGHT, EMPTY
};

struct Modification;
struct Node;

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

struct OrderTree {

    unordered_map<int, int> depth;
    unordered_map<int, vector<int>> parent;

    OrderTree() {
        depth[0] = 0;
        parent[0] = vector<int>(31, -1);
    }

    int findParent(int x, int k) {
        if(x == -1 || k == 0) return x;
        for(int i = 30; i >= 0; i--) {
            if(k >= (1 << i)) return findParent(parent[x][i], k - (1 << i));
        }
        return -1;
    }

    void insert(int x, int y) {
        depth[y] = depth[x] + 1;
        parent[y] = vector<int>(31, -1);
        parent[y][0] = x;
        for(int i = 1; i < 30; i++) {
            if(parent[y][i-1] == -1) break;
            parent[y][i] = parent[parent[y][i-1]][i-1];
        }
    }

    bool isAncestor(int x, int y) {
        return depth[x] <= depth[y] && findParent(y, depth[y] - depth[x]) == x;
    }
};

struct Tree {

    int currentVersion;
    map<int, shared_ptr<Node>> root;
    OrderTree versions;

    Tree() : currentVersion(0) { root[0] = nullptr; }

    shared_ptr<Node> clone(const shared_ptr<Node>& node) {
        auto newNode = make_shared<Node>(node->key);
        newNode->left = getLeft(node);
        newNode->right = getRight(node);
        return newNode;
    }

    shared_ptr<Node> getLeft(const shared_ptr<Node>& node, int version) {
        if(node->mod->type == LEFT && versions.isAncestor(node->mod->version, version)) return node->mod->node;
        return node->left;
    }

    shared_ptr<Node> getRight(const shared_ptr<Node>& node, int version) {
        if(node->mod->type == RIGHT && versions.isAncestor(node->mod->version, version)) return node->mod->node;
        return node->right;
    }

    shared_ptr<Node> getLeft(const shared_ptr<Node>& node) {
        return getLeft(node, currentVersion);
    }

    shared_ptr<Node> getRight(const shared_ptr<Node>& node) {
        return getRight(node, currentVersion);
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

    shared_ptr<Node> insert(const shared_ptr<Node>& node, int key) {

        if(!node) return make_shared<Node>(key);

        if(key < node->key) {
            auto left = insert(getLeft(node), key);
            return setLeft(node, left);
        }

        if(key > node->key) {
            auto right = insert(getRight(node), key);
            return setRight(node, right);
        }

        return node;
    }

    shared_ptr<Node> erase(const shared_ptr<Node>& node, int key) {

        if(!node) return nullptr;

        if(key < node->key) {
            auto left = erase(getLeft(node), key);
            return setLeft(node, left);
        }

        if(key > node->key) {
            auto right = erase(getRight(node), key);
            return setRight(node, right);
        }

        if(!getLeft(node)) return getRight(node);
        if(!getRight(node)) return getLeft(node);

        auto succ = getRight(node);
        while(getLeft(succ)) succ = getLeft(succ);

        auto newNode = make_shared<Node>(succ->key);
        newNode->left = getLeft(node);

        auto right = erase(getRight(node), succ->key);
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

    void insert(int key, int version) {
        ++currentVersion;
        versions.insert(version, currentVersion);
        root[currentVersion] = insert(root[version], key);
    }

    void erase(int key, int version) {
        ++currentVersion;
        versions.insert(version, currentVersion);
        root[currentVersion] = erase(root[version], key);
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

    set<int> traverse(const shared_ptr<Node>& node, int version) {
        if(!node) return {};
        auto left = traverse(getLeft(node, version), version);
        auto right = traverse(getRight(node, version), version);
        set<int> res;
        res.insert(node->key);
        res.insert(left.begin(), left.end());
        res.insert(right.begin(), right.end());
        return res;
    }

    set<int> traverse(int version) {
        return traverse(root[version], version);
    }
};

void test() {

    Tree tree;

    vector<int> keys(1000);
    for(int &k : keys) k = uniform_int_distribution<int>(1,10)(rng);

    vector<set<int>> versions(1001);

    int i = 1;
    for(int k : keys) {
        int v = uniform_int_distribution<int>(0,i-1)(rng);
        versions[i] = versions[v];
        if(tree.find(k,v)) {
            tree.erase(k,v);
            versions[i].erase(k);
        } else {
            tree.insert(k,v);
            versions[i].insert(k);
        }
        i++;
    }

    for(int i = 0; i < 1000; i++) {
        auto res = tree.traverse(i);
        if(res != versions[i]) {
            cout << "Mismatch at version " << i << endl;
            cout << "Expected: ";
            for(int x : versions[i]) cout << x << " ";
            cout << endl;
            cout << "Got: ";
            for(int x : res) cout << x << " ";
            cout << endl;
            return;
        }
    }
}


int main() {

    test();
}