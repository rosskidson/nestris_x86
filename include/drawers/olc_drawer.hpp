#pragma once

#include <any>

#include "olcPixelGameEngine.h"
#include "pixel_drawing_interface.hpp"

namespace tetris_clone {

class OlcDrawer : public PixelDrawingInterface {
 public:
  OlcDrawer(olc::PixelGameEngine& pixel_game_engine);

  // std::any& sprite must of type olc::Sprite.
  void drawSprite(const int x, const int y, const std::any& sprite) const override;

  void drawString(const int x, const int y, const std::string& text,
                  const Color& color = WHITE()) const override;

  void drawPixel(const int x, const int y, const Color& color = WHITE()) const override;

  void drawLine(const int x1, const int y1, const int x2, const int y2,
                const Color& color = WHITE()) const override;

  void drawCircle(const int x, const int y, const int radius,
                  const Color& color = WHITE()) const override;

  void fillCircle(const int x, const int y, const int radius,
                  const Color& color = WHITE()) const override;

  void fillRect(const int x, const int y, const int width, const int height,
                const Color& color = WHITE()) const override;

 private:
  olc::PixelGameEngine& olc_engine_ref_;
};

inline olc::Pixel toOlcPix(const PixelDrawingInterface::Color& color) {
  return {color.r, color.g, color.b, color.a};
}


}  // namespace tetris_clone
