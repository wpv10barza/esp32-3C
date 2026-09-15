#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace virtual_keyboard {

constexpr int kScreenWidth = 480;
constexpr int kScreenHeight = 480;

// Bottom 256 px of the 480x480 display are reserved for editing controls.
constexpr int kKeyboardX = 5;
constexpr int kKeyboardY = 216;
constexpr int kKeyboardWidth = 474;
constexpr int kKeyboardHeight = 256;
constexpr int kColumns = 10;
constexpr int kRows = 5;
constexpr int kColumnGap = 4;
constexpr int kRowGap = 4;
constexpr int kKeyWidth = 43;
constexpr int kKeyHeight = 48;

static_assert(kKeyboardX + kKeyboardWidth == kScreenWidth - 1,
              "keyboard must stay inside the 480px screen");
static_assert(kKeyboardY + kKeyboardHeight <= kScreenHeight,
              "keyboard must stay inside the 480px screen");

// Rectangles are half-open: [left, right) x [top, bottom). This makes
// boundary hit-testing deterministic and prevents adjacent keys from both
// claiming the same touch point.
struct KeyRect {
  int16_t left = 0;
  int16_t top = 0;
  int16_t right = 0;
  int16_t bottom = 0;

  constexpr bool contains(int x, int y) const {
    return x >= left && x < right && y >= top && y < bottom;
  }
};

enum class KeyboardMode : uint8_t {
  Alpha,
  NumericSymbols,
};

enum class KeyKind : uint8_t {
  Character,
  Backspace,
  Enter,
  Space,
  ToggleAlphaNumeric,
};

struct KeyDefinition {
  KeyKind kind;
  const char* label;
  uint8_t startColumn;
  uint8_t spanColumns;
};

struct Key {
  KeyDefinition definition;
  KeyRect rect;
};

struct Row {
  const KeyDefinition* definitions;
  size_t count;
};

constexpr int columnOrigin(uint8_t column) {
  return kKeyboardX + static_cast<int>(column) * (kKeyWidth + kColumnGap);
}

constexpr int rowOrigin(uint8_t row) {
  return kKeyboardY + static_cast<int>(row) * (kKeyHeight + kRowGap);
}

constexpr KeyRect makeRect(const KeyDefinition& definition, uint8_t row) {
  const int left = columnOrigin(definition.startColumn);
  const int width = static_cast<int>(definition.spanColumns) * kKeyWidth +
                    static_cast<int>(definition.spanColumns - 1) * kColumnGap;
  return KeyRect{
      static_cast<int16_t>(left),
      static_cast<int16_t>(rowOrigin(row)),
      static_cast<int16_t>(left + width),
      static_cast<int16_t>(rowOrigin(row) + kKeyHeight),
  };
}

