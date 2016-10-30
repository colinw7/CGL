#include <CPixelRenderer.h>
#include <CGUIPixelRenderer.h>

void
CGUIPixelRenderer::
drawButton(int x, int y, int width, int height, int thickness, bool pressed)
{
  if (pressed)
    drawFrame(x, y, width, height, thickness, FRAME_SUNKEN);
  else
    drawFrame(x, y, width, height, thickness, FRAME_RAISED);
}

void
CGUIPixelRenderer::
drawFrame(int x, int y, int width, int height, int thickness, FrameStyle style)
{
  CRGBA bg, fg;

  renderer_->getBackground(bg);
  renderer_->getForeground(fg);

  CRGBA bg1 = bg.getDarkRGBA ();
  CRGBA bg2 = bg.getLightRGBA();

  if (style == FRAME_SUNKEN)
    std::swap(bg1, bg2);

  // top

  {
    int x1 = x;
    int x2 = x + width - 1;
    int y1 = y;

    renderer_->setForeground(bg2);

    for (int i = 0; i < thickness; ++i, ++x1, --x2, ++y1)
      renderer_->drawLine(CIPoint2D(x1, y1), CIPoint2D(x2, y1));
  }

  // left

  {
    int x1 = x;
    int y1 = y + 1;
    int y2 = y + height - 1;

    renderer_->setForeground(bg2);

    for (int i = 0; i < thickness; ++i, ++x1, ++y1, --y2)
      renderer_->drawLine(CIPoint2D(x1, y1), CIPoint2D(x1, y2));
  }

  // bottom

  {
    int x1 = x;
    int y1 = y + height - 1;
    int x2 = x + width  - 1;

    renderer_->setForeground(bg1);

    for (int i = 0; i < thickness; ++i, ++x1, --x2, --y1)
      renderer_->drawLine(CIPoint2D(x1, y1), CIPoint2D(x2, y1));
  }

  // right

  {
    int x1 = x + width  - 1;
    int y1 = y;
    int y2 = y + height - 1;

    renderer_->setForeground(bg1);

    for (int i = 0; i < thickness; ++i, --x1, ++y1, --y2)
      renderer_->drawLine(CIPoint2D(x1, y1), CIPoint2D(x1, y2));
  }

  renderer_->setForeground(bg);

  renderer_->fillRectangle(CIBBox2D(x +            thickness    , y            + thickness,
                                    x + width  - 2*thickness + 1, y + height - 2*thickness + 1));

  renderer_->setForeground(fg);
}

void
CGUIPixelRenderer::
drawTriangleButton(int x, int y, int width, int height, CDirectionType direction,
                   int thickness, bool pressed)
{
  CRGBA bg, fg;

  renderer_->getBackground(bg);
  renderer_->getForeground(fg);

  CRGBA bg1 = bg.getDarkRGBA ();
  CRGBA bg2 = bg.getLightRGBA();

  if (pressed)
    std::swap(bg1, bg2);

  int x1 = x            ; int y1 = y;
  int x2 = x + width - 1; int y2 = y + height - 1;

  int xm = (x1 + x2)/2;
  int ym = (y1 + y2)/2;

  CPixelRenderer::IPointList points;

  points.resize(3);

  if      (direction == CDIRECTION_TYPE_LEFT) {
    points[0] = CIPoint2D(x2, y1);
    points[1] = CIPoint2D(x1, ym);
    points[2] = CIPoint2D(x2, y2);
  }
  else if (direction == CDIRECTION_TYPE_RIGHT) {
    points[0] = CIPoint2D(x1, y1);
    points[1] = CIPoint2D(x2, ym);
    points[2] = CIPoint2D(x1, y2);
  }
  else if (direction == CDIRECTION_TYPE_UP) {
    points[0] = CIPoint2D(x1, y2);
    points[1] = CIPoint2D(xm, y1);
    points[2] = CIPoint2D(x2, y2);
  }
  else {
    points[0] = CIPoint2D(x1, y1);
    points[1] = CIPoint2D(xm, y2);
    points[2] = CIPoint2D(x2, y1);
  }

  renderer_->setForeground(bg);

  renderer_->fillPolygon(points);

  for (int i = 0; i < thickness; i++) {
    int x1i = x1 + i;
    int x2i = x2 - i;
    int y1i = y1 + i;
    int y2i = y2 - i;

    if      (direction == CDIRECTION_TYPE_LEFT) {
      renderer_->setForeground(bg1);

      renderer_->drawLine(CIPoint2D(x1i, ym ), CIPoint2D(x2i, y2i));
      renderer_->drawLine(CIPoint2D(x2i, y2i), CIPoint2D(x2i, y1i));

      renderer_->setForeground(bg2);

      renderer_->drawLine(CIPoint2D(x1i, ym ), CIPoint2D(x2i, y1i));
    }
    else if (direction == CDIRECTION_TYPE_RIGHT) {
      renderer_->setForeground(bg2);

      renderer_->drawLine(CIPoint2D(x1i, y2i), CIPoint2D(x1i, y1i));
      renderer_->drawLine(CIPoint2D(x1i, y1i), CIPoint2D(x2i, ym ));

      renderer_->setForeground(bg1);

      renderer_->drawLine(CIPoint2D(x1i, y2i), CIPoint2D(x2i, ym ));
    }
    else if (direction == CDIRECTION_TYPE_UP) {
      renderer_->setForeground(bg1);

      renderer_->drawLine(CIPoint2D(x1i, y2i), CIPoint2D(x2i, y2i));
      renderer_->drawLine(CIPoint2D(x2i, y2i), CIPoint2D(xm , y1i));

      renderer_->setForeground(bg2);

      renderer_->drawLine(CIPoint2D(x1i, y2i), CIPoint2D(xm , y1i));
    }
    else if (direction == CDIRECTION_TYPE_DOWN) {
      renderer_->setForeground(bg2);

      renderer_->drawLine(CIPoint2D(xm , y2i), CIPoint2D(x1i, y1i));
      renderer_->drawLine(CIPoint2D(x1i, y1i), CIPoint2D(x2i, y1i));

      renderer_->setForeground(bg1);

      renderer_->drawLine(CIPoint2D(xm , y2i), CIPoint2D(x2i, y1i));
    }
  }

  renderer_->setForeground(fg);
}
