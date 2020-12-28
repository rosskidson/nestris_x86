#pragma once

#include <any>
#include <string>

namespace nestris_x86 {

class PixelDrawingInterface {
 public:
  struct Coords {
    int x;
    int y;
  };

  struct Rect {
    int width;
    int height;
  };

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
  static Color GREEN() { return Color{0, 255, 0, 255}; }
  static Color BLUE() { return Color{0, 0, 255, 255}; }
  static Color YELLOW() { return Color{255, 255, 0, 255}; }

  virtual ~PixelDrawingInterface() = default;

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

  inline void drawSprite(const Coords& coords, const std::any& sprite) {
    drawSprite(coords.x, coords.y, sprite);
  }

  inline void drawString(const Coords& coords, const std::string& text,
                         const Color& color = WHITE()) const {
    drawString(coords.x, coords.y, text, color);
  }

  inline void drawPixel(const Coords& coords, const Color& color = WHITE()) const {
    drawPixel(coords.x, coords.y, color);
  }

  inline void drawLine(const Coords p1, const Coords& p2, const Color& color = WHITE()) const {
    drawLine(p1.x, p1.y, p2.y, p2.y, color);
  }

  inline void drawCircle(const Coords& coords, const int radius,
                         const Color& color = WHITE()) const {
    drawCircle(coords.x, coords.y, radius, color);
  }

  inline void fillCircle(const Coords& coords, const int radius,
                         const Color& color = WHITE()) const {
    fillCircle(coords.x, coords.y, radius, color);
  }

  inline void fillRect(const Coords& coords, const Rect& size, const Color& color = WHITE()) const {
    fillRect(coords.x, coords.y, size.width, size.height, color);
  }
};

inline PixelDrawingInterface::Coords operator+(const PixelDrawingInterface::Coords& lhs,
                                               const PixelDrawingInterface::Coords& rhs) {
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}

}  // namespace nestris_x86
