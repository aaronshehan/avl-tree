
// Source: https://gist.github.com/alexnoz/6f2338bdea22c3064402b1034fcff08c


// AVL tree implementation in C++

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <climits>
#include <queue>

using namespace std;

struct Node {
  int key;
  int height;
  Node* left;
  Node* right;
};

using node_cb = void(*)(Node*);

namespace bstutils {
  bool isBst(const Node* root);
  Node* inOrderSuccessor(Node* root, int key);
  Node* inOrderPredecessor(Node* root, int key);
} // namespace bstutils

class AvlTree {
  Node* root = nullptr;

  public:

    void insert(int key);
    void remove(int key);
    bool exists(int key) const;
    void traverse(node_cb cb) const;
    const Node* getRoot() const;
};

namespace {
  int max(int a, int b) {
    return a > b ? a : b;
  }

  void deleteNode(Node* node) {
    delete node;
  }

  Node* findNode(Node* root, int key) {
    return !root
      ? nullptr
      : root->key == key
        ? root
        : findNode(root->key < key ? root->right : root->left, key);
  }

  Node* findMinNode(Node* root) {
    if (!root) return nullptr;

    Node* min = root;

    while (min->left) {
      min = min->left;
    }

    return min;
  }

  Node* findMaxNode(Node* root) {
    if (!root) return nullptr;

    Node* max = root;

    while (max->right) {
      max = max->right;
    }

    return max;
  }

  int getHeight(const Node* node) {
    return !node ? -1 : node->height;
  }

  int getBalance(const Node* node) {
    return getHeight(node->left) - getHeight(node->right);
  }

  bool nodeExists(const Node* node, int key) {
    return !node
      ? false
      : node->key == key
        ? true
        : nodeExists(key <= node->key ? node->left : node->right, key);
  }

  void rightRotate(Node*& x) {
    Node* y = x->left;
    Node* b = y->right;

    y->right = x;
    x->left = b;

    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    x = y;
  }

  void leftRotate(Node*& y) {
    Node* x = y->right;
    Node* b = x->left;

    x->left = y;
    y->right = b;

    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    y = x;
  }

  void rotateNode(Node*& node) {
    node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

    int balance = getBalance(node);

    if (balance < -1) {
      if (getHeight(node->right->left) < getHeight(node->right->right)) {
        // TODO: refactor
        leftRotate(node);
      } else {
        rightRotate(node->right);
        leftRotate(node);
      }
    } else if (balance > 1) {
      if (getHeight(node->left->left) > getHeight(node->left->right)) {
        // TODO: refactor
        rightRotate(node);
      } else {
        leftRotate(node->left);
        rightRotate(node);
      }
    }
  }

  void insertNode(Node*& node, int key) {
    if (!node) {
      node = new Node { key, 0, nullptr, nullptr };
      return;
    } else if (key <= node->key) {
      insertNode(node->left, key);
    } else {
      insertNode(node->right, key);
    }

    rotateNode(node);
  }

  void traverseInOrder(Node* node, node_cb cb) {
    if (!node) return;

    traverseInOrder(node->left, cb);

    cb(node);

    traverseInOrder(node->right, cb);
  }

  void traverseLevelOrder(Node* root, node_cb cb) {
    if (!root) return;

    std::queue<Node*> q;

    q.push(root);

    while (!q.empty()) {
      Node* cur = q.front();
      q.pop();

      cb(cur);

      if (cur->left) q.push(cur->left);
      if (cur->right) q.push(cur->right);
    }
  }

  void removeNode(Node*& node, int key) {
    if (!node) {
      return;
    } else if (node->key < key) {
      removeNode(node->right, key);
    } else if (node->key > key) {
      removeNode(node->left, key);
    } else {
      // gotcha
      if (!node->left && !node->right) {
        delete node;
        node = nullptr;
      } else if (!node->right) {
        Node* temp = node;
        node = node->left;
        delete temp;
      } else if (!node->left) {
        Node* temp = node;
        node = node->right;
        delete temp;
      } else {
        // 2 children
        Node* min = findMinNode(node->right);
        node->key = min->key;
        removeNode(node->right, min->key);
      }
    }

    if (node) rotateNode(node);
  }
  
  bool isBstUtil(const Node* root, const Node*& prev) {
    if (!root) return true;

    if (!isBstUtil(root->left, prev) || (prev && prev->key >= root->key))
      return false;

    prev = root;

    return isBstUtil(root->right, prev);
  }

  bool isBstUtil(const Node* root, int min, int max) {
    return !root || (
      root->key > min && root->key < max &&
      isBstUtil(root->left, min, root->key) &&
      isBstUtil(root->right, root->key, max)
    );
  }
} // anonymous namespace

bool bstutils::isBst(const Node* root) {
  // const Node* prev = nullptr;
  // return isBstUtil(root, prev);
  return isBstUtil(root, INT_MIN, INT_MAX);
}

Node* bstutils::inOrderSuccessor(Node* root, int key) {
  Node* cur = findNode(root, key);

  if (!cur) return nullptr;

  if (cur->right) return findMinNode(cur->right);

  Node* successor = nullptr;
  Node* ancestor = root;

  while (ancestor != cur) {
    if (cur->key < ancestor->key) {
      successor = ancestor;
      ancestor = ancestor->left;
    } else {
      ancestor = ancestor->right;
    }
  }

  return successor;
}

Node* bstutils::inOrderPredecessor(Node* root, int key) {
  Node* cur = findNode(root, key);

  if (!cur) return nullptr;

  if (cur->left) return findMaxNode(cur->left);

  Node* predecessor = nullptr;
  Node* ancestor = root;

  while (ancestor != cur) {
    if (cur->key > ancestor->key) {
      predecessor = ancestor;
      ancestor = ancestor->right;
    } else {
      ancestor = ancestor->left;
    }
  }

  return predecessor;
}

void AvlTree::insert(int key) {
  insertNode(root, key);
}

void AvlTree::remove(int key) {
  removeNode(root, key);
}

bool AvlTree::exists(int key) const {
  return nodeExists(root, key);
}

void AvlTree::traverse(node_cb cb) const {
  traverseInOrder(root, cb);
}

const Node* AvlTree::getRoot() const {
  return root;
}


int main()
{
    vector<int> input;

    ifstream fin;
    fin.open("input.txt");

    int line;

    while (fin >> line) {
        input.push_back(line);
    }
    fin.close();

    AvlTree t;

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    
    for (const auto& x : input) {
        t.insert(x);
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    cout << "Time to insert: " << (chrono::duration_cast<chrono::nanoseconds>(end - begin).count())  << endl;

    begin = chrono::steady_clock::now();

    t.exists(2000000);

    end = chrono::steady_clock::now();

    cout << "Time to search: " << (chrono::duration_cast<chrono::nanoseconds>(end - begin).count())  << endl;

}
