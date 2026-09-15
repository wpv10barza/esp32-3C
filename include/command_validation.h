#pragma once

namespace command_validation {

inline bool hasNonWhitespaceContent(const char* text) {
  if (text == nullptr) return false;

  for (const unsigned char* current = reinterpret_cast<const unsigned char*>(text); *current != '\0'; ++current) {
    switch (*current) {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
      case '\v':
      case '\f':
        continue;
      default:
        return true;
    }
  }
  return false;
}

}  // namespace command_validation
