#include <chrono>
#include <random>
#include <memory>
#include <variant>
#include <map>

using namespace std;

mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());

enum ModType {
    EMPTY, VALUE, LEFT, RIGHT
};

struct Modification;
struct Node;
struct Treap;

struct Modification {

    ModType type;
    int version;
    variant<int, shared_ptr<Node>> data;

    Modification() : type(EMPTY) {}
};

struct Node {

    int weight;
    int value;
    shared_ptr<Node> left, right;
    shared_ptr<Modification> mod;

    Node(int value) :
        weight(rnd()),
        value(value),
        mod(make_shared<Modification>()) {}

    Node(const Node &node) : weight(node.weight) {

        value = node.value;
        left = node.left;
        right = node.right;
        mod = make_shared<Modification>();

        switch(node.mod->type) {
            case VALUE:
            value = get<int>(node.mod->data);
            break;
            case LEFT:
            left = get<shared_ptr<Node>>(node.mod->data);
            break;
            case RIGHT:
            right = get<shared_ptr<Node>>(node.mod->data);
        }
    }

    shared_ptr<Node> clone() {
        return make_shared<Node>(*this);
    }

    int getValue(int version) {

        if(mod->type == VALUE && mod->version <= version) {
            return get<int>(mod->data);
        }

        return value;
    }

    shared_ptr<Node> getLeft(int version) {

        if(mod->type == LEFT && mod->version <= version) {
            return get<shared_ptr<Node>>(mod->data);
        }

        return left;
    }

    shared_ptr<Node> getRight(int version) {

        if(mod->type == RIGHT && mod->version <= version) {
            return get<shared_ptr<Node>>(mod->data);
        }

        return right;
    }

    shared_ptr<Node> setValue(int value, int version) {

        if(mod->type == EMPTY) {
            mod->type = VALUE;
            mod->version = version;
            mod->data = value;
            return nullptr;
        }

        shared_ptr<Node> newnode = clone();

        
    }
};

struct Treap {
    
    int latestVersion;
    map<int, shared_ptr<Node>> roots;

    Treap() : latestVersion(0) {}
};

int main() {

}