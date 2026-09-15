#pragma once

#include <cstdint>

namespace touch_priority {

enum class Route : uint8_t { None, VirtualEditor, ProbeWsl, Send3C };

struct Rect {
  int16_t left = 0;
  int16_t top = 0;
  int16_t right = 0;
  int16_t bottom = 0;
  constexpr bool contains(int x, int y) const {
    return x >= left && x < right && y >= top && y < bottom;
  }
};

constexpr Rect kProbeWslButton{20, 370, 230, 452};
constexpr Rect kSend3CButton{250, 370, 460, 452};

constexpr Route route(bool touched, int x, int y, bool editorActive) {
  if (!touched) return Route::None;
  if (editorActive) return Route::VirtualEditor;
  if (kProbeWslButton.contains(x, y)) return Route::ProbeWsl;
  if (kSend3CButton.contains(x, y)) return Route::Send3C;
  return Route::None;
}

}  // namespace touch_priority
