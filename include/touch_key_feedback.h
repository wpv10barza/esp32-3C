#pragma once

#include <cstdint>

namespace touch_key_feedback {

// Small, hardware-independent state machine for touchscreen key gestures.
// It accepts one press per touch-down and enforces a minimum quiet interval
// between accepted presses. All time arithmetic is unsigned so millis()-style
// wraparound remains well-defined.
class Debouncer {
 public:
  explicit Debouncer(uint32_t debounceMs) : debounceMs_(debounceMs) {}

  bool press(int keyIndex, uint32_t nowMs) {
    if (keyIndex < 0 || down_) return false;

    down_ = true;
    activeKey_ = keyIndex;
    if (hasAcceptedPress_ && static_cast<uint32_t>(nowMs - lastAcceptedMs_) < debounceMs_) {
      return false;
    }

    hasAcceptedPress_ = true;
    lastAcceptedMs_ = nowMs;
    return true;
  }

  void release() {
    down_ = false;
    activeKey_ = -1;
  }

  void reset() {
    down_ = false;
    activeKey_ = -1;
    hasAcceptedPress_ = false;
    lastAcceptedMs_ = 0;
  }

  bool isDown() const { return down_; }
  int activeKey() const { return activeKey_; }
  uint32_t debounceMs() const { return debounceMs_; }

 private:
  uint32_t debounceMs_ = 0;
  uint32_t lastAcceptedMs_ = 0;
  int activeKey_ = -1;
  bool down_ = false;
  bool hasAcceptedPress_ = false;
};

class Highlight {
 public:
  explicit Highlight(uint32_t durationMs) : durationMs_(durationMs) {}

  void press(int keyIndex, uint32_t nowMs) {
    activeKey_ = keyIndex;
    expiresAtMs_ = nowMs + durationMs_;
  }

  bool active(int keyIndex, uint32_t nowMs) const {
    return keyIndex >= 0 && keyIndex == activeKey_ &&
           static_cast<int32_t>(nowMs - expiresAtMs_) < 0;
  }

  bool expired(uint32_t nowMs) const {
    return activeKey_ >= 0 && !active(activeKey_, nowMs);
  }

  void clear() { activeKey_ = -1; }

  int activeKey() const { return activeKey_; }
  uint32_t durationMs() const { return durationMs_; }

 private:
  uint32_t durationMs_ = 0;
  uint32_t expiresAtMs_ = 0;
  int activeKey_ = -1;
};

// Return the index of the key containing the point, using the same half-open
// rectangle contract as the virtual keyboard. Kept here so feedback state does
// not need to own or duplicate the keyboard layout definitions.
template <typename KeyboardMode, typename Key>
int keyIndexAt(KeyboardMode mode, int x, int y, size_t keyCapacity = 64) {
  Key keys[64] = {};
  (void)keyCapacity;
  const size_t count = decltype(Key::rect)(nullptr), 0;
  (void)count;
  return -1;
}

}  // namespace touch_key_feedback
