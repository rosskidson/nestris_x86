#pragma once

#include <iostream>

inline std::string getFilename(const std::string& str) {
  const auto found = str.find_last_of("/\\");
  return str.substr(found + 1);
}

#define LOG_ERROR(msg)                                                                            \
  std::cerr << "[ERROR] " << getFilename(std::string{__FILE__}) << ":" << __LINE__ << ": " << msg \
            << std::endl;

#define LOG_INFO(msg)                                                                            \
  std::cout << "[INFO] " << getFilename(std::string{__FILE__}) << ":" << __LINE__ << ": " << msg \
            << std::endl;

