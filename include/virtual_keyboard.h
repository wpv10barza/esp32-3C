#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "touch_hit_test.h"

namespace virtual_keyboard {

constexpr int kScreenWidth = 480;
constexpr int kScreenHeight = 480;
constexpr int kKeyboardX = 5;
constexpr int kKeyboardY = 216;
constexpr int kKeyboardWidth = 470;
constexpr int kKeyboardHeight = 208;
constexpr int kColumns = 10;
constexpr int kRows = 4;
constexpr int kColumnGap = 4;
constexpr int kRowGap = 4;
constexpr int kKeyWidth = 43;
constexpr int kKeyHeight = 48;

static_assert(kKeyboardX + kKeyboardWidth <= kScreenWidth, "keyboard must stay inside 480px screen");
static_assert(kKeyboardY + kKeyboardHeight <= kScreenHeight, "keyboard must stay inside 480px screen");
static_assert(kRows * kKeyHeight + (kRows - 1) * kRowGap == kKeyboardHeight, "keyboard rows must fill frame");

struct KeyRect {
  int16_t left = 0;
  int16_t top = 0;
  int16_t right = 0;
  int16_t bottom = 0;

  constexpr bool contains(int x, int y) const {
    return x >= left && x < right && y >= top && y < bottom;
  }
};

constexpr bool rectanglesOverlap(const KeyRect& a, const KeyRect& b) {
  return a.left < b.right && b.left < a.right && a.top < b.bottom && b.top < a.bottom;
}

enum class KeyboardMode : uint8_t { Alpha, NumericSymbols };

enum class KeyKind : uint8_t {
  Character,
  Backspace,
  DeleteForward,
  Enter,
  Space,
  Clear,
  CursorLeft,
  CursorRight,
  ToggleAlphaNumeric,
};

struct KeyDefinition {
  KeyKind kind;
  const char* label;
  uint8_t startColumn;
  uint8_t spanColumns;
};

struct Key { KeyDefinition definition; KeyRect rect; };
struct Row { const KeyDefinition* definitions; size_t count; };

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
  return {static_cast<int16_t>(left), static_cast<int16_t>(rowOrigin(row)),
          static_cast<int16_t>(left + width), static_cast<int16_t>(rowOrigin(row) + kKeyHeight)};
}

namespace detail {
constexpr std::array<KeyDefinition, 10> kAlphaRow0 = {{
  {KeyKind::Character,"Q",0,1},{KeyKind::Character,"W",1,1},{KeyKind::Character,"E",2,1},
  {KeyKind::Character,"R",3,1},{KeyKind::Character,"T",4,1},{KeyKind::Character,"Y",5,1},
  {KeyKind::Character,"U",6,1},{KeyKind::Character,"I",7,1},{KeyKind::Character,"O",8,1},{KeyKind::Character,"P",9,1}}};
constexpr std::array<KeyDefinition, 10> kAlphaRow1 = {{
  {KeyKind::Character,"A",0,1},{KeyKind::Character,"S",1,1},{KeyKind::Character,"D",2,1},
  {KeyKind::Character,"F",3,1},{KeyKind::Character,"G",4,1},{KeyKind::Character,"H",5,1},
  {KeyKind::Character,"J",6,1},{KeyKind::Character,"K",7,1},{KeyKind::Character,"L",8,1},{KeyKind::Backspace,"BKSP",9,1}}};
constexpr std::array<KeyDefinition, 10> kAlphaRow2 = {{
  {KeyKind::Character,"Z",0,1},{KeyKind::Character,"X",1,1},{KeyKind::Character,"C",2,1},
  {KeyKind::Character,"V",3,1},{KeyKind::Character,"B",4,1},{KeyKind::Character,"N",5,1},
  {KeyKind::Character,"M",6,1},{KeyKind::Character,",",7,1},{KeyKind::Character,".",8,1},{KeyKind::Character,"/",9,1}}};
constexpr std::array<KeyDefinition, 7> kAlphaControlRow = {{
  {KeyKind::ToggleAlphaNumeric,"123",0,1},{KeyKind::Space,"SPACE",1,4},{KeyKind::Backspace,"BKSP",5,1},
  {KeyKind::Clear,"CLR",6,1},{KeyKind::CursorLeft,"<",7,1},{KeyKind::CursorRight,">",8,1},{KeyKind::Enter,"ENTER",9,1}}};

constexpr std::array<KeyDefinition, 10> kNumericRow0 = {{
  {KeyKind::Character,"1",0,1},{KeyKind::Character,"2",1,1},{KeyKind::Character,"3",2,1},{KeyKind::Character,"4",3,1},
  {KeyKind::Character,"5",4,1},{KeyKind::Character,"6",5,1},{KeyKind::Character,"7",6,1},{KeyKind::Character,"8",7,1},
  {KeyKind::Character,"9",8,1},{KeyKind::Character,"0",9,1}}};
constexpr std::array<KeyDefinition, 10> kNumericRow1 = {{
  {KeyKind::Character,"@",0,1},{KeyKind::Character,"#",1,1},{KeyKind::Character,"$",2,1},{KeyKind::Character,"%",3,1},
  {KeyKind::Character,"&",4,1},{KeyKind::Character,"*",5,1},{KeyKind::Character,"-",6,1},{KeyKind::Character,"+",7,1},
  {KeyKind::Character,"=",8,1},{KeyKind::Character,"/",9,1}}};
constexpr std::array<KeyDefinition, 10> kNumericRow2 = {{
  {KeyKind::Character,"(",0,1},{KeyKind::Character,")",1,1},{KeyKind::Character,"[",2,1},{KeyKind::Character,"]",3,1},
  {KeyKind::Character,"{",4,1},{KeyKind::Character,"}",5,1},{KeyKind::Character,":",6,1},{KeyKind::Character,";",7,1},
  {KeyKind::Character,"'",8,1},{KeyKind::Backspace,"BKSP",9,1}}};
constexpr std::array<KeyDefinition, 7> kNumericControlRow = {{
  {KeyKind::ToggleAlphaNumeric,"ABC",0,1},{KeyKind::Space,"SPACE",1,4},{KeyKind::Backspace,"BKSP",5,1},
  {KeyKind::Clear,"CLR",6,1},{KeyKind::CursorLeft,"<",7,1},{KeyKind::CursorRight,">",8,1},{KeyKind::Enter,"ENTER",9,1}}};
}  // namespace detail

