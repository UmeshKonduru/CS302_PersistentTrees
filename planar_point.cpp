#include <iostream>
#include <vector>
#include <utility>
#include <map>
#include <algorithm>
#include <memory>
#include <numeric>
#include <random>
#include <fstream>
#include <chrono>
#include <thread>
using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

enum Mod {
    LEFT, RIGHT, EMPTY
};
int xglobe=0;

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

    pair<pair<int, int>, pair<int, int>> key;//line
    shared_ptr<Node> left, right;
    shared_ptr<Modification> mod;

    Node(pair<pair<int, int>, pair<int, int>> key) : key(key), left(nullptr), right(nullptr), mod(make_shared<Modification>()) {}
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
    double Ycord(pair<pair<int, int>, pair<int, int>> key){
        int x1 = key.first.first;
        int y1 = key.first.second;
        int x2 = key.second.first;
        int y2 = key.second.second;
        return (1.0*(y2-y1)*(xglobe-x1))/(1.0*(x2-x1))+y1;
    }
    bool isLess(pair<pair<int, int>, pair<int, int>> key1,pair<pair<int, int>, pair<int, int>> key2) {//is below
        return Ycord(key1) < Ycord(key2);
    }
    shared_ptr<Node> insertKey(const shared_ptr<Node>& node, pair<pair<int, int>, pair<int, int>> key) {

        if(!node) return make_shared<Node>(key);

        // if(key < node->key) {
        if(isLess(key,node->key)) {
            auto left = insertKey(getLeft(node), key);
            return setLeft(node, left);
        }

        // if(key > node->key) {
        if(isLess(node->key,key)) {
            auto right = insertKey(getRight(node), key);
            return setRight(node, right);
        }

        return node;
    }

    shared_ptr<Node> deleteKey(const shared_ptr<Node>& node,  pair<pair<int, int>, pair<int, int>> key) {

        if(!node) return nullptr;

        // if(key < node->key) {
        if(isLess(key,node->key)) {
            auto left = deleteKey(getLeft(node), key);
            return setLeft(node, left);
        }

        // if(key > node->key) {
        if(isLess(node->key,key)) {
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
    double Ycordx(pair<pair<int, int>, pair<int, int>> line,int x) {
        int x1 = line.first.first;
        int y1 = line.first.second;
        int x2 = line.second.first;
        int y2 = line.second.second;
        return (1.0*(y2-y1)*(x-x1))/(1.0*(x2-x1))+y1;
    }
    bool checkAbove(pair<pair<int, int>, pair<int, int>> line,pair<int,int> point) {
        return point.second >= Ycordx(line,point.first);
    }
    pair<pair<int, int>, pair<int, int>> find(pair<int,int> point, int version) {
        auto node = root[version];
        pair<pair<int, int>, pair<int, int>> line;
        line = node->key;
        while(node) {
            if(checkAbove(node->key,point)) {
                line = node->key;
                node = getRight(node, version);
            }
            else node = getLeft(node, version);
            // auto kk = node->key;
            // cout << "Line: (" << kk.first.first << "," << kk.first.second << ") -> (" << kk.second.first << "," << kk.second.second << ")" << endl;
        }
        return line;
    }

    void insert(pair<pair<int, int>, pair<int, int>> key) {
        currentVersion++;
        root[currentVersion] = insertKey(getRoot(), key);
    }

    void erase(pair<pair<int, int>, pair<int, int>> key) {
        currentVersion++;
        root[currentVersion] = deleteKey(getRoot(), key);
    }

    void inorder(const shared_ptr<Node>& node, int version) {
        if(!node) return;
        inorder(getLeft(node, version), version);
        auto line = node->key;
        cout << "Line: (" << line.first.first << "," << line.first.second << ") -> (" << line.second.first << "," << line.second.second << ")" << endl;
        inorder(getRight(node, version), version);
    }

    void inorder(int version) {
        inorder(root[version], version);
        cout << endl;
    }
};

// Function to find intersections
void findIntersections(
    vector<pair<pair<int, int>, pair<int, int>>> lines,
    map<pair<int, int>, pair<pair<pair<int, int>, pair<int, int>>, pair<pair<int, int>, pair<int, int>>>>& intersections) {
    
    for (int i = 0; i < lines.size(); i++) {
        for (int j = i + 1; j < lines.size(); j++) {
            int x1 = lines[i].first.first;
            int y1 = lines[i].first.second;
            int x2 = lines[i].second.first;
            int y2 = lines[i].second.second;
            int x3 = lines[j].first.first;
            int y3 = lines[j].first.second;
            int x4 = lines[j].second.first;
            int y4 = lines[j].second.second;

            // Line equation coefficients
            int a1 = y2 - y1;
            int b1 = x1 - x2;
            int c1 = a1 * x1 + b1 * y1;

            int a2 = y4 - y3;
            int b2 = x3 - x4;
            int c2 = a2 * x3 + b2 * y3;

            int det = a1 * b2 - a2 * b1;
            if (det == 0) {
                // Lines are parallel or coincident
                continue;
            }

            // Calculate intersection point
            int x = (b2 * c1 - b1 * c2) / det;
            int y = (a1 * c2 - a2 * c1) / det;

            // Check if the intersection point is within bounds
            if (x >= 0 && x <= 100 && y >= 0 && y <= 100) {
                intersections[{x, y}] = make_pair(lines[i], lines[j]);
            }
        }
    }
}
double findSlope(pair<pair<int, int>, pair<int, int>> line) {
    return ((1.0*(line.second.second - line.first.second)) / (1.0*(line.second.first - line.first.first)));
}
bool touchSmallerX(pair<pair<int, int>, pair<int, int>> line,int slab) {
    return (line.first.first == slab);
}
bool repetitions(vector<int>v){
    sort(v.begin(),v.end());
    for(int i=0;i<v.size()-1;i++){
        if(v[i]==v[i+1]){
            return true;
        }
    }
    return false;
}
void preprocess( Tree &tree ,vector<pair<pair<int, int>, pair<int, int>>> &lines,vector<int> &slabEnds,map<int,int> &slabToVersion) {
    // Define boundaries: x and y range from 0 to 100
    lines.push_back(make_pair(make_pair(0, 0), make_pair(100, 0)));
    lines.push_back(make_pair(make_pair(0, 100), make_pair(100, 100)));
    // Map to store intersection points and corresponding intersecting lines
    map<pair<int, int>, pair<pair<pair<int, int>, pair<int, int>>, pair<pair<int, int>, pair<int, int>>>> intersections;

    // Find intersections
    findIntersections(lines, intersections);

    // Print the intersection points and intersecting lines
    for (auto i : intersections) {
        cout << "Intersection point: " << i.first.first << "," << i.first.second << endl;
        cout << "Line 1: (" << i.second.first.first.first << "," << i.second.first.first.second << ") -> ("
             << i.second.first.second.first << "," << i.second.first.second.second << ")" << endl;
        cout << "Line 2: (" << i.second.second.first.first << "," << i.second.second.first.second << ") -> ("
             << i.second.second.second.first << "," << i.second.second.second.second << ")" << endl;
    }
    for(auto i : intersections) {
        slabEnds.push_back(i.first.first);
    }
    sort(slabEnds.begin(), slabEnds.end());
    if(repetitions(slabEnds)){
        cout<<"Repetitions in slab ends"<<endl;
        exit(0);
    }
    // if(slabEnds[intersections.size()-1] != 100)
    //     slabEnds.push_back(100);
    cout << "Slab ends: ";
    for(auto i : slabEnds) {
        cout << i << " ";
    }
    cout <<endl;
    //insert the initial line
    // xglobe=0;
    // tree.insert(make_pair(make_pair(0, 0), make_pair(100, 0)));
    int prev = 0;
    int version = 0;
    //insert all the lines where x axis of first line is 0
    for(int i = 0; i < lines.size(); i++) {
        if(lines[i].first.first == 0) {
            xglobe=0;
            tree.insert(lines[i]);
            version++;
        }
    }
    slabToVersion[0]=version;
    for(auto slabs : slabEnds) {
        version++;
        //find a line that has an x axis value equal to the slab end
        pair<pair<int, int>, pair<int, int>> line;
        for(auto i : lines){
            if(i.first.first == slabs || i.second.first == slabs) {
                line = i;
                break;
            }
        }
        if(line.first.first != slabs && line.second.first != slabs) {
            pair<pair<int, int>, pair<int, int>> line1, line2;
            for(auto i : intersections) {
                if(i.first.first == slabs) {
                    line1 = i.second.first;
                    line2 = i.second.second;
                    break;
                }
            }
            cout << "intersection at " << slabs << endl;
            cout << "Line1: (" << line1.first.first << "," << line1.first.second << ") -> (" << line1.second.first << "," << line1.second.second << ")" << endl;
            cout << "Line2: (" << line2.first.first << "," << line2.first.second << ") -> (" << line2.second.first << "," << line2.second.second << ")" << endl;
            xglobe=(prev+slabs)/2;
            tree.erase(line1);
            tree.erase(line2);
            xglobe=slabs+1;
            tree.insert(line1);
            tree.insert(line2);
            prev = slabs;
            version+=3;
            slabToVersion[slabs]=version;
            continue;
        }
        cout << "Just a line at " << slabs << endl;
        cout << "Line: (" << line.first.first << "," << line.first.second << ") -> (" << line.second.first << "," << line.second.second << ")" << endl;
        if(touchSmallerX(line,slabs)){
            xglobe=slabs+1;
            tree.insert(line);
        }
        else{
            xglobe=(prev+slabs)/2;
            tree.erase(line);
        }
        prev = slabs;
        slabToVersion[slabs]=version;
    }
    slabEnds.push_back(0);
    sort(slabEnds.begin(), slabEnds.end());
    for(auto i : slabToVersion) {
        cout << "slab: " << i.first << " version: " << i.second << endl;
        tree.inorder(i.second);
    }
}
int lastSlabLess(vector<int> slabEnds,int slab) {//do binary search
    int l = 0;
    int r = slabEnds.size()-1;
    int ans = 0;
    while(l <= r) {
        int mid = l + (r-l)/2;
        if(slabEnds[mid] < slab) {
            ans = mid;
            l=mid+1;
        }
        else {
            r=mid-1;
        }
    }
    return slabEnds[ans];
}
pair<pair<int, int>, pair<int, int>> query(pair<int,int> point,vector<int> slabEnds,map<int,int> slabToVersion,Tree &tree) {
    int slab = lastSlabLess(slabEnds,point.first);
    // cout << "Slab: " << slab << endl;
    int version = slabToVersion[slab];
    // cout << "Version: " << version << endl;
    // cout << "Inorder:" << endl;
    // tree.inorder(slabToVersion[slab]);
    pair<pair<int, int>, pair<int, int>> line = tree.find(point,version);
    cout << "Point:" << point.first << "," << point.second << endl;
    cout << "Line: (" << line.first.first << "," << line.first.second << ") -> (" << line.second.first << "," << line.second.second << ")" << endl;
    pair<pair<int, int>, pair<int, int>> result = line;
    cout << endl;
    return result;
}

int main() {
    vector<pair<pair<int, int>, pair<int, int>>> lines = {
        {{15, 0}, {82, 100}},  // Line 1
        {{5, 100}, {95, 0}},  // Line 2
        {{0,95},{100,80}},
        {{4, 0}, {100, 100}},
        {{0,20},{100,40}},
        {{60,0},{100,70}},
        {{0,60},{27,100}},
        {{0,40},{55,100}},
        {{0,55},{100,60}}
    };

    pair<int,int> point;

    Tree tree;
    vector<int> slabEnds;
    map<int,int> slabToVersion;
    preprocess(tree,lines,slabEnds,slabToVersion);


    for(int i=0;i<10;i++){
        // Query the tree
        cout << "Query " << i+1 << "    :" ;
        //generate a random point
        point.first = rng() % 96 + 2;
        point.second = rng() % 96 + 2;
        // cout << "Point: " << point.first << "," << point.second << endl;
        pair<pair<int, int>, pair<int, int>> result = query(point,slabEnds,slabToVersion,tree);
        lines.push_back(result);
        // Write data to a file
        std::ofstream outFile("input_data.txt");
        if (!outFile) {
            std::cerr << "Error: Could not open the file for writing.\n";
            return 1;
        }

        // Write lines to the file
        for (const auto& line : lines) {
            outFile << line.first.first << " " << line.first.second << " "
                    << line.second.first << " " << line.second.second << "\n";
        }

        // Write the point to the file
        outFile << point.first << " " << point.second << "\n";
        outFile.close();
        // Execute the Python script

        std::string command = "python3 lines.py";
        int ret_code = std::system(command.c_str());
        if (ret_code != 0) {
            std::cerr << "Error: Python script execution failed.\n";
            return 1;
        }
        // Wait for 2 seconds before the next iteration
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //remove the last line 
        lines.pop_back();
    }
    return 0;
}