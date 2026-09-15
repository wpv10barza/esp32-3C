#pragma once

#include <cstdint>

namespace touch_key_feedback {

// Hardware-independent state for touchscreen key gestures. One press is
// accepted per touch-down, with a minimum quiet interval between presses.
// Unsigned subtraction keeps millis()-style wraparound safe.
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

}  // namespace touch_key_feedback