namespace detail {

constexpr std::array<KeyDefinition, 10> kAlphaRow0 = {{
    {KeyKind::Character, "Q", 0, 1},
    {KeyKind::Character, "W", 1, 1},
    {KeyKind::Character, "E", 2, 1},
    {KeyKind::Character, "R", 3, 1},
    {KeyKind::Character, "T", 4, 1},
    {KeyKind::Character, "Y", 5, 1},
    {KeyKind::Character, "U", 6, 1},
    {KeyKind::Character, "I", 7, 1},
    {KeyKind::Character, "O", 8, 1},
    {KeyKind::Character, "P", 9, 1},
}};

constexpr std::array<KeyDefinition, 10> kAlphaRow1 = {{
    {KeyKind::Character, "A", 0, 1},
    {KeyKind::Character, "S", 1, 1},
    {KeyKind::Character, "D", 2, 1},
    {KeyKind::Character, "F", 3, 1},
    {KeyKind::Character, "G", 4, 1},
    {KeyKind::Character, "H", 5, 1},
    {KeyKind::Character, "J", 6, 1},
    {KeyKind::Character, "K", 7, 1},
    {KeyKind::Character, "L", 8, 1},
    {KeyKind::Backspace, "BKSP", 9, 1},
}};

constexpr std::array<KeyDefinition, 10> kAlphaRow2 = {{
    {KeyKind::Character, "Z", 0, 1},
    {KeyKind::Character, "X", 1, 1},
    {KeyKind::Character, "C", 2, 1},
    {KeyKind::Character, "V", 3, 1},
    {KeyKind::Character, "B", 4, 1},
    {KeyKind::Character, "N", 5, 1},
    {KeyKind::Character, "M", 6, 1},
    {KeyKind::Character, ",", 7, 1},
    {KeyKind::Character, ".", 8, 1},
    {KeyKind::Character, "/", 9, 1},
}};

// Final alpha row provides common command/shell symbols before the controls.
constexpr std::array<KeyDefinition, 10> kAlphaRow3 = {{
    {KeyKind::Character, "-", 0, 1},
    {KeyKind::Character, "_", 1, 1},
    {KeyKind::Character, ":", 2, 1},
    {KeyKind::Character, ";", 3, 1},
    {KeyKind::Character, "@", 4, 1},
    {KeyKind::Character, "#", 5, 1},
    {KeyKind::Character, "=", 6, 1},
    {KeyKind::Character, "+", 7, 1},
    {KeyKind::Backspace, "BKSP", 8, 1},
    {KeyKind::ToggleAlphaNumeric, "123", 9, 1},
}};

constexpr std::array<KeyDefinition, 10> kNumericRow0 = {{
    {KeyKind::Character, "1", 0, 1},
    {KeyKind::Character, "2", 1, 1},
    {KeyKind::Character, "3", 2, 1},
    {KeyKind::Character, "4", 3, 1},
    {KeyKind::Character, "5", 4, 1},
    {KeyKind::Character, "6", 5, 1},
    {KeyKind::Character, "7", 6, 1},
    {KeyKind::Character, "8", 7, 1},
    {KeyKind::Character, "9", 8, 1},
    {KeyKind::Character, "0", 9, 1},
}};

constexpr std::array<KeyDefinition, 10> kNumericRow1 = {{
    {KeyKind::Character, "@", 0, 1},
    {KeyKind::Character, "#", 1, 1},
    {KeyKind::Character, "$", 2, 1},
    {KeyKind::Character, "%", 3, 1},
    {KeyKind::Character, "&", 4, 1},
    {KeyKind::Character, "*", 5, 1},
    {KeyKind::Character, "-", 6, 1},
    {KeyKind::Character, "+", 7, 1},
    {KeyKind::Character, "=", 8, 1},
    {KeyKind::Character, "/", 9, 1},
}};

constexpr std::array<KeyDefinition, 10> kNumericRow2 = {{
    {KeyKind::Character, "(", 0, 1},
    {KeyKind::Character, ")", 1, 1},
    {KeyKind::Character, "[", 2, 1},
    {KeyKind::Character, "]", 3, 1},
    {KeyKind::Character, "{", 4, 1},
    {KeyKind::Character, "}", 5, 1},
    {KeyKind::Character, ":", 6, 1},
    {KeyKind::Character, ";", 7, 1},
    {KeyKind::Character, "'", 8, 1},
    {KeyKind::Character, "\"", 9, 1},
}};

constexpr std::array<KeyDefinition, 10> kNumericRow3 = {{
    {KeyKind::Character, "<", 0, 1},
    {KeyKind::Character, ">", 1, 1},
    {KeyKind::Character, "_", 2, 1},
    {KeyKind::Character, "^", 3, 1},
    {KeyKind::Character, "|", 4, 1},
    {KeyKind::Character, "~", 5, 1},
    {KeyKind::Character, "`", 6, 1},
    {KeyKind::Character, "\\", 7, 1},
    {KeyKind::Character, ".", 8, 1},
    {KeyKind::Backspace, "BKSP", 9, 1},
}};

constexpr std::array<KeyDefinition, 5> kAlphaControlRow = {{
    {KeyKind::ToggleAlphaNumeric, "123", 0, 2},
    {KeyKind::Space, "SPACE", 2, 5},
    {KeyKind::Enter, "ENTER", 7, 3},
    {KeyKind::ToggleAlphaNumeric, "", 0, 0},
    {KeyKind::ToggleAlphaNumeric, "", 0, 0},
}};

constexpr std::array<KeyDefinition, 5> kNumericControlRow = {{
    {KeyKind::ToggleAlphaNumeric, "ABC", 0, 2},
    {KeyKind::Space, "SPACE", 2, 5},
    {KeyKind::Enter, "ENTER", 7, 3},
    {KeyKind::ToggleAlphaNumeric, "", 0, 0},
    {KeyKind::ToggleAlphaNumeric, "", 0, 0},
}};

}  // namespace detail

inline Row rowDefinition(KeyboardMode mode, uint8_t row) {
  if (mode == KeyboardMode::Alpha) {
    switch (row) {
      case 0: return {detail::kAlphaRow0.data(), detail::kAlphaRow0.size()};
      case 1: return {detail::kAlphaRow1.data(), detail::kAlphaRow1.size()};
      case 2: return {detail::kAlphaRow2.data(), detail::kAlphaRow2.size()};
      case 3: return {detail::kAlphaRow3.data(), detail::kAlphaRow3.size()};
      case 4: return {detail::kAlphaControlRow.data(), 3};
      default: return {nullptr, 0};
    }
  }

  switch (row) {
    case 0: return {detail::kNumericRow0.data(), detail::kNumericRow0.size()};
    case 1: return {detail::kNumericRow1.data(), detail::kNumericRow1.size()};
    case 2: return {detail::kNumericRow2.data(), detail::kNumericRow2.size()};
    case 3: return {detail::kNumericRow3.data(), detail::kNumericRow3.size()};
    case 4: return {detail::kNumericControlRow.data(), 3};
    default: return {nullptr, 0};
  }
}

inline size_t keyCount(KeyboardMode mode) {
  size_t total = 0;
  for (uint8_t row = 0; row < kRows; ++row) {
    total += rowDefinition(mode, row).count;
  }
  return total;
}

inline size_t buildKeys(KeyboardMode mode, Key* out, size_t capacity) {
  size_t written = 0;
  for (uint8_t row = 0; row < kRows; ++row) {
    const Row definitions = rowDefinition(mode, row);
    for (size_t index = 0; index < definitions.count; ++index) {
      if (written >= capacity) return written;
      const KeyDefinition& definition = definitions.definitions[index];
      out[written++] = Key{definition, makeRect(definition, row)};
    }
  }
  return written;
}

inline bool hitTest(KeyboardMode mode, int x, int y, Key* matched = nullptr) {
  std::array<Key, 50> keys{};
  const size_t count = buildKeys(mode, keys.data(), keys.size());
  for (size_t index = 0; index < count; ++index) {
    if (!keys[index].rect.contains(x, y)) continue;
    if (matched) *matched = keys[index];
    return true;
  }
  return false;
}

}  // namespace virtual_keyboard
