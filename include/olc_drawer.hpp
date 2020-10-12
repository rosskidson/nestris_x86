#pragma once

#include "olcPixelGameEngine.h"
#include "pixel_drawing_interface.hpp"

class OlcDrawer : public PixelDrawingInterface<olc::Sprite> {
 public:
  OlcDrawer(olc::PixelGameEngine& pixel_game_engine);

  void drawSprite(const int x, const int y, olc::Sprite& sprite) override;

  void drawString(const int x, const int y, const std::string& text,
                  const Color& color = Color::WHITE()) override;
  void drawPixel(const int x, const int y, const Color& color = Color::WHITE()) override;
  void drawLine(const int x1, const int y1, const int x2, const int y2,
                const Color& color = Color::WHITE()) override;
  void drawCircle(const int x, const int y, const int radius,
                  const Color& color = Color::WHITE()) override;
  void fillCircle(const int x, const int y, const int radius,
                  const Color& color = Color::WHITE()) override;
  void fillRect(const int x, const int y, const int width, const int height,
                const Color& color = Color::WHITE()) override;

 private:
  olc::PixelGameEngine& olc_engine_ref_;
};

inline olc::Pixel toOlcPix(const PixelDrawingInterface<olc::Sprite>::Color& color) {
  return {color.r, color.g, color.b, color.a};
}

void OlcDrawer::drawSprite(const int x, const int y, olc::Sprite& sprite) {
  olc_engine_ref_.DrawSprite(x, y, &sprite);
}
