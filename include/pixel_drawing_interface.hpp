#pragma once

#include <any>
#include <string>

namespace tetris_clone {

class PixelDrawingInterface {
 public:
  struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  };
  static Color WHITE() { return Color{255, 255, 255, 255}; }
  static Color BLACK() { return Color{0, 0, 0, 255}; }
  static Color GREY() { return Color{192, 192, 192, 255}; }
  static Color DARK_GREY() { return Color{128, 128, 128, 255}; }
  static Color RED() { return Color{255, 0, 0, 255}; }
  static Color BLUE() { return Color{0, 255, 0, 255}; }
  static Color GREEN() { return Color{0, 0, 255, 255}; }

  virtual void drawSprite(const int x, const int y, const std::any& sprite) const = 0;

  virtual void drawString(const int x, const int y, const std::string& text,
                          const Color& color = WHITE()) const = 0;

  virtual void drawPixel(const int x, const int y, const Color& color = WHITE()) const = 0;

  virtual void drawLine(const int x1, const int y1, const int x2, const int y2,
                        const Color& color = WHITE()) const = 0;

  virtual void drawCircle(const int x, const int y, const int radius,
                          const Color& color = WHITE()) const = 0;

  virtual void fillCircle(const int x, const int y, const int radius,
                          const Color& color = WHITE()) const = 0;

  virtual void fillRect(const int x, const int y, const int width, const int height,
                        const Color& color = WHITE()) const = 0;
};

}  // namespace tetris_clone
