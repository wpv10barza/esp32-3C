#include <cassert>
#include <cstring>

#include "command_buffer.h"

int main() {
  {
    CommandBuffer<32> buffer;
    assert(buffer.set("AC"));
    buffer.setCursor(1);
    assert(buffer.insert('B'));
    assert(std::strcmp(buffer.c_str(), "ABC") == 0);
    assert(buffer.length() == 3 && buffer.cursor() == 2);
    assert(buffer.invariantHolds());
  }

  {
    CommandBuffer<32> buffer;
    assert(buffer.set("ABEF"));
    buffer.setCursor(2);
    assert(buffer.insert("CD", 2));
    assert(std::strcmp(buffer.c_str(), "ABCDEF") == 0);
    assert(buffer.length() == 6 && buffer.cursor() == 4);
    assert(buffer.invariantHolds());
  }

  {
    CommandBuffer<32> buffer;
    assert(buffer.set("ABCD"));
    buffer.setCursor(2);
    assert(buffer.backspace());
    assert(std::strcmp(buffer.c_str(), "ACD") == 0);
    assert(buffer.cursor() == 1);
    buffer.set("ABCD");
    buffer.setCursor(1);
    assert(buffer.deleteForward());
    assert(std::strcmp(buffer.c_str(), "ACD") == 0);
    assert(buffer.cursor() == 1);
  }

  {
    CommandBuffer<32> buffer;
    assert(buffer.set("ABC"));
    buffer.moveHome();
    buffer.moveLeft();
    assert(buffer.cursor() == 0);
    buffer.moveRight();
    buffer.moveRight();
    buffer.moveRight();
    buffer.moveRight();
    assert(buffer.cursor() == 3);
    buffer.moveEnd();
    buffer.moveRight();
    assert(buffer.cursor() == 3);
    buffer.setCursor(999);
    assert(buffer.cursor() == 3);
    assert(buffer.invariantHolds());
  }

  {
    CommandBuffer<4> buffer;
    assert(buffer.set("ABCD"));
    buffer.setCursor(2);
    assert(!buffer.insert('X'));
    assert(!buffer.insert("YZ"));
    assert(std::strcmp(buffer.c_str(), "ABCD") == 0);
    assert(buffer.length() == 4 && buffer.cursor() == 2);
    assert(buffer.deleteForward());
    assert(std::strcmp(buffer.c_str(), "ABD") == 0);
    assert(buffer.invariantHolds());
  }

  {
    CommandBuffer<4> buffer;
    assert(buffer.set("ABCD"));
    assert(!buffer.set("ABCDE"));
    assert(std::strcmp(buffer.c_str(), "ABCD") == 0);
    assert(buffer.invariantHolds());
  }

  return 0;
}
