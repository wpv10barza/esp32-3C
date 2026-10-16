#pragma once

#include <cstddef>

#include "command_buffer.h"

// Transactional editing wrapper for the command sent by the physical UI.
// While editing, the committed CommandBuffer is never modified. OK is the
// only operation that commits the draft; CANCEL only discards the draft.
template <std::size_t Capacity>
class CommandEditSession {
 public:
  using Buffer = CommandBuffer<Capacity>;

  explicit CommandEditSession(Buffer& committed) : committed_(committed) {}

  // Enters editing mode and snapshots the currently committed command.
  // Re-entering an already active session is rejected and has no side effect.
  bool begin() {
    if (editing_) return false;
    if (!committed_.invariantHolds()) return false;
    if (!original_.set(committed_.c_str())) return false;
    if (!draft_.set(original_.c_str())) return false;
    editing_ = true;
    return true;
  }

  // Saves the current draft into the committed buffer. On any failure the
  // session remains active so the user can keep editing or cancel safely.
  bool ok() {
    if (!editing_) return false;
    if (!original_.invariantHolds() || !draft_.invariantHolds()) return false;
    if (!committed_.set(draft_.c_str())) return false;
    editing_ = false;
    original_.clear();
    draft_.clear();
    return true;
  }

  // Discards all edits. The committed buffer is intentionally untouched.
  // The draft is restored from the original snapshot before leaving editing
  // mode, making repeated state checks deterministic.
  bool cancel() {
    if (!editing_) return false;
    const bool restored = draft_.set(original_.c_str());
    if (!restored || !original_.invariantHolds() || !draft_.invariantHolds()) return false;
    editing_ = false;
    draft_.clear();
    original_.clear();
    return true;
  }

  bool editing() const { return editing_; }

  Buffer& draft() { return draft_; }
  const Buffer& draft() const { return draft_; }

  const char* value() const {
    return editing_ ? draft_.c_str() : committed_.c_str();
  }

  // Useful for transition tests and defensive UI assertions.
  bool invariantHolds() const {
    if (!committed_.invariantHolds()) return false;
    if (!editing_) return true;
    return original_.invariantHolds() && draft_.invariantHolds();
  }

 private:
  Buffer& committed_;
  Buffer original_;
  Buffer draft_;
  bool editing_ = false;
};
