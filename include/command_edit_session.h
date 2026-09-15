#pragma once

#include <cstddef>

#include "command_buffer.h"
#include "command_validation.h"

enum class CommandEditError : unsigned char {
  None,
  EmptyCommand,
};

template <std::size_t Capacity>
class CommandEditSession {
 public:
  using Buffer = CommandBuffer<Capacity>;

  explicit CommandEditSession(Buffer& committed) : committed_(committed) {}

  bool begin() {
    lastError_ = CommandEditError::None;
    if (editing_ || !committed_.invariantHolds()) return false;
    if (!original_.set(committed_.c_str())) return false;
    if (!draft_.set(original_.c_str())) return false;
    editing_ = true;
    return true;
  }

  bool ok() {
    lastError_ = CommandEditError::None;
    if (!editing_ || !original_.invariantHolds() || !draft_.invariantHolds()) return false;
    if (!command_validation::isNonEmpty(draft_.c_str())) {
      lastError_ = CommandEditError::EmptyCommand;
      return false;
    }
    if (!committed_.set(draft_.c_str())) return false;
    editing_ = false;
    original_.clear();
    draft_.clear();
    return true;
  }

  bool cancel() {
    lastError_ = CommandEditError::None;
    if (!editing_ || !original_.invariantHolds() || !draft_.invariantHolds()) return false;
    editing_ = false;
    original_.clear();
    draft_.clear();
    return true;
  }

  bool editing() const { return editing_; }
  CommandEditError lastError() const { return lastError_; }
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
  CommandEditError lastError_ = CommandEditError::None;
};
