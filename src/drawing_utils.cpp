#include "drawing_utils.hpp"

#include <iomanip>
#include <sstream>

namespace tetris_clone {
using pdi = PixelDrawingInterface;

void drawNumber(const PixelDrawingInterface& drawer, const int x, const int y, const int num,
                const int pad, const pdi::Color& color) {
  constexpr int TEXT_WIDTH_PX = 8;
  drawer.fillRect(x, y, TEXT_WIDTH_PX * pad, TEXT_WIDTH_PX, pdi::BLACK());
  std::stringstream ss;
  ss << std::setw(pad) << std::setfill('0') << num;
  drawer.drawString(x, y, ss.str(), color);
}

void drawTriangleSelector(const PixelDrawingInterface& drawer, const int x, const int y,
                          const int size, const pdi::Color& color, const bool right) {
  for (int j = 0; j < size; ++j) {
    const int j_max = j < size / 2 ? j + 1 : size - j;
    if (right) {
      for (int i = 0; i > -j_max; --i) {
        drawer.drawPixel(x + i, y + j, color);
      }
    } else {
      for (int i = 0; i < j_max; ++i) {
        drawer.drawPixel(x + i, y + j, color);
      }
    }
  }
}
}  // namespace tetris_clone
