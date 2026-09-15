#include <cassert>
#include <string>

#include "command_edit_state.h"

using command_ui::EditingCommandState;
using command_ui::Event;
using command_ui::Mode;

int main() {
  EditingCommandState state;

  // Normal mode is the default and rejects edit-only input.
  assert(state.mode() == Mode::Normal);
  assert(!state.isEditing());
  assert(!state.handle(Event::inputText("abc")).changed);
  assert(state.commandBuffer().empty());

  // Normal -> Keyboard Editing, with a deterministic seed.
  auto transition = state.beginEditing("Cambia la tarea J10 a mensual");
  assert(transition.changed);
  assert(!transition.commandReady);
  assert(state.mode() == Mode::EditingCommand);
  assert(state.commandBuffer() == "Cambia la tarea J10 a mensual");

  // Editing mutates only the command buffer, not the mode.
  transition = state.handle(Event::inputText("."));
  assert(transition.changed);
  assert(!transition.commandReady);
  assert(state.mode() == Mode::EditingCommand);
  assert(state.commandBuffer() == "Cambia la tarea J10 a mensual.");

  transition = state.handle(Event::backspace());
  assert(transition.changed);
  assert(state.commandBuffer() == "Cambia la tarea J10 a mensual");

  transition = state.handle(Event::clear());
  assert(transition.changed);
  assert(state.commandBuffer().empty());
  assert(state.mode() == Mode::EditingCommand);

  // Empty input cannot be committed; editor stays active.
  transition = state.handle(Event::commitEditing());
  assert(!transition.changed);
  assert(!transition.commandReady);
  assert(state.mode() == Mode::EditingCommand);

  // A real command commits and returns to Normal.
  state.handle(Event::inputText("prueba 3C"));
  transition = state.handle(Event::commitEditing());
  assert(transition.changed);
  assert(transition.commandReady);
  assert(state.mode() == Mode::Normal);
  assert(state.commandBuffer() == "prueba 3C");

  // Normal mode can be re-entered without losing the last buffer.
  transition = state.beginEditing();
  assert(transition.changed);
  assert(state.mode() == Mode::EditingCommand);
  assert(state.commandBuffer() == "prueba 3C");

  // Cancel returns to Normal without destroying the user's draft.
  state.handle(Event::inputText(" + mas"));
  transition = state.handle(Event::cancelEditing());
  assert(transition.changed);
  assert(state.mode() == Mode::Normal);
  assert(state.commandBuffer() == "prueba 3C + mas");

  // Repeated begin/cancel events are idempotent from the wrong mode.
  transition = state.handle(Event::cancelEditing());
  assert(!transition.changed);
  assert(state.mode() == Mode::Normal);

  transition = state.beginEditing("nuevo comando");
  assert(transition.changed);
  assert(state.commandBuffer() == "nuevo comando");
  transition = state.handle(Event::beginEditing());
  assert(!transition.changed);
  assert(state.commandBuffer() == "nuevo comando");

  return 0;
}
