#pragma once

namespace touch_priority {

enum class Route {
  None,
  ProbeWsl,
  Send3C,
};

// Physical 480x480 panel contract. Rectangles are half-open: [left,right) x [top,bottom).
struct Rect {
  int left;
  int top;
  int right;
  int bottom;

  constexpr bool contains(int x, int y) const {
    return x >= left && x < right && y >= top && y < bottom;
  }

  constexpr int width() const { return right - left; }
  constexpr int height() const { return bottom - top; }
};

constexpr int kScreenWidth = 480;
constexpr int kScreenHeight = 480;
constexpr int kActionTop = 370;
constexpr int kActionBottom = 452;
constexpr int kActionGap = 20;
constexpr int kScreenMargin = 20;

constexpr Rect kProbeWslButton{20, kActionTop, 230, kActionBottom};
constexpr Rect kSend3CButton{250, kActionTop, 460, kActionBottom};

constexpr int kTextSize2CharAdvance = 12;
constexpr int kProbeWslLabelMaxWidth = 10 * kTextSize2CharAdvance;
constexpr int kSend3CLabelMaxWidth = 9 * kTextSize2CharAdvance;

static_assert(kScreenWidth == 480 && kScreenHeight == 480, "Panel contract must remain 480x480");
static_assert(kProbeWslButton.left >= 0 && kProbeWslButton.right <= kScreenWidth,
              "PROBAR WSL must stay inside the panel");
static_assert(kSend3CButton.left >= 0 && kSend3CButton.right <= kScreenWidth,
              "ENVIAR 3C must stay inside the panel");
static_assert(kProbeWslButton.top >= 0 && kProbeWslButton.bottom <= kScreenHeight,
              "PROBAR WSL vertical bounds must stay inside the panel");
static_assert(kSend3CButton.top >= 0 && kSend3CButton.bottom <= kScreenHeight,
              "ENVIAR 3C vertical bounds must stay inside the panel");
static_assert(kProbeWslButton.right + kActionGap == kSend3CButton.left,
              "Action zones must have an explicit non-overlapping gap");
static_assert(kScreenHeight - kActionBottom >= kScreenMargin,
              "Keep a visible footer so the action buttons cannot be clipped");
static_assert(kProbeWslLabelMaxWidth < kProbeWslButton.width(),
              "PROBAR WSL label must fit inside its button");
static_assert(kSend3CLabelMaxWidth < kSend3CButton.width(),
              "ENVIAR 3C label must fit inside its button");

inline Route route(bool touched, int x, int y, bool editorActive) {
  if (!touched) return Route::None;
  if (editorActive) return Route::None;
  if (kProbeWslButton.contains(x, y)) return Route::ProbeWsl;
  if (kSend3CButton.contains(x, y)) return Route::Send3C;
  return Route::None;
}

}  // namespace touch_priority
