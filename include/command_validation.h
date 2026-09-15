#pragma once

#include <cctype>

namespace command_validation {

inline bool isNonEmpty(const char* text) {
  if (text == nullptr) return false;
  for (const unsigned char* cursor = reinterpret_cast<const unsigned char*>(text); *cursor != '\0'; ++cursor) {
    if (!std::isspace(*cursor)) return true;
  }
  return false;
}

}  // namespace command_validation
