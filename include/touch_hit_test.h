#pragma once

#include <stddef.h>
#include <stdint.h>

namespace touch {

// Screen-space rectangle using the half-open interval convention:
// [x, x + width) × [y, y + height).
// This makes shared edges deterministic: a point on the right/bottom edge
// belongs to the next frame, not both frames.
struct KeyFrame {
  int16_t x;
  int16_t y;
  int16_t width;
  int16_t height;
};

constexpr size_t kNoKey = static_cast<size_t>(-1);

inline bool contains(const KeyFrame& frame, int16_t x, int16_t y) {
  if (frame.width <= 0 || frame.height <= 0) return false;
  return x >= frame.x && x < frame.x + frame.width &&
         y >= frame.y && y < frame.y + frame.height;
}

// Returns the unique frame containing the touch point.
// If no frame contains the point, or more than one frame contains it,
// kNoKey is returned. The latter protects dispatch from conflicting actions
// when an accidentally overlapping layout reaches the touch layer.
inline size_t hitTest(const KeyFrame* frames, size_t count,
                      int16_t x, int16_t y) {
  if (frames == nullptr || count == 0) return kNoKey;

  size_t matched = kNoKey;
  for (size_t index = 0; index < count; ++index) {
    if (!contains(frames[index], x, y)) continue;
    if (matched != kNoKey) return kNoKey;
    matched = index;
  }
  return matched;
}

}  // namespace touch
