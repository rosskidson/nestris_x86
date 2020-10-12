#pragma once

#include <string>

template <typename SpriteType>
class PixelDrawingInterface {
 public:
  struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
    static Color WHITE() { return Color{255, 255, 255, 255}; }
    static Color BLACK() { return Color{0, 0, 0, 255}; }
  };

  virtual void drawSprite(const int x, const int y, SpriteType& sprite) = 0;

  virtual void drawString(const int x, const int y, const std::string& text,
                          const Color& color = Color::WHITE()) = 0;
  virtual void drawPixel(const int x, const int y, const Color& color = Color::WHITE()) = 0;
  virtual void drawLine(const int x1, const int y1, const int x2, const int y2,
                        const Color& color = Color::WHITE()) = 0;
  virtual void drawCircle(const int x, const int y, const int radius,
                          const Color& color = Color::WHITE()) = 0;
  virtual void fillCircle(const int x, const int y, const int radius,
                          const Color& color = Color::WHITE()) = 0;
  virtual void fillRect(const int x, const int y, const int width, const int height,
                        const Color& color = Color::WHITE()) = 0;
};
