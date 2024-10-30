#include <chrono>
#include <random>
#include <memory>
#include <map>

using namespace std;

mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());

enum ModType {
    EMPTY, VALUE, LEFT, RIGHT
};

struct Modification;
struct Node;
struct Treap;

struct Modifcation {

    ModType type;
    int version;
    
    union {
        int value;
        shared_ptr<Node> node;
    } data;
};

struct Node {

    int weight;
    int value;
    shared_ptr<Node> left, right;
    shared_ptr<Modification> mod;
};

struct Treap {

    
};

int main() {

}
