#pragma once

#include "pixel_drawing_interface.hpp"

namespace tetris_clone {
// clang-format off
void drawNumber(const PixelDrawingInterface& drawer,
                const int x,
                const int y,
                const int num,
                const int pad,
                const PixelDrawingInterface::Color& color = PixelDrawingInterface::WHITE());

void drawTriangleSelector(const PixelDrawingInterface& drawer,
                          const int x,
                          const int y,
                          const int size,
                          const PixelDrawingInterface::Color& color,
                          const bool right);
// clang-format off

}  // namespace tetris_clone
