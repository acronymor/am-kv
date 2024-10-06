#pragma once

#include <comm/iterator.h>

#include <cassert>

#include "lsm/comparator.h"
#include "util/arena.h"
#include "util/random.h"

namespace amkv::lsm {
const std::size_t kMaxHeight = 12;

template <typename K, typename V>
class SkipList {
 public:
  template <typename, typename>
  friend class SkipListIterator;

  class Node;

  SkipList(util::Arena* arena, const Comparator* comparator);

  SkipList(const SkipList& other) = delete;
  SkipList& operator=(const SkipList& other) = delete;

  void Insert(const K& key);

  bool Contains(const K& key) const;

 private:
  int randomHeight();

  Node* newNode(const K& key, std::size_t height);

  bool equal(const K& a, const K& b) const;

  Node* findGreaterOrEqual(const K& key, Node** prev) const;

  Node* findLessThan(const K& key) const;

  Node* findLast() const;

  std::size_t getMaxHeight() const;

 private:
  util::Arena* const arena_;
  const lsm::Comparator* comparator_;

  Node* head_;
  util::Random random_;

  std::atomic<std::size_t> max_height_;
};

template <typename K, typename V>
SkipList<K, V>::SkipList(util::Arena* arena, const Comparator* comparator)
    : arena_(arena), comparator_(comparator), head_(newNode("", kMaxHeight)), max_height_(1), random_(0xdeadbeef) {
  for (std::size_t i = 0; i < kMaxHeight; i++) {
    this->head_->SetNext(i, nullptr);
  }
}

template <typename K, typename V>
void SkipList<K, V>::Insert(const K& key) {
  Node* prev[kMaxHeight];
  Node* x = this->findGreaterOrEqual(key, prev);
  assert(x == nullptr || !this->equal(key, x->key));

  int height = this->randomHeight();
  if (height > this->getMaxHeight()) {
    for (std::size_t i = this->getMaxHeight(); i < height; i++) {
      prev[i] = this->head_;
    }
    this->max_height_.store(height, std::memory_order_relaxed);
  }

  x = this->newNode(key, height);
  for (std::size_t i = 0; i < height; i++) {
    x->NoBarrier_SetNext(i, prev[i]->NoBarrier_Next(i));
    prev[i]->SetNext(i, x);
  }
}

template <typename K, typename V>
bool SkipList<K, V>::Contains(const K& key) const {
  Node* x = this->findGreaterOrEqual(key, nullptr);
  return x != nullptr && this->equal(key, x->key);
}

template <typename K, typename V>
int SkipList<K, V>::randomHeight() {
  const std::size_t branch = 4;
  std::size_t height = 1;
  while (height < kMaxHeight && this->random_.OneIn(branch)) {
    height++;
  }
  assert(height > 0);
  assert(height <= kMaxHeight);
  return height;
}

template <typename K, typename V>
SkipList<K, V>::Node* SkipList<K, V>::newNode(const K& key, std::size_t height) {
  char* const node = this->arena_->AllocateAligned(sizeof(Node) + sizeof(std::atomic<Node*>) * (height - 1));
  return new (node) Node(key);
}

template <typename K, typename V>
bool SkipList<K, V>::equal(const K& a, const K& b) const {
  return this->comparator_->Compare(a, b) == 0;
}

template <typename K, typename V>
SkipList<K, V>::Node* SkipList<K, V>::findGreaterOrEqual(const K& key, Node** prev) const {
  Node* x = this->head_;
  std::size_t level = this->getMaxHeight() - 1;

  while (true) {
    Node* next = x->Next(level);
    if (next != nullptr && this->comparator_->Compare(next->key, key) < 0) {
      x = next;
    } else {
      if (prev != nullptr) {
        prev[level] = x;
      }

      if (level == 0) {
        return next;
      } else {
        level--;
      }
    }
  }
}

template <typename K, typename V>
SkipList<K, V>::Node* SkipList<K, V>::findLessThan(const K& key) const {
  Node* x = this->head_;
  std::size_t level = this->getMaxHeight() - 1;
  while (true) {
    Node* next = x->Next(level);
    if (next == nullptr || this->comparator_->Compare(next->key, key) >= 0) {
      if (level == 0) {
        return x;
      } else {
        level--;
      }
    } else {
      x = next;
    }
  }
}

template <typename K, typename V>
SkipList<K, V>::Node* SkipList<K, V>::findLast() const {
  Node* x = this->head_;
  std::size_t level = this->getMaxHeight() - 1;
  while (true) {
    Node* next = x->Next(level);
    if (next == nullptr) {
      if (level == 0) {
        return x;
      } else {
        level--;
      }
    } else {
      x = next;
    }
  }
}

template <typename K, typename V>
std::size_t SkipList<K, V>::getMaxHeight() const {
  return this->max_height_.load(std::memory_order_relaxed);
}

template <typename K, typename V>
class SkipList<K, V>::Node {
 public:
  K const key;

 public:
  explicit Node(const K& key) : key(key) {}

  Node* Next(std::size_t n) { return this->next_[n].load(std::memory_order_acquire); }
  void SetNext(std::size_t n, Node* x) { this->next_[n].store(x, std::memory_order_release); }
  Node* NoBarrier_Next(int n) { return this->next_[n].load(std::memory_order_relaxed); }
  void NoBarrier_SetNext(int n, Node* x) { this->next_[n].store(x, std::memory_order_relaxed); }

 private:
  std::atomic<Node*> next_[1];
};
}  // namespace amkv::lsm