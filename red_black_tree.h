#pragma once
#include "base.h"

namespace sus {
template<typename KeyType, typename ValueType>
class RedBlackTree {
 public:
  RedBlackTree()
      : size_(0) {

  }

  int32 Size() const {
    return size_;
  }

  void SubAllKeys(KeyType const& delta) {
    if (root_ == NULL)
      return;

    std::vector<RBNode*> next;
    RBNode* x = root_.get();
    while (x->left != NULL) {
      next->push_back(x);
      x = x->left;
    }

    while (!next.empty()) {
      RBNode* x = next.back();
      CHECK_GE(x->key, delta);
      x->key -= delta;
    }
  }

  std::vector<KeyType> InorderInterate() const {
    std::vector<KeyType> keys;
    CHECK(root_->parent->IsNil());
    InorderInterate(root_.get(), &keys);
    return keys;
  }

  void Output() const {
    CHECK(root_->parent->IsNil());
    PreOrderOutput(root_.get());
  }

  bool Verify() const {
    if (root_ == NULL || root_->IsNil())
      return true;

    if (!root_->black)
      return false;

    return VerifyBH(root_.get(), NULL);
  }

  ValueType Delete(KeyType const& key) {
    RBNode* node = FindNode(key);
    if (node->key != key)
      return ValueType();

    bool is_black = node->black;
    RBNode* balanced_node = NULL;
    RBNode* successor = NULL;

    if (node->right->IsNil()) {
      balanced_node = node->left;
      NodeTransplant(node, node->left);
      successor = node->parent;
    } else if (node->left->IsNil()) {
      balanced_node = node->right;
      NodeTransplant(node, node->right);
      successor = node->parent;
    } else {
      successor = node->right;
      while (!successor->left->IsNil()) {
        successor = successor->left;
      }

      is_black = successor->black;
      balanced_node = successor->right;

      if (successor->parent != node) {
        NodeTransplant(successor, successor->right);
        successor->right = node->right;
        successor->right->parent = successor;
      }

      NodeTransplant(node, successor);
      successor->left = node->left;
      successor->left->parent = successor;
      successor->black = node->black;
    }

    if (is_black)
      DeletionBalance(balanced_node);

    ValueType value = node->value;
    node->left = NULL;
    node->right = NULL;
    delete node;
    return value;
  }

  bool Insert(KeyType const& key, ValueType const& value) {
    if (!root_) {
      root_.reset(new RBNode(this, key, value));
      return true;
    }

    RBNode* parent = FindNode(key);
    if (parent->key == key)
      return false;

    InsertionBalance(parent->Insert(key, value));
    return true;
  }

 private:

  struct RBNode {
   public:
    explicit RBNode(RBNode* p)
        : black(true),
          tree(p->tree),
          left(NULL),
          right(NULL),
          parent(p) {

    }

    RBNode(RedBlackTree* t, KeyType const& k, ValueType const& v)
        : black(true),
          tree(t),
          left(NilNode(this)),
          right(NilNode(this)),
          parent(NilNode(this)),
          key(k),
          value(v) {
      ++tree->size_;
    }

    RBNode(RBNode* p, KeyType const& k, ValueType const& v)
        : black(false),
          tree(p->tree),
          left(NilNode(this)),
          right(NilNode(this)),
          parent(p),
          key(k),
          value(v) {
      ++tree->size_;
    }

    ~RBNode() {
      if (left != NULL) {
        delete left;
      }

      if (right != NULL) {
        delete right;
      }

      --tree->size_;
    }

    RBNode* Insert(KeyType const& k, ValueType const& v) {
      RBNode* new_one = new RBNode(this, k, v);

      if (k < key) {
        CHECK(left->IsNil());
        left = new_one;
      } else {
        CHECK_GT(k, key);
        CHECK(right->IsNil());
        right = new_one;
      }

      return new_one;
    }

    bool IsNil() const {
      return left == NULL && right == NULL;
    }

    static RBNode* NilNode(RBNode* parent) {
      return new RBNode(parent);
    }

    bool black;
    RedBlackTree* tree;
    RBNode* left;
    RBNode* right;
    RBNode* parent;
    KeyType key;
    ValueType value;
  };

  std::unique_ptr<RBNode> root_;
  int32 size_;

  void NodeTransplant(RBNode* node, RBNode* substitute) {
    if (node->parent->IsNil()) {
      root_.release();
      root_.reset(substitute);
    } else {
      RBNode* parent = node->parent;
      if (node == parent->right) {
        parent->right = substitute;
      } else {
        CHECK_EQ(node, parent->left);
        parent->left = substitute;
      }
    }

    if (substitute)
      substitute->parent = node->parent;
  }

