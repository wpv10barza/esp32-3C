#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "command_buffer.h"
#include "command_field.h"
#include "command_text_viewport.h"
#include "editing_command_state.h"
#include "touch_hit_test.h"
#include "touch_priority_dispatch.h"
#include "virtual_keyboard.h"

int main() {
  using Buffer = CommandBuffer<48>;
  Buffer committed;
  assert(committed.set("Cambia la tarea J10 a mensual"));

  // ENVIAR 3C enters EditingCommand rather than sending immediately.
  EditingCommandState<48> editor(committed);
  assert(!editor.isEditing());
  assert(editor.begin());
  assert(editor.mode() == EditingCommandMode::EditingCommand);
  assert(std::strcmp(committed.c_str(), "Cambia la tarea J10 a mensual") == 0);

  // Editor has exclusive priority, including over the normal button zones.
  assert(touch_priority::route(true, 30, 390, true) == touch_priority::Route::VirtualEditor);
  assert(touch_priority::route(true, 300, 390, true) == touch_priority::Route::VirtualEditor);
  assert(touch_priority::route(true, 20, 370, false) == touch_priority::Route::ProbeWsl);
  assert(touch_priority::route(true, 250, 370, false) == touch_priority::Route::Send3C);
  assert(touch_priority::route(true, 230, 400, false) == touch_priority::Route::None);

  // Physical 480x480 field geometry is inside the panel and separated from controls.
  assert(command_field::insideScreen(command_field::kNormalBounds));
  assert(command_field::insideScreen(command_field::kEditingBounds));
  assert(command_field::kNormalBounds.bottom < touch_priority::kProbeWslButton.top);
  assert(command_field::kEditingBounds.bottom <= virtual_keyboard::kKeyboardY);
  assert(command_field::kEditingBounds.contains(command_field::kEditingBounds.left,
                                                command_field::kEditingBounds.top));
  assert(!command_field::kEditingBounds.contains(command_field::kEditingBounds.right,
                                                 command_field::kEditingBounds.top));

  // Keyboard hit-testing finds a unique key at exact boundaries.
  virtual_keyboard::Key q{};
  assert(virtual_keyboard::hitTest(virtual_keyboard::KeyboardMode::Alpha,
                                    virtual_keyboard::columnOrigin(0) + 1,
                                    virtual_keyboard::rowOrigin(0) + 1, &q));
  assert(q.definition.kind == virtual_keyboard::KeyKind::Character);
  assert(q.definition.label[0] == 'Q');
  assert(!virtual_keyboard::hitTest(virtual_keyboard::KeyboardMode::Alpha,
                                    virtual_keyboard::kScreenWidth, 1));

  // Simulate arbitrary-position editing: append, move left, insert, backspace.
  assert(editor.draft().insert('X'));
  editor.draft().moveLeft();
  const std::size_t before = editor.draft().length();
  assert(editor.draft().insert('Y'));
  assert(editor.draft().length() == before + 1);
  assert(editor.draft().backspace());
  assert(editor.draft().length() == before);
  assert(editor.invariantHolds());

  // Horizontal viewport keeps the cursor visible for long commands.
  uint16_t widths[49] = {};
  for (std::size_t i = 0; i < 48; ++i) widths[i + 1] = static_cast<uint16_t>(widths[i] + 9);
  auto window = command_text_viewport::compute(widths, 48, 48, 100, 2);
  assert(window.last == 48);
  assert(window.first > 0);
  assert(window.cursorX <= 98);

  // Touch-to-caret mapping must use the active scrolled window, not the full text origin.
  assert(command_text_viewport::cursorForTouch(widths, 48, window, 0, 0) == window.first);
  assert(command_text_viewport::cursorForTouch(widths, 48, window, window.cursorX - 1, 0) >= window.first);
  assert(command_text_viewport::cursorForTouch(widths, 48, window, 100, 0) == window.last);

  // CANCEL never changes the committed value.
  const char* committedBeforeCancel = committed.c_str();
  assert(editor.cancel());
  assert(std::strcmp(committed.c_str(), committedBeforeCancel) == 0);
  assert(!editor.isEditing());

  // Re-enter, make a real edit, and OK is the only commit path.
  assert(editor.begin());
  editor.draft().moveEnd();
  assert(editor.draft().insert('!'));
  assert(editor.ok());
  assert(!editor.isEditing());
  assert(std::strstr(committed.c_str(), "!") != nullptr);
  assert(!editor.ok());
  assert(!editor.cancel());
  assert(editor.invariantHolds());

  // Shared hit-test helper fails closed on overlap.
  const touch::KeyFrame overlap[] = {{0, 0, 20, 20}, {10, 0, 20, 20}};
  assert(touch::hitTest(overlap, 2, 15, 10) == touch::kNoKey);
  return 0;
}
