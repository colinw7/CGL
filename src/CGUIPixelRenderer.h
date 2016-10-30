#ifndef CGUI_PIXEL_RENDERER_H
#define CGUI_PIXEL_RENDERER_H

#include <CDirectionType.h>

class CPixelRenderer;

class CGUIPixelRenderer {
 private:
  CPixelRenderer *renderer_;

 public:
  enum FrameStyle {
    FRAME_RAISED,
    FRAME_SUNKEN
  };

  CGUIPixelRenderer(CPixelRenderer *renderer) :
   renderer_(renderer) {
  }

  virtual ~CGUIPixelRenderer() { }

 public:
  virtual void drawButton(int x, int y, int width, int height,
                          int thickness=2, bool pressed=false);

  virtual void drawFrame(int x, int y, int width, int height,
                         int thickness=2, FrameStyle style=FRAME_RAISED);

  virtual void drawTriangleButton(int x, int y, int width, int height,
                                  CDirectionType direction, int thickness=2,
                                  bool pressed=false);
};

#endif
