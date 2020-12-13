#pragma once

#include <any>
#include <string>
#include <vector>

class DataEncoder {
 public:
  virtual ~DataEncoder() = default;
  virtual std::any stringToObj(const std::string& encoded_str) const = 0;
  virtual std::vector<std::string> objToString(const std::any& object,
                                               const int max_line_len = 80) const = 0;
};
