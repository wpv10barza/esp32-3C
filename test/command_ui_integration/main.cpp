#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "command_buffer.h"
#include "command_text_viewport.h"
#include "command_validation.h"
#include "editing_command_state.h"
#include "touch_hit_test.h"
#include "touch_priority_dispatch.h"
#include "virtual_keyboard.h"

int main() {
  using Buffer = CommandBuffer<48>;
  Buffer committed;
  assert(committed.set("Cambia la tarea J10 a mensual"));

  // Execution validation must fail closed for null, empty, and whitespace-only input.
  assert(!command_validation::hasNonWhitespaceContent(nullptr));
  assert(!command_validation::hasNonWhitespaceContent(""));
  assert(!command_validation::hasNonWhitespaceContent("   \t\r\n"));
  assert(command_validation::hasNonWhitespaceContent(" X "));
  assert(command_validation::hasNonWhitespaceContent("0"));

  EditingCommandState<48> editor(committed);
  assert(!editor.isEditing());
  assert(editor.begin());
  assert(editor.mode() == EditingCommandMode::EditingCommand);
  assert(std::strcmp(committed.c_str(), "Cambia la tarea J10 a mensual") == 0);

  assert(touch_priority::route(true, 30, 390, true) == touch_priority::Route::VirtualEditor);
  assert(touch_priority::route(true, 300, 390, true) == touch_priority::Route::VirtualEditor);
  assert(touch_priority::route(true, 20, 370, false) == touch_priority::Route::ProbeWsl);
  assert(touch_priority::route(true, 250, 370, false) == touch_priority::Route::Send3C);
  assert(touch_priority::route(true, 230, 400, false) == touch_priority::Route::None);

  virtual_keyboard::Key q{};
  assert(virtual_keyboard::hitTest(virtual_keyboard::KeyboardMode::Alpha,
                                    virtual_keyboard::columnOrigin(0) + 1,
                                    virtual_keyboard::rowOrigin(0) + 1, &q));
  assert(q.definition.kind == virtual_keyboard::KeyKind::Character);
  assert(q.definition.label[0] == 'Q');
  assert(!virtual_keyboard::hitTest(virtual_keyboard::KeyboardMode::Alpha,
                                    virtual_keyboard::kScreenWidth, 1));

  assert(editor.draft().insert('X'));
  editor.draft().moveLeft();
  const std::size_t before = editor.draft().length();
  assert(editor.draft().insert('Y'));
  assert(editor.draft().length() == before + 1);
  assert(editor.draft().backspace());
  assert(editor.draft().length() == before);
  assert(editor.invariantHolds());

  // A whitespace-only draft must not be committed, and the existing command must remain intact.
  editor.draft().clear();
  assert(!editor.ok());
  assert(editor.isEditing());
  assert(editor.lastRejection() == CommandCommitRejection::EmptyCommand);
  assert(std::strcmp(committed.c_str(), "Cambia la tarea J10 a mensual") == 0);

  assert(editor.draft().set(" \t "));
  assert(!editor.ok());
  assert(editor.isEditing());
  assert(editor.lastRejection() == CommandCommitRejection::EmptyCommand);
  assert(std::strcmp(committed.c_str(), "Cambia la tarea J10 a mensual") == 0);

  assert(editor.draft().set("Comando valido"));
  assert(editor.ok());
  assert(!editor.isEditing());
  assert(std::strcmp(committed.c_str(), "Comando valido") == 0);

  uint16_t widths[49] = {};
  for (std::size_t i = 0; i < 48; ++i) widths[i + 1] = static_cast<uint16_t>(widths[i] + 9);
  auto window = command_text_viewport::compute(widths, 48, 48, 100, 2);
  assert(window.last == 48);
  assert(window.first > 0);
  assert(window.cursorX <= 98);

  assert(editor.begin());
  const char committedBeforeCancel[] = "Comando valido";
  editor.draft().moveEnd();
  assert(editor.draft().insert('!'));
  assert(editor.cancel());
  assert(std::strcmp(committed.c_str(), committedBeforeCancel) == 0);
  assert(!editor.isEditing());

  assert(editor.begin());
  editor.draft().moveEnd();
  assert(editor.draft().insert('?'));
  assert(editor.ok());
  assert(!editor.isEditing());
  assert(std::strstr(committed.c_str(), "?") != nullptr);
  assert(!editor.ok());
  assert(!editor.cancel());
  assert(editor.invariantHolds());

  const touch::KeyFrame overlap[] = {{0, 0, 20, 20}, {10, 0, 20, 20}};
  assert(touch::hitTest(overlap, 2, 15, 10) == touch::kNoKey);
  return 0;
}
