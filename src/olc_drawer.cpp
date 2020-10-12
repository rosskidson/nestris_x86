#include "olc_drawer.hpp"

OlcDrawer::OlcDrawer(olc::PixelGameEngine& pixel_game_engine)
    : olc_engine_ref_(pixel_game_engine) {}

void OlcDrawer::drawString(const int x, const int y, const std::string& text, const Color& color) {
  olc_engine_ref_.DrawString(x, y, text, toOlcPix(color));
}

void OlcDrawer::drawPixel(const int x, const int y, const Color& color) {
  olc_engine_ref_.Draw(x, y, toOlcPix(color));
}

void OlcDrawer::drawLine(const int x1, const int y1, const int x2, const int y2,
                         const Color& color) {
  olc_engine_ref_.DrawLine(x1, y1, x2, y2, toOlcPix(color));
}

void OlcDrawer::drawCircle(const int x, const int y, const int radius, const Color& color) {
  olc_engine_ref_.DrawCircle(x, y, radius, toOlcPix(color));
}

void OlcDrawer::fillCircle(const int x, const int y, const int radius, const Color& color) {
  olc_engine_ref_.FillCircle(x, y, radius, toOlcPix(color));
}

void OlcDrawer::fillRect(const int x, const int y, const int width, const int height,
                         const Color& color) {
  olc_engine_ref_.FillRect(x, y, width, height, toOlcPix(color));
}
