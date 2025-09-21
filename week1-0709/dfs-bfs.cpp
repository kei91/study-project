#include <iostream>
#include <queue>

class Node {
public:
    int data;
    Node* left, * right;

    // Constructor to initialize a new node
    Node(int value) {
        data = value;
        left = nullptr;
        right = nullptr;
    }
};

// https://www.geeksforgeeks.org/dsa/level-order-tree-traversal/
// https://www.geeksforgeeks.org/dsa/dfs-traversal-of-a-tree-using-recursion/

Node* CreateTree()
{
    //      1
    //     / \
    //    2    3
    //   /  \    \
    //  4    5    6

    Node* root = new Node(1);
    root->left = new Node(2);
    root->right = new Node(3);
    root->left->left = new Node(4);
    root->left->right = new Node(5);
    root->right->right = new Node(6);

    return root;
}

void printDFS(Node* node)
{
    if (node == nullptr)
        return;

    std::cout << node->data << " ";

    printDFS(node->left);
    printDFS(node->right);
}

void printBFS(Node* node)
{
    std::queue<Node*> q;
    q.push(node);

    while (!q.empty()) {
        int lvl = static_cast<int>(q.size());
        for (int i = 0; i < lvl; ++i) {
            Node* n = q.front();
            q.pop();

            std::cout << n->data << " ";

            if (n->left)
                q.push(n->left);

            if (n->right)
                q.push(n->right);
        }
    }
}

int main()
{
    Node* root = CreateTree();
    printDFS(root);

    std::cout << std::endl;

    printBFS(root);
}
