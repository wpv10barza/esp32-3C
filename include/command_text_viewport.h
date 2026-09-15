#pragma once

#include <cstddef>
#include <cstdint>

namespace command_text_viewport {

struct Window {
  std::size_t first = 0;
  std::size_t last = 0;  // Exclusive.
  int textWidth = 0;
  int cursorX = 0;  // Relative to the viewport's left edge.
};

// prefixWidths[i] is the measured pixel width of the first i characters.
// The returned character window is chosen so that the cursor is always visible
// and the rendered text plus cursor fits inside viewportWidth.
template <std::size_t N>
inline Window compute(const uint16_t (&prefixWidths)[N],
                      std::size_t textLength,
                      std::size_t cursor,
                      int viewportWidth,
                      int cursorWidth = 2) {
  Window result;
  if (N == 0 || viewportWidth <= cursorWidth) return result;

  const std::size_t safeLength = textLength < (N - 1) ? textLength : (N - 1);
  const std::size_t safeCursor = cursor < safeLength ? cursor : safeLength;
  const int availableForText = viewportWidth - cursorWidth;

  std::size_t first = 0;
  while (first < safeCursor &&
         static_cast<int>(prefixWidths[safeCursor] - prefixWidths[first]) > availableForText) {
    ++first;
  }

  std::size_t last = first;
  while (last < safeLength &&
         static_cast<int>(prefixWidths[last + 1] - prefixWidths[first]) <= availableForText) {
    ++last;
  }

  // A single glyph wider than the viewport still gets a one-character window;
  // the caller must clip the drawing primitive to the field rectangle.
  if (last == first && first < safeLength) ++last;

  result.first = first;
  result.last = last;
  result.textWidth = static_cast<int>(prefixWidths[last] - prefixWidths[first]);
  result.cursorX = static_cast<int>(prefixWidths[safeCursor] - prefixWidths[first]);
  return result;
}

// Map a screen-space touch coordinate to the nearest caret represented by the
// currently visible window. Coordinates outside the visible text are clamped
// to the first/last visible caret, so scrolling never causes a jump to an
// off-screen character.
template <std::size_t N>
inline std::size_t cursorForTouch(const uint16_t (&prefixWidths)[N],
                                  std::size_t textLength,
                                  const Window& view,
                                  int touchX,
                                  int contentX) {
  if (N == 0) return 0;

  const std::size_t safeLength = textLength < (N - 1) ? textLength : (N - 1);
  const std::size_t first = view.first < safeLength ? view.first : safeLength;
  const std::size_t last = view.last < safeLength ? view.last : safeLength;
  if (first > last) return first;

  const int target = touchX - contentX;
  if (target <= 0) return first;
  if (first == last) return first;

  for (std::size_t index = first; index < last; ++index) {
    const int left = static_cast<int>(prefixWidths[index] - prefixWidths[first]);
    const int right = static_cast<int>(prefixWidths[index + 1] - prefixWidths[first]);
    const int midpoint = left + (right - left) / 2;
    if (target < midpoint) return index;
  }
  return last;
}

}  // namespace command_text_viewport
