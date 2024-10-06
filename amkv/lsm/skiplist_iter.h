#pragma once

#include "comm/iterator.h"
#include "lsm/skiplist.h"

namespace amkv::lsm {
template <typename K, typename V>
class SkipListIterator : public comm::Iterator<K, V> {
 public:
  explicit SkipListIterator(const SkipList<K, V>* list);
  ~SkipListIterator() override;
  bool Valid() const override;
  void SeekToFirst() override;
  void SeekToLast() override;
  void Seek(const K& target) override;
  void Next() override;
  void Prev() override;
  K Key() const override;
  V Value() const override;
  comm::Status Status() const override;

 private:
  const SkipList<K, V>* list_;
  typename SkipList<K, V>::Node* node_;
};

template <typename K, typename V>
SkipListIterator<K, V>::SkipListIterator(const SkipList<K, V>* list) : list_(list), node_(nullptr) {}

template <typename K, typename V>
SkipListIterator<K, V>::~SkipListIterator() {}

template <typename K, typename V>
bool SkipListIterator<K, V>::Valid() const {
  return this->node_ != nullptr;
}

template <typename K, typename V>
void SkipListIterator<K, V>::SeekToFirst() {
  this->node_ = this->list_->head_->Next(0);
}

template <typename K, typename V>
void SkipListIterator<K, V>::SeekToLast() {
  this->node_ = this->list_->findLast();
  if (this->node_ == this->list_->head_) {
    this->node_ = nullptr;
  }
}

template <typename K, typename V>
void SkipListIterator<K, V>::Seek(const K& target) {
  this->node_ = this->list_->findGreaterOrEqual(target, nullptr);
}

template <typename K, typename V>
void SkipListIterator<K, V>::Next() {
  assert(this->Valid());
  this->node_ = this->node_->Next(0);
}

template <typename K, typename V>
void SkipListIterator<K, V>::Prev() {
  assert(this->Valid());
  this->node_ = this->list_->findLessThan(this->node_->key);
  if (this->node_ == this->list_->head_) {
    this->node_ = nullptr;
  }
}

template <typename K, typename V>
K SkipListIterator<K, V>::Key() const {
  assert(this->Valid());
  return this->node_->key;
}

template <typename K, typename V>
V SkipListIterator<K, V>::Value() const {
}

template <typename K, typename V>
comm::Status SkipListIterator<K, V>::Status() const {
  return comm::Status::OK();
}
};  // namespace amkv::lsm
