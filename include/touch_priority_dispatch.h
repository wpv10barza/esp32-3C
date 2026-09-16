#pragma once

#include <cstdint>

namespace touch_priority {

enum class Route : uint8_t { None, VirtualEditor, ProbeWsl, Send3C };

struct Rect {
  int16_t left;
  int16_t top;
  int16_t right;
  int16_t bottom;

  constexpr Rect(int16_t leftValue, int16_t topValue, int16_t rightValue, int16_t bottomValue)
      : left(leftValue), top(topValue), right(rightValue), bottom(bottomValue) {}

  constexpr bool contains(int x, int y) const {
    return x >= left && x < right && y >= top && y < bottom;
  }
};

constexpr Rect kProbeWslButton(20, 370, 230, 452);
constexpr Rect kSend3CButton(250, 370, 460, 452);

inline Route route(bool touched, int x, int y, bool editorActive) {
  if (!touched) return Route::None;
  if (editorActive) return Route::VirtualEditor;
  if (kProbeWslButton.contains(x, y)) return Route::ProbeWsl;
  if (kSend3CButton.contains(x, y)) return Route::Send3C;
  return Route::None;
}

}  // namespace touch_priority
