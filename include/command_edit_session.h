#pragma once

#include <cstddef>

#include "command_buffer.h"

// Transactional editing wrapper for the committed command buffer.
// The committed value is untouched until OK succeeds; CANCEL only discards the draft.
template <std::size_t Capacity>
class CommandEditSession {
 public:
  using Buffer = CommandBuffer<Capacity>;

  explicit CommandEditSession(Buffer& committed) : committed_(committed) {}

  bool begin() {
    if (editing_ || !committed_.invariantHolds()) return false;
    if (!original_.set(committed_.c_str())) return false;
    if (!draft_.set(original_.c_str())) return false;
    editing_ = true;
    return true;
  }

  bool ok() {
    if (!editing_ || !original_.invariantHolds() || !draft_.invariantHolds()) return false;
    if (!committed_.set(draft_.c_str())) return false;
    editing_ = false;
    original_.clear();
    draft_.clear();
    return true;
  }

  bool cancel() {
    if (!editing_) return false;
    if (!original_.invariantHolds() || !draft_.invariantHolds()) return false;
    editing_ = false;
    original_.clear();
    draft_.clear();
    return true;
  }

  bool editing() const { return editing_; }
  bool invariantHolds() const {
    return committed_.invariantHolds() &&
           (!editing_ || (original_.invariantHolds() && draft_.invariantHolds()));
  }

  Buffer& draft() { return draft_; }
  const Buffer& draft() const { return draft_; }

  const char* value() const { return editing_ ? draft_.c_str() : committed_.c_str(); }

 private:
  Buffer& committed_;
  Buffer original_;
  Buffer draft_;
  bool editing_ = false;
};
