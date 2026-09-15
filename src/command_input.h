#pragma once

#include <Arduino.h>
#include <Arduino_GFX_Library.h>

class CommandInput {
 public:
  void begin(Arduino_GFX* display);
  void open(const String& seed);
  void close();
  bool active() const { return active_; }
  const String& text() const { return buffer_; }
  bool handleTouch(uint16_t x, uint16_t y);
  void draw();

 private:
  static constexpr size_t kMaxLength = 48;
  static constexpr int kFieldX = 20;
  static constexpr int kFieldY = 207;
  static constexpr int kFieldW = 440;
  static constexpr int kFieldH = 42;
  static constexpr int kKeyboardX = 11;
  static constexpr int kKeyboardY = 258;
  static constexpr int kKeyW = 44;
  static constexpr int kKeyH = 20;
  static constexpr int kKeyGap = 2;

  Arduino_GFX* display_ = nullptr;
  String buffer_;
  size_t cursor_ = 0;
  bool active_ = false;

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) const;
  void insert(char value);
  void backspace();
  void moveLeft();
  void moveRight();
  void moveHome();
  void moveEnd();
  void drawKey(int x, int y, int width, const char* label, uint8_t textSize = 1);
  void drawField();
  int cursorFromTouch(uint16_t x) const;
};