  void rightRotate(RBNode* node) {
    RBNode* new_top = node->left;
    RBNode* leaf_changed = new_top->right;

    NodeTransplant(node, new_top);
    node->left = leaf_changed;
    leaf_changed->parent = node;

    new_top->right = node;
    node->parent = new_top;
  }

  void leftRotate(RBNode* node) {
    RBNode* new_top = node->right;
    RBNode* leaf_changed = new_top->left;

    NodeTransplant(node, new_top);
    node->right = leaf_changed;
    leaf_changed->parent = node;

    new_top->left = node;
    node->parent = new_top;
  }

  bool VerifyBH(RBNode* node, int* bh) const {
    int r_bh = 0;
    if (node->right->IsNil()) {
      r_bh = 1;
    } else {
      if (!node->black && !node->right->black) {
        return false;
      }

      if (!VerifyBH(node->right, &r_bh))
        return false;
    }

    int l_bh = 0;
    if (node->left->IsNil()) {
      l_bh = 1;
    } else {
      if (!node->black && !node->left->black) {
        return false;
      }

      if (!VerifyBH(node->left, &l_bh))
        return false;
    }

    if (r_bh == l_bh) {
      if (bh != NULL)
        *bh = r_bh;
      return true;
    }

    return false;
  }

  int RightBH(RBNode* node) {
    if (node->right->IsNil()) {
      return 1;
    }

    return RightBH(node->right);
  }

  int LeftBH(RBNode* node) {
    if (node->left->IsNil()) {
      return 1;
    }

    return LeftBH(node->left);
  }

  void InsertionBalance(RBNode* z) {
#define INSERTION_BALANCE(z, parent, grade_parent, which_child, another_child) \
    RBNode* y = grade_parent->another_child;\
    if (!y->black) { \
      y->black = true; \
      parent->black = true; \
      grade_parent->black = false; \
      z = grade_parent; \
    } else { \
      if (z == parent->another_child) { \
        z = parent; \
        which_child##Rotate(z); \
      } \
    \
      z->parent->black = true; \
      z->parent->parent->black = false; \
      another_child##Rotate(z->parent->parent);\
    }

    while (!z->parent->IsNil() && !z->parent->parent->IsNil()
        && !z->parent->black) {
      RBNode* parent = z->parent;
      RBNode* grade_parent = parent->parent;
      CHECK(!grade_parent->IsNil());
      if (parent == grade_parent->left) {
        INSERTION_BALANCE(z, parent, grade_parent, left, right);
      } else {
        INSERTION_BALANCE(z, parent, grade_parent, right, left);
      }
    }

    root_->black = true;
  }

  void DeletionBalance(RBNode* node) {
#define DELETION_BALANCE(node, parent, which_child, another_child) \
    RBNode* w = parent->another_child; \
    CHECK(!w->IsNil()); \
    if (!w->black) { \
      w->black = true; \
      parent->black = false; \
      which_child##Rotate(parent); \
      w = parent->another_child; \
    } \
    \
    if (w->which_child->black && w->another_child->black) { \
      w->black = false; \
      node = parent; \
    } else { \
      if (w->another_child->black) { \
        w->which_child->black = true; \
        w->black = false; \
        another_child##Rotate(w); \
        w = parent->another_child; \
      } \
      \
      w->black = parent->black; \
      parent->black = true; \
      CHECK(!w->another_child->IsNil()); \
      w->another_child->black = true; \
      which_child##Rotate(parent); \
      node = root_.get(); \
    }

    while (node->black && !node->parent->IsNil()) {
      RBNode* parent = node->parent;
      if (node == parent->left) {
        DELETION_BALANCE(node, parent, left, right);
      } else {
        DELETION_BALANCE(node, parent, right, left);
      }
    }

    if (!node->IsNil())
      node->black = true;
  }

  RBNode* FindNode(KeyType const& key) const {
    RBNode* node = root_.get();
    while (!node->IsNil()) {
      if (key == node->key)
        return node;

      if (key > node->key) {
        if (!node->right->IsNil()) {
          node = node->right;
          continue;
        }

        return node;
      }

      if (!node->left->IsNil()) {
        node = node->left;
        continue;
      }

      return node;
    }

    LOG_FATAL << "Can not reach here";
    return NULL;
  }

  void InorderInterate(RBNode* root, std::vector<KeyType>* keys) const {
    if (!root->left->IsNil())
      InorderInterate(root->left, keys);

    keys->push_back(root->key);

    if (!root->right->IsNil())
      InorderInterate(root->right, keys);
  }

  void PreOrderOutput(RBNode* node) const {
    std::cout << "{" << node->key << "[" << (node->black ? "B" : "R") << "]";
    if (node->left->IsNil()) {
      PreOrderOutput(node->left);
    }

    if (node->right->IsNil()) {
      PreOrderOutput(node->right);
    }

    std::cout << "}";
  }

  DISALLOW_COPY_AND_ASSIGN(RedBlackTree);
}
;
}