inline Row rowDefinition(KeyboardMode mode, uint8_t row) {
  if (mode == KeyboardMode::Alpha) {
    switch (row) {
      case 0: return {detail::kAlphaRow0.data(), detail::kAlphaRow0.size()};
      case 1: return {detail::kAlphaRow1.data(), detail::kAlphaRow1.size()};
      case 2: return {detail::kAlphaRow2.data(), detail::kAlphaRow2.size()};
      case 3: return {detail::kAlphaControlRow.data(), detail::kAlphaControlRow.size()};
      default: return {nullptr, 0};
    }
  }
  switch (row) {
    case 0: return {detail::kNumericRow0.data(), detail::kNumericRow0.size()};
    case 1: return {detail::kNumericRow1.data(), detail::kNumericRow1.size()};
    case 2: return {detail::kNumericRow2.data(), detail::kNumericRow2.size()};
    case 3: return {detail::kNumericControlRow.data(), detail::kNumericControlRow.size()};
    default: return {nullptr, 0};
  }
}

inline size_t keyCount(KeyboardMode mode) {
  size_t total = 0;
  for (uint8_t row = 0; row < kRows; ++row) total += rowDefinition(mode, row).count;
  return total;
}

inline size_t buildKeys(KeyboardMode mode, Key* out, size_t capacity) {
  size_t written = 0;
  for (uint8_t row = 0; row < kRows; ++row) {
    const Row definitions = rowDefinition(mode, row);
    for (size_t index = 0; index < definitions.count; ++index) {
      if (written >= capacity) return written;
      const KeyDefinition& definition = definitions.definitions[index];
      out[written++] = {definition, makeRect(definition, row)};
    }
  }
  return written;
}

inline int hitTestIndex(KeyboardMode mode, int x, int y) {
  std::array<Key, 50> keys{};
  const size_t count = buildKeys(mode, keys.data(), keys.size());
  for (size_t index = 0; index < count; ++index) {
    if (keys[index].rect.contains(x, y)) return static_cast<int>(index);
  }
  return -1;
}

inline bool hitTest(KeyboardMode mode, int x, int y, Key* matched = nullptr) {
  std::array<Key, 50> keys{};
  const size_t count = buildKeys(mode, keys.data(), keys.size());
  std::array<touch::KeyFrame, 50> frames{};
  for (size_t index = 0; index < count; ++index) {
    frames[index] = {keys[index].rect.left, keys[index].rect.top,
                     static_cast<int16_t>(keys[index].rect.right - keys[index].rect.left),
                     static_cast<int16_t>(keys[index].rect.bottom - keys[index].rect.top)};
  }
  const size_t match = touch::hitTest(frames.data(), count, x, y);
  if (match == touch::kNoKey) return false;
  if (matched) *matched = keys[match];
  return true;
}

}  // namespace virtual_keyboard
