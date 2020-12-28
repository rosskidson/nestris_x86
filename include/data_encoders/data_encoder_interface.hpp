#pragma once

#include <any>
#include <vector>

namespace data_encoding {
class DataEncoder {
 public:
  virtual ~DataEncoder() = default;
  virtual std::any dataToObj(const std::vector<long>& encoded_str) const = 0;
  virtual std::vector<long> objToData(const std::any& object,
                                      const int max_line_len = 80) const = 0;
};

}
