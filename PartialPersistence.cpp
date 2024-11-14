// Partially Persistent Red-Black Tree

#include <memory>
#include <map>

using namespace std;

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
        node->left = left;
        node->right = right;

        switch(mod->type) {
            case LEFT:
            node->left = mod->node;
            break;
            case RIGHT:
            node->right = mod->node;
        }

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

    RedBlackTree() : latestVersion(0) {}

    shared_ptr<Node> getRoot(int version) {
        auto it = --(root.upper_bound(version));
        return it->second;
    }

    shared_ptr<Node> getRoot() {
        auto it = root.rbegin();
        return it->second;
    }

    void setLeft(shared_ptr<Node>&, shared_ptr<Node>);
    void setRight(shared_ptr<Node>&, shared_ptr<Node>);
    void leftRotate(shared_ptr<Node>&);
    void rightRotate(shared_ptr<Node>&);
    void fixInsert(shared_ptr<Node>);
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

    if(node->parent == nullptr) {
        root[latestVersion] = newNode;
        node = newNode;
        return;
    }

    if(node->parent->left == node) setLeft(node->parent, newNode);
    else setRight(node->parent, newNode);

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

    if(node->parent == nullptr) {
        root[latestVersion] = newNode;
        node = newNode;
        return;
    }

    if(node->parent->left == node) setLeft(node->parent, newNode);
    else setRight(node->parent, newNode);

    node = newNode;
}

void RedBlackTree::leftRotate(shared_ptr<Node> &node) {

    if(node == nullptr || node->getRight(latestVersion) == nullptr) return;

    auto right = node->getRight(latestVersion);
    
    setRight(node, right->getLeft(latestVersion));

    if(node->parent == nullptr) {
        root[latestVersion] = right;
        right->parent = nullptr;
    } else if(node->parent->left == node) {
        setLeft(node->parent, right);
    } else {
        setRight(node->parent, right);
    }

    setLeft(right, node);
}

void RedBlackTree::rightRotate(shared_ptr<Node> &node) {

    if(node == nullptr || node->getLeft(latestVersion) == nullptr) return;

    auto left = node->getLeft(latestVersion);

    setLeft(node, left->getRight(latestVersion));

    if(node->parent == nullptr) {
        root[latestVersion] = left;
        left->parent = nullptr;
    } else if(node->parent->left == node) {
        setLeft(node->parent, left);
    } else {
        setRight(node->parent, left);
    }

    setRight(left, node);
}

void RedBlackTree::fixInsert(shared_ptr<Node> node) {

    shared_ptr<Node> parent, grandParent, uncle;

    while(node->parent != nullptr && node->parent->color == RED) {

        parent = node->parent, grandParent = parent->parent;
        
        if(parent == grandParent->getLeft(latestVersion)) {
            uncle = grandParent->getRight(latestVersion);
            if(uncle != nullptr && uncle->color == RED) {
                grandParent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                node = grandParent;
            } else {
                if(node == parent->getRight(latestVersion)) {
                    leftRotate(parent);
                    node = parent;
                    parent = node->parent;
                }
                rightRotate(grandParent);
                swap(parent->color, grandParent->color);
                node = parent;
            }
        } else {
            uncle = grandParent->getLeft(latestVersion);
            if(uncle != nullptr && uncle->color == RED) {
                grandParent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                node = grandParent;
            } else {
                if(node == parent->getLeft(latestVersion)) {
                    rightRotate(parent);
                    node = parent;
                    parent = node->parent;
                }
                leftRotate(grandParent);
                swap(parent->color, grandParent->color);
                node = parent;
            }
        }
    }

    getRoot()->color = BLACK;
}

int main() {

}