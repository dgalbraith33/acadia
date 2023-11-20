#pragma once

#include "glacier/container/optional.h"
#include "glacier/container/pair.h"
#include "glacier/memory/move.h"
#include "glacier/memory/ref_counted.h"
#include "glacier/memory/ref_ptr.h"
#include "glacier/memory/reference.h"
#include "glacier/memory/unique_ptr.h"
#include "glacier/string/str_format.h"

namespace glcr {

template <typename K, typename V>
class BinaryTree {
 public:
  BinaryTree() = default;
  BinaryTree(const BinaryTree&) = delete;
  // FIXME: Implement move.
  BinaryTree(BinaryTree&&) = delete;

  void Insert(K key, V&& value);
  void Delete(K key);

  Optional<Ref<V>> Predecessor(K key);
  Optional<Ref<V>> Successor(K key);

  Optional<Ref<V>> Find(K key);

  void DebugTreeIntoStr(StringBuilder& builder) const;

 private:
  // TODO: Consider adding a sharedptr type to
  // avoid making this "RefCounted".
  struct BinaryNode : public RefCounted<BinaryNode> {
    K key;
    V value;

    RefPtr<BinaryNode> left;
    RefPtr<BinaryNode> right;
    RefPtr<BinaryNode> parent;

    BinaryNode(K k, V v) : key(k), value(Move(v)) {}
  };

  RefPtr<BinaryNode> root_;

  // If this node exists, return it. Otherwise, this
  // will be the parent of where this node would be inserted.
  RefPtr<BinaryNode> FindOrInsertionParent(K key);

  static void DebugNodeIntoString(StringBuilder& builder, uint64_t indent_level,
                                  const RefPtr<BinaryNode>& node);
};

template <typename K, typename V>
void BinaryTree<K, V>::Insert(K key, V&& value) {
  auto parent = FindOrInsertionParent(key);
  if (parent.empty()) {
    root_ = AdoptPtr(new BinaryNode(key, Move(value)));
    return;
  }

  if (parent->key > key) {
    parent->left = AdoptPtr(new BinaryNode(key, Move(value)));
    parent->left->parent = parent;
  } else if (parent->key < key) {
    parent->right = AdoptPtr(new BinaryNode(key, Move(value)));
    parent->right->parent = parent;
  } else {
    parent->value = Move(value);
  }
}

template <typename K, typename V>
void BinaryTree<K, V>::Delete(K key) {
  auto node = FindOrInsertionParent(key);
  if (node.empty() || node->key != key) {
    return;
  }

  RefPtr<BinaryNode> new_child = nullptr;
  if (!node->left) {
    // No children.
    // Right child only.
    new_child = node->right;
  } else if (!node->right) {
    // Left child only.
    new_child = node->left;
  } else {
    // Find Successor.
    auto successor = node->right;
    while (successor->left) {
      successor = successor->left;
    }
    new_child = successor;
    if (successor != node->right) {
      successor->parent->left = successor->right;
    }
  }

  if (node == root_) {
    root_ = new_child;
  } else {
    if (node->parent->right == node) {
      node->parent->right = new_child;
    } else {
      node->parent->left = new_child;
    }
  }
  if (new_child) {
    new_child->parent = node->parent;
  }
}

template <typename K, typename V>
Optional<Ref<V>> BinaryTree<K, V>::Predecessor(K key) {
  auto current = FindOrInsertionParent(key);
  if (current.empty()) {
    return {};
  }

  // The case where the current is the insertion parent and
  // the predecessor is unique. If the key was going to be
  // inserted as the left child, it shares its predecessor with the parent.
  if (current->key < key) {
    return Optional<Ref<V>>(current->value);
  }

  // Case where the predecessor is below us in the tree.
  if (current->left) {
    current = current->left;
    while (current->right) {
      current = current->right;
    }
    return {current->value};
  }

  // Case where the predecessor is above us in the tree.
  auto parent = current->parent;
  while (parent && (parent->left == current)) {
    current = parent;
    parent = current->parent;
  }
  if (parent) {
    return {parent->value};
  }
  return {};
}

template <typename K, typename V>
Optional<Ref<V>> BinaryTree<K, V>::Successor(K key) {
  auto current = FindOrInsertionParent(key);
  if (current.empty()) {
    return {};
  }

  // The case where the current is the insertion parent and
  // the predecessor is unique. If the key was going to be
  // inserted as the left child, it shares its predecessor with the parent.
  if (current->key > key) {
    return Optional<Ref<V>>(current->value);
  }

  // Case where the predecessor is below us in the tree.
  if (current->right) {
    current = current->right;
    while (current->left) {
      current = current->left;
    }
    return {current->value};
  }

  // Case where the predecessor is above us in the tree.
  auto parent = current->parent;
  while (parent && (parent->right == current)) {
    current = parent;
    parent = current->parent;
  }
  if (parent) {
    return {parent->value};
  }
  return {};
}

template <typename K, typename V>
Optional<Ref<V>> BinaryTree<K, V>::Find(K key) {
  auto current = FindOrInsertionParent(key);
  if (current.empty()) {
    return {};
  }
  if (current->key == key) {
    return Optional<Ref<V>>(current->value);
  }
  return {};
}

template <typename K, typename V>
RefPtr<typename BinaryTree<K, V>::BinaryNode>
BinaryTree<K, V>::FindOrInsertionParent(K key) {
  if (root_.empty()) {
    return nullptr;
  }

  auto current = root_;
  while (true) {
    if (key == current->key) {
      return current;
    } else if (key < current->key) {
      if (!current->left) {
        return current;
      }
      current = current->left;
    } else {
      if (!current->right) {
        return current;
      }
      current = current->right;
    }
  }
}

template <typename K, typename V>
void StrFormatValue(StringBuilder& builder, const BinaryTree<K, V>& value,
                    StringView opts) {
  value.DebugTreeIntoStr(builder);
}

template <typename K, typename V>
void BinaryTree<K, V>::DebugTreeIntoStr(StringBuilder& builder) const {
  DebugNodeIntoString(builder, 0, root_);
}

template <typename K, typename V>
void BinaryTree<K, V>::DebugNodeIntoString(StringBuilder& builder,
                                           uint64_t indent_level,
                                           const RefPtr<BinaryNode>& node) {
  if (node.empty()) {
    return;
  }
  for (uint64_t i = 0; i < indent_level; i++) {
    builder.PushBack('\t');
  }
  StrFormatValue(builder, node->value, "");
  builder.PushBack('\n');
  if (node->left) {
    builder.PushBack('L');
    DebugNodeIntoString(builder, indent_level + 1, node->left);
  }
  if (node->right) {
    builder.PushBack('R');
    DebugNodeIntoString(builder, indent_level + 1, node->right);
  }
}
}  // namespace glcr
