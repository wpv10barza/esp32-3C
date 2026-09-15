#pragma once

#include <cstdint>

namespace display_redraw {

struct Rect {
  int16_t x;
  int16_t y;
  int16_t width;
  int16_t height;
};

enum class UpdateKind : uint8_t {
  None,
  FullScreen,
  Region,
};

struct Plan {
  UpdateKind kind = UpdateKind::None;
  Rect region{0, 0, 0, 0};
};

// The editable command field used by the 480x480 UI. Keep this geometry in one
// place so the renderer and tests agree on exactly what may be erased while typing.
constexpr Rect kCommandField{20, 272, 440, 42};

// A mode transition must redraw the whole scene because the state background,
// iconography, status text and buttons can all change at once.
constexpr Plan modeTransitionPlan(bool modeChanged) {
  return modeChanged
    ? Plan{UpdateKind::FullScreen, Rect{0, 0, 480, 480}}
    : Plan{UpdateKind::None, Rect{0, 0, 0, 0}};
}

// Typing only invalidates the command field. The caller should include both the
// old and new cursor/text extents so stale pixels are erased before repainting.
Plan typingUpdatePlan(Rect previousBounds, Rect nextBounds);

constexpr bool valid(Rect r) {
  return r.width > 0 && r.height > 0 &&
         r.x >= 0 && r.y >= 0 &&
         static_cast<int32_t>(r.x) + r.width <= 480 &&
         static_cast<int32_t>(r.y) + r.height <= 480;
}

constexpr Rect unite(Rect a, Rect b) {
  if (!valid(a)) return b;
  if (!valid(b)) return a;
  const int16_t left = a.x < b.x ? a.x : b.x;
  const int16_t top = a.y < b.y ? a.y : b.y;
  const int16_t right =
      (a.x + a.width) > (b.x + b.width) ? (a.x + a.width) : (b.x + b.width);
  const int16_t bottom =
      (a.y + a.height) > (b.y + b.height) ? (a.y + a.height) : (b.y + b.height);
  return Rect{left, top, static_cast<int16_t>(right - left),
              static_cast<int16_t>(bottom - top)};
}

// These helpers intentionally depend only on a small fillRect-style display API.
// They can be used with Arduino_GFX on hardware without coupling the planner to
// a concrete display class, which also keeps the planner unit-testable on native.
template <typename Display>
void clearFull(Display& target, uint16_t background) {
  target.fillScreen(background);
}

template <typename Display>
void clearRegion(Display& target, Rect region, uint16_t background) {
  if (!valid(region)) return;
  target.fillRect(region.x, region.y, region.width, region.height, background);
}

}  // namespace display_redraw
