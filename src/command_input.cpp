#include "command_input.h"

namespace {
constexpr uint16_t kBorder = 0xFFFF;
constexpr uint16_t kKeyFill = 0x163248;
constexpr uint16_t kKeySpecial = 0x57420A;
constexpr char kRow1[] = "1234567890";
constexpr char kRow2[] = "qwertyuiop";
constexpr char kRow3[] = "asdfghjkl";
constexpr char kRow4[] = "zxcvbnm-._";
}

void CommandInput::begin(Arduino_GFX* display) { display_ = display; }

void CommandInput::open(const String& seed) {
  buffer_ = seed;
  if (buffer_.length() > kMaxLength) buffer_ = buffer_.substring(0, kMaxLength);
  cursor_ = buffer_.length();
  active_ = true;
  draw();
}

void CommandInput::close() { active_ = false; }

uint16_t CommandInput::color565(uint8_t red, uint8_t green, uint8_t blue) const {
  return display_ ? display_->color565(red, green, blue) : 0;
}

void CommandInput::insert(char value) {
  if (!active_ || buffer_.length() >= kMaxLength) return;
  buffer_ = buffer_.substring(0, cursor_) + String(value) + buffer_.substring(cursor_);
  ++cursor_;
}

void CommandInput::backspace() {
  if (!active_ || cursor_ == 0) return;
  buffer_.remove(cursor_ - 1, 1);
  --cursor_;
}

void CommandInput::moveLeft() { if (cursor_ > 0) --cursor_; }
void CommandInput::moveRight() { if (cursor_ < buffer_.length()) ++cursor_; }

void CommandInput::drawKey(int x, int y, int width, const char* label, uint8_t textSize) {
  if (!display_) return;
  const bool special = strcmp(label, "BK") == 0 || strcmp(label, "CLR") == 0 || strcmp(label, "SPC") == 0;
  const uint16_t fill = special ? kKeySpecial : kKeyFill;
  display_->fillRoundRect(x, y, width, kKeyH, 4, fill);
  display_->drawRoundRect(x, y, width, kKeyH, 4, color565(95, 135, 165));
  display_->setTextSize(textSize);
  display_->setTextColor(kBorder);
  int16_t x1 = 0;
  int16_t y1 = 0;
  uint16_t tw = 0;
  uint16_t th = 0;
  display_->getTextBounds(label, 0, 0, &x1, &y1, &tw, &th);
  display_->setCursor(x + (width - static_cast<int>(tw)) / 2, y + (kKeyH - static_cast<int>(th)) / 2 - 1);
  display_->print(label);
}

int CommandInput::cursorFromTouch(uint16_t x) const {
  if (x <= kFieldX + 8) return 0;
  const int relative = static_cast<int>(x) - (kFieldX + 8);
  const size_t index = static_cast<size_t>(relative / 12);
  return static_cast<int>(index > buffer_.length() ? buffer_.length() : index);
}

void CommandInput::drawField() {
  if (!display_) return;
  display_->fillRoundRect(kFieldX, kFieldY, kFieldW, kFieldH, 7, color565(10, 22, 33));
  display_->drawRoundRect(kFieldX, kFieldY, kFieldW, kFieldH, 7, color565(180, 215, 235));
  display_->setTextSize(2);
  display_->setTextColor(color565(235, 245, 250));

  constexpr size_t visibleChars = 34;
  const size_t start = buffer_.length() > visibleChars && cursor_ > visibleChars ? cursor_ - visibleChars : 0;
  const String visible = buffer_.substring(start, visibleChars);
  display_->setCursor(kFieldX + 9, kFieldY + 12);
  display_->print(visible);

  const size_t visibleCursor = cursor_ >= start ? cursor_ - start : 0;
  const int cursorX = kFieldX + 9 + static_cast<int>(visibleCursor) * 12;
  display_->drawFastVLine(cursorX, kFieldY + 8, 23, color565(170, 225, 255));
}

void CommandInput::draw() {
  if (!active_ || !display_) return;
  display_->fillRect(kFieldX - 2, kFieldY - 12, kFieldW + 4, 159, color565(10, 18, 38));
  display_->setTextSize(1);
  display_->setTextColor(color565(180, 205, 220));
  display_->setCursor(kFieldX + 8, kFieldY - 8);
  display_->print("ORDEN (48 caracteres max.)");
  drawField();

  for (int row = 0; row < 4; ++row) {
    const int y = kKeyboardY + row * (kKeyH + kKeyGap);
    const char* keys = row == 0 ? kRow1 : row == 1 ? kRow2 : row == 2 ? kRow3 : kRow4;
    for (int index = 0; index < 10; ++index) {
      char label[2] = {keys[index], '\0'};
      drawKey(kKeyboardX + index * (kKeyW + kKeyGap), y, kKeyW, label);
    }
  }

  const int specialY = kKeyboardY + 4 * (kKeyH + kKeyGap);
  drawKey(kKeyboardX, specialY, 182, "SPC");
  drawKey(kKeyboardX + 184, specialY, 90, "BK");
  drawKey(kKeyboardX + 276, specialY, 90, "CLR");
  drawKey(kKeyboardX + 368, specialY, 46, "<-");
  drawKey(kKeyboardX + 418, specialY, 46, "->");
}

bool CommandInput::handleTouch(uint16_t x, uint16_t y) {
  if (!active_) return false;
  if (y >= kFieldY && y < kFieldY + kFieldH && x >= kFieldX && x < kFieldX + kFieldW) {
    cursor_ = static_cast<size_t>(cursorFromTouch(x));
    drawField();
    return true;
  }

  const int lastKeyboardY = kKeyboardY + 5 * kKeyH + 4 * kKeyGap;
  if (y < kKeyboardY || y >= lastKeyboardY || x < kKeyboardX || x >= kKeyboardX + 10 * kKeyW + 9 * kKeyGap) return false;
  const int rowHeight = kKeyH + kKeyGap;
  const int row = (static_cast<int>(y) - kKeyboardY) / rowHeight;
  const int rowOffsetY = (static_cast<int>(y) - kKeyboardY) % rowHeight;
  if (rowOffsetY >= kKeyH) return true;

  if (row < 4) {
    const int column = (static_cast<int>(x) - kKeyboardX) / (kKeyW + kKeyGap);
    const int columnOffsetX = (static_cast<int>(x) - kKeyboardX) % (kKeyW + kKeyGap);
    if (column >= 10 || columnOffsetX >= kKeyW) return true;
    const char* keys = row == 0 ? kRow1 : row == 1 ? kRow2 : row == 2 ? kRow3 : kRow4;
    insert(keys[column]);
    draw();
    return true;
  }

  const int localX = static_cast<int>(x) - kKeyboardX;
  if (localX < 182) {
    insert(' ');
  } else if (localX < 274) {
    backspace();
  } else if (localX < 366) {
    buffer_ = "";
    cursor_ = 0;
  } else if (localX < 414) {
    moveLeft();
  } else {
    moveRight();
  }
  draw();
  return true;
}
