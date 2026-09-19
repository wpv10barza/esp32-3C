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

inline Rect clampToCommandField(Rect region) {
  const int16_t left = region.x > kCommandField.x ? region.x : kCommandField.x;
  const int16_t top = region.y > kCommandField.y ? region.y : kCommandField.y;
  const int16_t rightA = region.x + region.width;
  const int16_t rightB = kCommandField.x + kCommandField.width;
  const int16_t bottomA = region.y + region.height;
  const int16_t bottomB = kCommandField.y + kCommandField.height;
  const int16_t right = rightA < rightB ? rightA : rightB;
  const int16_t bottom = bottomA < bottomB ? bottomA : bottomB;
  if (right <= left || bottom <= top) return Rect{0, 0, 0, 0};
  return Rect{left, top, static_cast<int16_t>(right - left),
              static_cast<int16_t>(bottom - top)};
}

inline Plan typingUpdatePlan(Rect previousBounds, Rect nextBounds) {
  // Restrict invalidation to the editor. A malformed caller rectangle cannot
  // accidentally erase the keyboard, status area, or buttons.
  const Rect previous = clampToCommandField(previousBounds);
  const Rect next = clampToCommandField(nextBounds);
  const Rect dirty = unite(previous, next);
  if (!valid(dirty)) return Plan{UpdateKind::Region, kCommandField};
  return Plan{UpdateKind::Region, dirty};
}

// These helpers intentionally depend only on a small fillRect-style display API.
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
