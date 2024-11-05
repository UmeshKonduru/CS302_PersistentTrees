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
    shared_ptr<Modification> mod;

    Node(int key) : 
        key(key), 
        color(RED),
        left(nullptr),
        right(nullptr),
        mod(make_shared<Modification>()) {}

    shared_ptr<Node> copy() {

        auto node = make_shared<Node>(key);
        node->color = color;
        node->left = left;
        node->right = right;
        node->mod = nullptr;

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

    shared_ptr<Node> setLeft(shared_ptr<Node> node, int version) {

        if(mod->type == EMPTY) {
            mod->type = LEFT;
            mod->version = version;
            mod->node = left;
            return nullptr;
        }

        auto newnode = copy();
        newnode->setLeft(node, version);
        return newnode;
    }

    shared_ptr<Node> setRight(shared_ptr<Node> node, int version) {

        if(mod->type == EMPTY) {
            mod->type = RIGHT;
            mod->version = version;
            mod->node = right;
            return nullptr;
        }

        auto newnode = copy();
        newnode->setRight(node, version);
        return newnode;
    }
};

struct RedBlackTree {
    
    map<int, shared_ptr<Node>> roots;
    int latestVersion;

    RedBlackTree() : latestVersion(0) {}
};

int main() {

}
