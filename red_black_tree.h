#pragma once
#include "base.h"

namespace sus {
template<typename KeyType, typename ValueType>
class RedBlackTree {
 public:
  RedBlackTree() {

  }

  void SubAllKeys(KeyType const& delta) {

  }

  std::vector<KeyType> InorderInterate() const {
    std::vector<KeyType> keys;
    InorderInterate(root_.get(), &keys);
    return keys;
  }

  ValueType Delete(KeyType const& key) {
    RBNode* node = FindNode(key);
    if (node == NULL)
      return ValueType();

    RBNode* left_most = node->LeftMost();
    if (node != left_most) {
      node->Swap(left_most);
    }

    CHECK(!left_most->HasLeft());
    RBNode* substitute = NULL;
    if (left_most->HasRight()) {
      substitute = left_most->ReleaseRight();
    }

    ValueType value = left_most->Value();
    delete left_most->Parent()->SwapLeft(substitute);
    DeletionBalance(node);
    return value;
  }

  bool Insert(KeyType const& key, ValueType const& value) {
    if (!root_) {
      root_.reset(new RBNode(key, value));
      return true;
    }

    RBNode* parent = FindParent(key);
    if (parent == NULL)
      return false;

    InsertionBalance(parent->Insert(key, value));
    return false;
  }

 private:
  class RBNode {
   public:
    RBNode(KeyType const& key, ValueType const& value)
        : black_(true),
          parent_(NULL),
          key_(key),
          value_(value) {

    }

    RBNode(RBNode* parent, KeyType const& key, ValueType const& value)
        : black_(false),
          parent_(parent),
          key_(key),
          value_(value) {

    }

    RBNode* Insert(KeyType const& key, ValueType const& value) {
      RBNode* new_one = new RBNode(this, key, value);

      if (key < key_) {
        CHECK(left_ == NULL);
        left_.reset(new_one);
      } else {
        CHECK_GT(key, key_);
        CHECK(right_ == NULL);
        right_.reset(new_one);
      }

      return new_one;
    }

    KeyType const& Key() const {
      return key_;
    }

    ValueType Value() const {
      return value_;
    }

    bool IsBlack() const {
      return black_;
    }

    void Repaint() {
      black_ = !black_;
    }

    RBNode* Parent() const {
      return parent_;
    }

    bool IsRoot() const {
      return parent_ == NULL;
    }

    RBNode* Uncle() const {
      return parent_->Left() == this ? parent_->Right() : parent_->Left();
    }

    bool HasLeft() const {
      return left_ != NULL;
    }

    RBNode* Left() const {
      return left_.get();
    }

    RBNode* ReleaseLeft() {
      if (left_ != NULL)
        left_->parent_ = NULL;
      return left_.release();
    }

    void SetLeft(RBNode* left) {
      if (left != NULL)
        left->parent_ = this;

      left_.reset(left);
    }

    RBNode* SwapLeft(RBNode* node) {
      RBNode* left = ReleaseLeft();
      node->parent_ = this;
      left_.reset(node);
      return left;
    }

    RBNode* SwapRight(RBNode* node) {
      RBNode* right = ReleaseRight();
      node->parent_ = this;
      right_.reset(node);
      return right;
    }

    bool HasRight() const {
      return right_ != NULL;
    }

    RBNode* Right() const {
      return right_.get();
    }

    RBNode* ReleaseRight() {
      if (right_ != NULL)
        right_->parent_ = NULL;
      return right_.release();
    }

    void SetRight(RBNode* right) {
      if (right != NULL)
        right->parent_ = this;

      right_.reset(right);
    }

    RBNode* LeftMost() const {
      RBNode* node = this;
      while (node->HasLeft())
        node = node->Left();
      return node;
    }

    void RotateLeft() {
      RBNode* right = ReleaseRight();
      CHECK(right != NULL);
      if (parent_ != NULL) {
        if (parent_->HasRightAs(this)) {
          CHECK_EQ(this, parent_->SwapRight(right));
        } else {
          CHECK(parent_->HasLeftAs(this));
          CHECK_EQ(this, parent_->SwapLeft(right));
        }
      }

      SetRight(right->SwapLeft(this));
    }

