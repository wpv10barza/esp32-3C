#pragma once

#include <cstddef>
#include <cstdint>

namespace touch {

struct KeyFrame {
  int16_t x;
  int16_t y;
  int16_t width;
  int16_t height;
};

constexpr size_t kNoKey = static_cast<size_t>(-1);

constexpr bool contains(const KeyFrame& frame, int x, int y) {
  return frame.width > 0 && frame.height > 0 &&
         x >= frame.x && x < frame.x + frame.width &&
         y >= frame.y && y < frame.y + frame.height;
}

inline size_t hitTest(const KeyFrame* frames, size_t count, int x, int y) {
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
