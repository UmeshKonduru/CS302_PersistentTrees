// Partially Persistent Red-Black Tree
#include <memory>

using namespace std;

enum Color {
    RED, BLACK
};

enum ModType {
    LEFT, RIGHT
};

struct Node;
struct Modification;

struct Modification {
    ModType type;
    int version;
    shared_ptr<Node> node;
};

struct Node {
    int key;
    Color color;
    shared_ptr<Node> left, right;
    shared_ptr<Modification> mod;
};

int main() {

}
