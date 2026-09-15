#pragma once

#include <stddef.h>
#include <string.h>

namespace input {

// Fixed-capacity editable text buffer for the ESP32 touchscreen keyboard.
// One byte is reserved for the terminating '\0'.
template <size_t Capacity>
class CommandBuffer {
 public:
  static_assert(Capacity >= 2, "CommandBuffer capacity must allow one character and NUL");

  constexpr CommandBuffer() : data_{0}, length_(0), cursor_(0) {}

  size_t capacity() const { return Capacity - 1; }
  size_t length() const { return length_; }
  size_t cursor() const { return cursor_; }
  bool empty() const { return length_ == 0; }
  bool full() const { return length_ >= capacity(); }

  const char* c_str() const { return data_; }
  char at(size_t index) const { return index < length_ ? data_[index] : '\0'; }

  void clear() {
    length_ = 0;
    cursor_ = 0;
    data_[0] = '\0';
  }

  // Replaces the complete buffer. Input is truncated safely at capacity().
  size_t assign(const char* text) {
    clear();
    if (text == nullptr) return 0;
    while (text[length_] != '\0' && length_ < capacity()) {
      data_[length_] = text[length_];
      ++length_;
    }
    data_[length_] = '\0';
    cursor_ = length_;
    return length_;
  }

  bool moveCursorLeft() {
    if (cursor_ == 0) return false;
    --cursor_;
    return true;
  }

  bool moveCursorRight() {
    if (cursor_ >= length_) return false;
    ++cursor_;
    return true;
  }

  bool moveCursorTo(size_t position) {
    if (position > length_) return false;
    cursor_ = position;
    return true;
  }

  // Inserts one character at the cursor and shifts the suffix right.
  bool insert(char value) {
    if (value == '\0' || full()) return false;
    for (size_t index = length_; index > cursor_; --index) {
      data_[index] = data_[index - 1];
    }
    data_[cursor_] = value;
    ++length_;
    ++cursor_;
    data_[length_] = '\0';
    return true;
  }

  // Removes the character immediately before the cursor.
  bool backspace() {
    if (cursor_ == 0 || length_ == 0) return false;
    const size_t eraseIndex = cursor_ - 1;
    for (size_t index = eraseIndex; index + 1 < length_; ++index) {
      data_[index] = data_[index + 1];
    }
    --length_;
    --cursor_;
    data_[length_] = '\0';
    return true;
  }

  // Removes the character at the cursor without moving the cursor.
  bool erase() {
    if (cursor_ >= length_) return false;
    for (size_t index = cursor_; index + 1 < length_; ++index) {
      data_[index] = data_[index + 1];
    }
    --length_;
    data_[length_] = '\0';
    return true;
  }

 private:
  char data_[Capacity];
  size_t length_;
  size_t cursor_;
};

}  // namespace input
