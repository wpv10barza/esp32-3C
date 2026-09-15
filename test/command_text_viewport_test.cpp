#include <cassert>
#include <cstddef>
#include <cstdint>

#include "command_text_viewport.h"

using command_text_viewport::Window;

int main() {
  // Synthetic measured widths: characters alternate between narrow and wide
  // glyphs so the algorithm cannot accidentally depend on monospace text.
  constexpr uint16_t widths[] = {0, 8, 18, 26, 36, 44, 54, 64, 72, 84, 96, 106, 118};
  constexpr int viewport = 40;
  constexpr int cursorWidth = 2;

  Window view = command_text_viewport::compute(widths, 12, 0, viewport, cursorWidth);
  assert(view.first == 0);
  assert(view.cursorX == 0);
  assert(view.textWidth <= viewport - cursorWidth);

  // Moving the cursor into the command must shift the horizontal window
  // instead of deleting the suffix just to make the text fit.
  view = command_text_viewport::compute(widths, 12, 5, viewport, cursorWidth);
  assert(view.first > 0);
  assert(view.first <= 5);
  assert(view.cursorX >= 0);
  assert(view.cursorX <= viewport - cursorWidth);
  assert(widths[view.last] - widths[view.first] <= viewport - cursorWidth);

  // Cursor at the end shows the trailing characters and keeps the insertion
  // point inside the field.
  view = command_text_viewport::compute(widths, 12, 12, viewport, cursorWidth);
  assert(view.last == 12);
  assert(view.first > 0);
  assert(view.cursorX == viewport - cursorWidth || view.cursorX < viewport - cursorWidth);
  assert(view.cursorX <= viewport - cursorWidth);

  // Touch-to-cursor mapping must use the same scrolled origin as rendering.
  const std::size_t touchedLeft = command_text_viewport::cursorForTouch(
      widths, 12, view, 100, 100);
  const std::size_t touchedRight = command_text_viewport::cursorForTouch(
      widths, 12, view, 100 + viewport - 1, 100);
  assert(touchedLeft == view.first);
  assert(touchedRight == view.last);

  // A touch in the middle of a visible glyph resolves to the nearest caret.
  view = command_text_viewport::compute(widths, 12, 8, 50, cursorWidth);
  const int contentX = 20;
  const int firstGlyphLeft = static_cast<int>(widths[view.first] - widths[view.first]);
  (void)firstGlyphLeft;
  const int glyphWidth = static_cast<int>(widths[view.first + 1] - widths[view.first]);
  if (view.first < view.last) {
    const int midpoint = contentX + glyphWidth / 2;
    assert(command_text_viewport::cursorForTouch(widths, 12, view, midpoint, contentX) == view.first + 1);
  }

  // Short text does not scroll at all.
  view = command_text_viewport::compute(widths, 4, 4, 100, cursorWidth);
  assert(view.first == 0);
  assert(view.last == 4);
  assert(view.cursorX == widths[4]);

  // Out-of-range cursor and text lengths are clamped without overrunning the
  // measured-width array.
  view = command_text_viewport::compute(widths, 999, 999, viewport, cursorWidth);
  assert(view.last <= 12);
  assert(view.first <= view.last);
  assert(view.cursorX <= viewport - cursorWidth);

  // Degenerate viewport leaves a safe empty result.
  view = command_text_viewport::compute(widths, 12, 4, 2, cursorWidth);
  assert(view.first == 0 && view.last == 0 && view.cursorX == 0);

  return 0;
}
