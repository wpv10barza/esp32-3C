#pragma once

namespace command_validation {

// A command is executable only when it contains at least one byte that is not
// ASCII control/whitespace (the same <= 0x20 rule used by Arduino String::trim()).
// Null pointers and whitespace-only input fail closed.
inline bool hasExecutableText(const char* text) {
  if (text == nullptr) return false;

  for (const unsigned char* current =
           reinterpret_cast<const unsigned char*>(text);
       *current != '\0'; ++current) {
    if (*current > 0x20) return true;
  }
  return false;
}

}  // namespace command_validation
