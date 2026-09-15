#pragma once

#include <cstddef>
#include <cstdint>

namespace command_text_viewport {

struct Window {
  size_t first = 0;
  size_t last = 0;  // Exclusive.
  int textWidth = 0;
  int cursorX = 0;  // Relative to the viewport's left edge.
};

// prefixWidths[i] is the measured pixel width of the first i characters.
// The returned range is the largest safe horizontal slice that keeps the cursor
// visible while reserving cursorWidth pixels for the caret. No returned text
// range is allowed to exceed the drawable text width.
template <size_t N>
inline Window compute(const uint16_t (&prefixWidths)[N],
                      size_t textLength,
                      size_t cursor,
                      int viewportWidth,
                      int cursorWidth = 2) {
  Window result;
  if (N == 0 || viewportWidth <= cursorWidth) return result;

  const size_t safeLength = textLength < (N - 1) ? textLength : (N - 1);
  const size_t safeCursor = cursor <= safeLength ? cursor : safeLength;
  const int availableForText = viewportWidth - cursorWidth;

  // Shift the left edge only as far as needed to keep the insertion point
  // inside the viewport. This makes the view stable for short commands and
  // progressively scrolls as the cursor advances through long commands.
  size_t first = 0;
  while (first < safeCursor &&
         static_cast<int>(prefixWidths[safeCursor] - prefixWidths[first]) > availableForText) {
    ++first;
  }

  // Extend to the right while the measured text remains inside the drawable
  // portion. This is deliberately strict: oversized glyphs are not returned
  // as a visible range, so the caller cannot knowingly render past the field.
  size_t last = first;
  while (last < safeLength &&
         static_cast<int>(prefixWidths[last + 1] - prefixWidths[first]) <= availableForText) {
    ++last;
  }

  result.first = first;
  result.last = last;
  result.textWidth = static_cast<int>(prefixWidths[last] - prefixWidths[first]);
  result.cursorX = static_cast<int>(prefixWidths[safeCursor] - prefixWidths[first]);
  return result;
}

}  // namespace command_text_viewport