    void RotateRight() {
      RBNode* left = ReleaseLeft();
      CHECK(left != NULL);
      if (parent_ != NULL) {
        if (parent_->HasRightAs(this)) {
          CHECK_EQ(this, parent_->SwapRight(left));
        } else {
          CHECK(parent_->HasLeftAs(this));
          CHECK_EQ(this, parent_->SwapLeft(left));
        }
      }

      SetLeft(left->SwapRight(this));
    }

    void Swap(RBNode* another) {
      std::swap(key_, another->key_);
      std::swap(value_, another->value_);
    }

    bool HasLeftAs(RBNode* node) const {
      CHECK(left_.get() == node || right_.get() == node);
      return left_.get() == node;
    }

    bool HasRightAs(RBNode* node) const {
      CHECK(left_.get() == node || right_.get() == node);
      return right_.get() == node;
    }

   private:
    bool black_;
    std::unique_ptr<RBNode> left_;
    std::unique_ptr<RBNode> right_;
    RBNode* parent_;
    KeyType key_;
    ValueType value_;

    DISALLOW_COPY_AND_ASSIGN(RBNode);
  };

  std::unique_ptr<RBNode> root_;

  void InsertionBalance(RBNode* node) {
    // The node is the root.
    if (node->IsRoot()) {
      if (!node->IsBlack()) {
        node->Repaint();
      }

      if (node != root_.get()) {
        root_.release();
        root_.reset(node);
      }

      return;
    }

    // The parent is black
    RBNode* parent = node->Parent();
    if (parent->IsBlack()) {
      return;
    }

    // The parent is red
    RBNode* grade_parent = parent->Parent();
    CHECK(grade_parent != NULL);

    RBNode* uncle = node->Uncle();
    // The uncle is red
    if (uncle != NULL && !uncle->IsBlack()) {
      parent->Repaint();
      uncle->Repaint();
      grade_parent->Repaint();
      InsertionBalance(grade_parent);
      return;
    }

    // The uncle is black
    if (grade_parent->HasLeftAs(parent) && parent->HasRightAs(node)) {
      parent->RotateLeft();
      node = node->Left();
    } else if (grade_parent->HasRightAs(parent) && parent->HasLeftAs(node)) {
      parent->RotateRight();
      node = node->Right();
    }

    parent = node->Parent();
    grade_parent = parent->Parent();

    parent->Repaint();
    grade_parent->Repaint();

    if (parent->HasLeftAs(node)) {
      grade_parent->RotateRight();
    } else {
      grade_parent->RotateLeft();
    }
  }

  void DeletionBalance(RBNode* node) {

  }

  RBNode* FindParent(KeyType const& key) {
    RBNode* node = root_.get();
    while (node != NULL) {
      if (key == node->Key())
        return NULL;

      if (key > node->Key()) {
        if (node->HasRight()) {
          node = node->Right();
          continue;
        } else {
          return node;
        }
      }

      if (node->HasLeft()) {
        node = node->Left();
        continue;
      } else {
        return node;
      }
    }

    LOG_FATAL<< "Can not reach here";
    return NULL;
  }

  RBNode* FindNode(KeyType const& key) {
    RBNode* node = root_.get();
    while (node != NULL) {
      if (key == node->Key())
        return node;

      if (key > node->Key()) {
        if (node->HasRight()) {
          node = node->Right();
          continue;
        }

        return NULL;
      }

      if (node->HasLeft()) {
        node = node->Left();
        continue;
      }

      return NULL;
    }

    return NULL;
  }

  void InorderInterate(RBNode* root, std::vector<KeyType>* keys) const {
    if (root->HasLeft())
      InorderInterate(root->Left(), keys);

    keys->push_back(root->Key());

    if (root->HasRight())
      InorderInterate(root->Right(), keys);
  }

  DISALLOW_COPY_AND_ASSIGN(RedBlackTree);
};
}
