#pragma once

#include <cstddef>

#include "command_buffer.h"

// Transactional editing wrapper for the command used by the physical UI.
// While editing, the committed buffer is never modified. OK is the only
// commit path; CANCEL is discard-only and restores the original draft state.
template <std::size_t Capacity>
class CommandEditSession {
 public:
  using Buffer = CommandBuffer<Capacity>;

  explicit CommandEditSession(Buffer& committed) : committed_(committed) {}

  // Enter editing by snapshotting the committed command. Re-entry is rejected
  // without changing either committed data or the current draft.
  bool begin() {
    if (editing_) return false;
    if (!committed_.invariantHolds()) return false;
    if (!original_.set(committed_.c_str())) return false;
    if (!draft_.set(original_.c_str())) return false;
    editing_ = true;
    return true;
  }

  // OK: validate the transactional buffers, then atomically publish the draft
  // through CommandBuffer::set(). Any failure leaves editing mode active.
  bool ok() {
    if (!editing_) return false;
    if (!original_.invariantHolds() || !draft_.invariantHolds()) return false;
    if (!committed_.set(draft_.c_str())) return false;

    editing_ = false;
    original_.clear();
    draft_.clear();
    return true;
  }

  // CANCEL: restore the draft from the original snapshot and leave the
  // committed buffer untouched. Any failed restore keeps the session active.
  bool cancel() {
    if (!editing_) return false;

    const bool restored = draft_.set(original_.c_str());
    if (!restored || !original_.invariantHolds() || !draft_.invariantHolds()) {
      return false;
    }

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

  // Defensive assertion for UI/event-dispatch code.
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
