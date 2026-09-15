#pragma once

#include <cstddef>
#include <cstdint>

namespace command_field {

constexpr int kScreenWidth = 480;
constexpr int kScreenHeight = 480;

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
  constexpr int width() const { return right - left; }
  constexpr int height() const { return bottom - top; }
};

constexpr Rect kNormalBounds(18, 312, 462, 360);
constexpr Rect kEditingBounds(18, 18, 462, 72);
constexpr int kHorizontalPadding = 10;
constexpr int kVerticalPadding = 8;
constexpr int kCursorWidth = 2;

constexpr bool insideScreen(const Rect& rect) {
  return rect.left >= 0 && rect.top >= 0 && rect.right <= kScreenWidth &&
         rect.bottom <= kScreenHeight && rect.left < rect.right && rect.top < rect.bottom;
}

constexpr std::size_t clampCursor(std::size_t cursor, std::size_t textLength) {
  return cursor > textLength ? textLength : cursor;
}

static_assert(insideScreen(kNormalBounds), "normal command field must fit 480x480");
static_assert(insideScreen(kEditingBounds), "editing command field must fit 480x480");
static_assert(kNormalBounds.bottom < 370, "normal command field must not overlap action buttons");
static_assert(kEditingBounds.bottom <= 216, "editing command field must stay above keyboard");

}  // namespace command_field
