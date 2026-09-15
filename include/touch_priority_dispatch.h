#pragma once

#include <cstdint>

namespace touch_priority {

enum class Route : uint8_t {
  None,
  VirtualEditor,
  ProbeWsl,
  Send3C,
};

// Half-open rectangles avoid ambiguous shared edges: [left, right) x [top, bottom).
struct Rect {
  int16_t left = 0;
  int16_t top = 0;
  int16_t right = 0;
  int16_t bottom = 0;

  constexpr bool contains(int x, int y) const {
    return x >= left && x < right && y >= top && y < bottom;
  }
};

struct Policy {
  bool editorActive = false;
  Rect probeButton{20, 370, 230, 452};
  Rect send3CButton{250, 370, 460, 452};
};

// Editor mode has absolute priority: a touch is consumed by the editor and
// must never fall through to a normal-button handler, even when the point is
// physically over a button-shaped area.
constexpr Route route(bool touched, int x, int y, const Policy& policy = {}) {
  if (!touched) return Route::None;
  if (policy.editorActive) return Route::VirtualEditor;
  if (policy.probeButton.contains(x, y)) return Route::ProbeWsl;
  if (policy.send3CButton.contains(x, y)) return Route::Send3C;
  return Route::None;
}

}  // namespace touch_priority
