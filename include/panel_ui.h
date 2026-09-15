#pragma once

namespace panel_ui {

constexpr int kScreenWidth = 480;
constexpr int kScreenHeight = 480;
constexpr int kScreenMargin = 12;

// Keep the two touch targets inside the physical 480x480 active area and
// leave a small bottom safety margin so the last row cannot be clipped.
struct Rect {
  int x;
  int y;
  int width;
  int height;

  constexpr bool contains(int px, int py) const {
    return px >= x && px < x + width && py >= y && py < y + height;
  }

  constexpr bool insideScreen() const {
    return x >= 0 && y >= 0 && width > 0 && height > 0 &&
           x + width <= kScreenWidth && y + height <= kScreenHeight;
  }
};

constexpr Rect kLeftButton{16, 352, 216, 104};
constexpr Rect kRightButton{248, 352, 216, 104};

enum class TouchAction {
  None,
  ProbarWSL,
  Enviar3C,
};

constexpr TouchAction actionForTouch(int x, int y) {
  if (kLeftButton.contains(x, y)) return TouchAction::ProbarWSL;
  if (kRightButton.contains(x, y)) return TouchAction::Enviar3C;
  return TouchAction::None;
}

}  // namespace panel_ui
