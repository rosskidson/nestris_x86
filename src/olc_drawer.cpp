#include "olc_drawer.hpp"

#include "pixel_drawing_interface.hpp"

namespace tetris_clone {
using pdi = PixelDrawingInterface;

OlcDrawer::OlcDrawer(olc::PixelGameEngine& pixel_game_engine)
    : olc_engine_ref_(pixel_game_engine) {}

void OlcDrawer::drawSprite(const int x, const int y, const std::any& sprite) const try {
  auto olc_sprite = std::any_cast<olc::Sprite*>(sprite);
  olc_engine_ref_.DrawSprite(x, y, olc_sprite);
} catch (const std::bad_any_cast& e) {
  std::cerr
      << "Bad cast in OlcDrawer::drawSprite. The type required for std::any is an olc::Sprite*"
      << std::endl;
}

void OlcDrawer::drawString(const int x, const int y, const std::string& text,
                           const pdi::Color& color) const {
  olc_engine_ref_.DrawString(x, y, text, toOlcPix(color));
}

void OlcDrawer::drawPixel(const int x, const int y, const pdi::Color& color) const {
  olc_engine_ref_.Draw(x, y, toOlcPix(color));
}

void OlcDrawer::drawLine(const int x1, const int y1, const int x2, const int y2,
                         const pdi::Color& color) const {
  olc_engine_ref_.DrawLine(x1, y1, x2, y2, toOlcPix(color));
}

void OlcDrawer::drawCircle(const int x, const int y, const int radius,
                           const pdi::Color& color) const {
  olc_engine_ref_.DrawCircle(x, y, radius, toOlcPix(color));
}

void OlcDrawer::fillCircle(const int x, const int y, const int radius,
                           const pdi::Color& color) const {
  olc_engine_ref_.FillCircle(x, y, radius, toOlcPix(color));
}

void OlcDrawer::fillRect(const int x, const int y, const int width, const int height,
                         const pdi::Color& color) const {
  olc_engine_ref_.FillRect(x, y, width, height, toOlcPix(color));
}
}  // namespace tetris_clone
