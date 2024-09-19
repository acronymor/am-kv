#pragma once

#include <cassert>

#include "util/arena.h"
#include "util/random.h"

namespace amkv::table {
const std::size_t kMaxHeight = 12;

template <typename Key, class Comparator>
class SkipList {
 public:
  class Node;
  class Iterator;

  explicit SkipList(util::Arena* arena, Comparator comparator)
      : arena_(arena), comparator_(comparator), head_(newNode(0, kMaxHeight)), max_height_(1), random_(0xdeadbeef) {
    for (std::size_t i = 0; i < kMaxHeight; i++) {
      this->head_->SetNext(i, nullptr);
    }
  }

  SkipList(const SkipList& other) = delete;
  SkipList& operator=(const SkipList& other) = delete;

  void Insert(const Key& key) {
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

  bool Contains(const Key& key) const {
    Node* x = this->findGreaterOrEqual(key, nullptr);
    return x != nullptr && this->equal(key, x->key);
  }

 private:
  int randomHeight() {
    const std::size_t branch = 4;
    std::size_t height = 1;
    while (height < kMaxHeight && this->random_.OneIn(branch)) {
      height++;
    }
    assert(height > 0);
    assert(height <= kMaxHeight);
    return height;
  }

  Node* newNode(const Key& key, std::size_t height) {
    char* const node = this->arena_->AllocateAligned(sizeof(Node) + sizeof(std::atomic<Node*>) * (height - 1));
    return new (node) Node(key);
  }

  bool equal(const Key& a, const Key& b) const { return this->comparator_(a, b); }

  Node* findGreaterOrEqual(const Key& key, Node** prev) const {
    Node* x = this->head_;
    std::size_t level = this->getMaxHeight() - 1;

    while (true) {
      Node* next = x->Next(level);
      if (next != nullptr && this->comparator_(next->key, key) < 0) {
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

  Node* findLessThan(const Key& key) const {
    Node* x = this->head_;
    std::size_t level = this->getMaxHeight() - 1;
    while (true) {
      Node* next = x->Next(level);
      if (next == nullptr || this->comparator_(next->key_, key) >= 0) {
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

  Node* findLast() const {
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

  inline std::size_t getMaxHeight() const { return this->max_height_.load(std::memory_order_relaxed); }

 private:
  util::Arena* const arena_;
  Comparator const comparator_;
  Node* const head_;

  util::Random random_;

  std::atomic<std::size_t> max_height_;
};

template <typename Key, class Comparator>
class SkipList<Key, Comparator>::Iterator {
 public:
  explicit Iterator(const SkipList* list) : list_(list), node_(nullptr) {}

  Iterator(const Iterator& other) = delete;

  bool Valid() const { return this->node_ != nullptr; }

  const Key& key() const {
    assert(this->Valid());
    return this->node_->key;
  }

  void Next() {
    assert(this->Valid());
    this->node_ = this->node_->Next(0);
  }

  void Prev() {
    assert(this->Valid());
    this->node_ = this->list_->findLessThan(this->node_->key);
    if (this->node_ == this->list_->head_) {
      this->node_ = nullptr;
    }
  }
  void Seek(const Key& target) { this->node_ = this->list_->findGreaterOrEqual(target, nullptr); }

  void SeekToFirst() { node_ = list_->head_->Next(0); }

  void SeekToLast() {
    node_ = list_->findLast();
    if (node_ == list_->head_) {
      node_ = nullptr;
    }
  }

 private:
  const SkipList* list_;
  Node* node_;
};

template <typename Key, class Comparator>
class SkipList<Key, Comparator>::Node {
 public:
  Key const key;

 public:
  explicit Node(const Key& key) : key(key) {}

  Node* Next(std::size_t n) { return this->next_[n].load(std::memory_order_acquire); }
  void SetNext(std::size_t n, Node* x) { this->next_[n].store(x, std::memory_order_release); }
  Node* NoBarrier_Next(int n) { return this->next_[n].load(std::memory_order_relaxed); }
  void NoBarrier_SetNext(int n, Node* x) { this->next_[n].store(x, std::memory_order_relaxed); }

 private:
  std::atomic<Node*> next_[1];
};
}  // namespace amkv::table