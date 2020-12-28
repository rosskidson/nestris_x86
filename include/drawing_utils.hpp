#pragma once

#include "drawers/pixel_drawing_interface.hpp"

namespace nestris_x86 {
// clang-format off
void drawNumber(const PixelDrawingInterface& drawer,
                const int x,
                const int y,
                const int num,
                const int pad,
                const PixelDrawingInterface::Color& color = PixelDrawingInterface::WHITE());

void drawNumber(const PixelDrawingInterface& drawer,
                const PixelDrawingInterface::Coords& coords,
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

}  // namespace nestris_x86
