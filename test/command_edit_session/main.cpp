#include <cassert>
#include <cstring>

#include "command_edit_session.h"

int main() {
  using Buffer = CommandBuffer<16>;

  Buffer committed;
  assert(committed.set("orden valida"));
  CommandEditSession<16> session(committed);

  assert(session.begin());
  session.draft().clear();
  assert(!session.ok());
  assert(session.lastError() == CommandEditError::EmptyCommand);
  assert(session.editing());
  assert(std::strcmp(committed.c_str(), "orden valida") == 0);
  assert(session.invariantHolds());

  session.draft().set("   \t\n");
  assert(!session.ok());
  assert(session.lastError() == CommandEditError::EmptyCommand);
  assert(session.editing());
  assert(std::strcmp(committed.c_str(), "orden valida") == 0);

  session.draft().set("nueva orden");
  assert(session.ok());
  assert(!session.editing());
  assert(session.lastError() == CommandEditError::None);
  assert(std::strcmp(committed.c_str(), "nueva orden") == 0);
  assert(session.invariantHolds());

  assert(session.begin());
  session.draft().clear();
  assert(!session.ok());
  assert(session.editing());
  assert(std::strcmp(committed.c_str(), "nueva orden") == 0);
  assert(session.cancel());
  assert(std::strcmp(committed.c_str(), "nueva orden") == 0);
  assert(session.invariantHolds());

  return 0;
}
