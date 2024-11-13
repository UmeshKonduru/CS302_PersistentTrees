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
    
    map<int, shared_ptr<Node>> roots;
    int latestVersion;

    RedBlackTree() : latestVersion(0) {}

    void setLeft(shared_ptr<Node>, shared_ptr<Node>);
    void setRight(shared_ptr<Node>, shared_ptr<Node>);
};

void RedBlackTree::setLeft(shared_ptr<Node> node, shared_ptr<Node> left) {

    if(node->mod->type == EMPTY) {
        node->mod->type = LEFT;
        node->mod->version = latestVersion;
        node->mod->node = left;
        return;
    }

    auto newNode = node->copy();
    newNode->left = left;

    if(node->parent == nullptr) {
        roots[latestVersion] = newNode;
        return;
    }

    if(node->parent->left == node) setLeft(node->parent, newNode);
    else setRight(node->parent, newNode);
}

void RedBlackTree::setRight(shared_ptr<Node> node, shared_ptr<Node> right) {

    if(node->mod->type == EMPTY) {
        node->mod->type = RIGHT;
        node->mod->version = latestVersion;
        node->mod->node = right;
        return;
    }

    auto newNode = node->copy();
    newNode->right = right;

    if(node->parent == nullptr) {
        roots[latestVersion] = newNode;
        return;
    }

    if(node->parent->left == node) setLeft(node->parent, newNode);
    else setRight(node->parent, newNode);
}

int main() {

}
