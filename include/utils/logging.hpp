#pragma once

#include <iostream>

namespace logging {
inline std::string getFilename(const std::string& str) {
  const auto found = str.find_last_of("/\\");
  return str.substr(found + 1);
}
}  // namespace logging

#define LOG_ERROR(msg)                                                                      \
  std::cerr << "[ERROR] " << logging::getFilename(std::string{__FILE__}) << ":" << __LINE__ \
            << ": " << msg << std::endl;

#define LOG_INFO(msg)                                                                              \
  std::cout << "[INFO] " << logging::getFilename(std::string{__FILE__}) << ":" << __LINE__ << ": " \
            << msg << std::endl;

