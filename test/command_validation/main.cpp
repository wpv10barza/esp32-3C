#include <cassert>

#include "command_validation.h"

int main() {
  assert(!command_validation::isNonEmpty(nullptr));
  assert(!command_validation::isNonEmpty(""));
  assert(!command_validation::isNonEmpty(" "));
  assert(!command_validation::isNonEmpty("\t\n\r"));
  assert(!command_validation::isNonEmpty("  \t  \n"));

  assert(command_validation::isNonEmpty("x"));
  assert(command_validation::isNonEmpty(" comando "));
  assert(command_validation::isNonEmpty("\t3C"));
  assert(command_validation::isNonEmpty("3C\n"));

  return 0;
}
