#pragma once

#include <cstddef>
#include <cstdint>

namespace command_field {

constexpr int kScreenWidth = 480;
constexpr int kScreenHeight = 480;

// Normal view keeps the editable field immediately above the two action
// buttons. Editing view moves it to the top so the virtual keyboard can own
// the lower portion of the 480x480 panel.
struct Rect {
  int16_t left = 0;
  int16_t top = 0;
  int16_t right = 0;
  int16_t bottom = 0;

  constexpr bool contains(int x, int y) const {
    return x >= left && x < right && y >= top && y < bottom;
  }

  constexpr int width() const { return right - left; }
  constexpr int height() const { return bottom - top; }
};

constexpr Rect kNormalBounds{18, 312, 462, 360};
constexpr Rect kEditingBounds{18, 18, 462, 72};
constexpr int kHorizontalPadding = 10;
constexpr int kVerticalPadding = 8;
constexpr int kCursorWidth = 2;

constexpr bool insideScreen(const Rect& rect) {
  return rect.left >= 0 && rect.top >= 0 && rect.right <= kScreenWidth &&
         rect.bottom <= kScreenHeight && rect.left < rect.right &&
         rect.top < rect.bottom;
}

constexpr bool insideKeyboardSafeArea(const Rect& rect, int keyboardTop) {
  return rect.bottom <= keyboardTop || rect.top >= kScreenHeight;
}

constexpr size_t clampCursor(size_t cursor, size_t textLength) {
  return cursor > textLength ? textLength : cursor;
}

static_assert(insideScreen(kNormalBounds), "normal command field must fit 480x480");
static_assert(insideScreen(kEditingBounds), "editing command field must fit 480x480");
static_assert(kNormalBounds.bottom < 370,
              "normal command field must not overlap action buttons");
static_assert(insideKeyboardSafeArea(kEditingBounds, 216),
              "editing command field must stay above the 216px keyboard");
static_assert(kHorizontalPadding * 2 + kCursorWidth < kNormalBounds.width(),
              "normal command field needs horizontal content room");

}  // namespace command_field
