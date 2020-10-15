#pragma once

#include <iostream>


// TODO:: Remove full path

#define LOG_ERROR(msg) \
  std::cerr << "[ERROR] " << __FILE__ << ":" << __LINE__ << ": " << msg << std::endl;

#define LOG_INFO(msg) \
  std::cout << "[INFO] " << __FILE__ << ":" << __LINE__ << ": " << msg << std::endl;

