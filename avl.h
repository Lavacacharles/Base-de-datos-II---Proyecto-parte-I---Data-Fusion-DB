#include <iostream>
#include <vector>
using namespace std;

//template<class T>
struct Node {
    int data;
    Node* left = nullptr;
    Node* right =  nullptr;
    int height=0;

    bool operator<(const Node &node) const {
        return this->data < node.data;
    }
    bool operator>(const Node &node) const {
        return this->data > node.data;
    }
    bool operator==(const Node &node) const {
        return this->data == node.data;
    }
};

class AVL {
    Node* root = nullptr;

    int getHeight(Node* node) {
        if (node == nullptr) return -1;
        return node->height;
    }
    Node* rightRotate(Node *node) {
        Node *newNode = node->left;
        Node *temp = newNode->right;
        newNode->right = node;
        node->left = temp;

        node->height=max(getHeight(node->left),getHeight(node->right)) + 1;
        newNode->height = max(getHeight(newNode->left),getHeight(newNode->right)) + 1;
        return newNode;
    }
    Node* leftRotate(Node *node) {
        Node *newNode = node->right;
        Node *temp = newNode->left;
        newNode->left = node;
        node->right = temp;

        node->height=max(getHeight(node->left),getHeight(node->right)) + 1;
        newNode->height = max(getHeight(newNode->left),getHeight(newNode->right)) + 1;
        return newNode;
    }
    int getBalance(Node *node) {
        if (node == nullptr)
            return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    Node* insert(Node *parent, int value) {
        if (parent == nullptr) return new Node(value);
        if (value < parent->data) parent->left = insert(parent->left, value);
        else if (value > parent->data) parent->right = insert(parent->right, value);
        else
            return parent;

        parent->height = max(getHeight(parent->left),getHeight(parent->right)) + 1;
        int balance = getBalance(parent);
        if (balance > 1 && value < parent->left->data) return rightRotate(parent);
        if (balance < -1 && value > parent->right->data) return leftRotate(parent);
        if (balance > 1 && value > parent->left->data) {
            parent->left = leftRotate(parent->left);
            return rightRotate(parent);
        }
        if (balance < -1 && value < parent->right->data) {
            parent->right = rightRotate(parent->right);
            return leftRotate(parent);
        }
        return parent;
    }
public:
    AVL(){};

    void buildFromVector(vector<int> vec) {

    };

    void add(int value) {
        cout<<"Add: "<<value<<endl;
        root = insert(root, value);
    }

    void search(int value) {
        Node* current= root;
        bool found = false;
        cout <<endl<<"Search in route:"<<endl;
        while (current!=nullptr) {
            cout <<current->data;//<<"["<<current->height<<"]";
            if(current->data == value) {
                found = true;
                break;
            }
            if (current->data > value) {
                cout <<"(left)";
                current = current->left;
            } else if (current->data < value) {
                cout <<"(right)";
                current = current->right;
            }
        }
        cout<<"\nStatus: "<<((found)?"Found":"Not Found");
    }

};



void testAVL() {
    //vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    AVL avl;
    //avl.show();

    avl.add(5);
    avl.add(2);
    avl.add(7);
    avl.add(9);
    avl.add(6);
    avl.add(8);
    avl.add(3);
    avl.add(1);

    avl.search(1);
    return 0;
}
