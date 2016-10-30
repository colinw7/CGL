#include <CXLibPixelRenderer.h>
#include <CXLib.h>

CXLibPixelRenderer::
CXLibPixelRenderer(CXWindow *window) :
 CPixelRenderer(), window_(window)
{
  window_size_ = CISize2D(window_->getWidth(), window_->getHeight());
}

CXLibPixelRenderer *
CXLibPixelRenderer::
dup() const
{
  return new CXLibPixelRenderer(window_);
}

void
CXLibPixelRenderer::
setWindow(CXWindow *window)
{
  window_ = window;

  updateSize(100, 100);
}

void
CXLibPixelRenderer::
updateSize(int, int)
{
  int width  = window_->getWidth();
  int height = window_->getHeight();

  window_size_ = CISize2D(width, height);
}

void
CXLibPixelRenderer::
startDoubleBuffer(bool clear)
{
  window_->startDoubleBuffer(clear);
}

void
CXLibPixelRenderer::
endDoubleBuffer(bool)
{
  window_->endDoubleBuffer();
}

void
CXLibPixelRenderer::
copyDoubleBuffer()
{
  window_->copyDoubleBuffer();
}

void
CXLibPixelRenderer::
setForeground(const CRGBA &fg)
{
  if (fg.getAlpha() != 1) {
    CRGB fg1 = fg.getRGB()*fg.getAlpha();

    window_->setForeground(fg1);
  }
  else
    window_->setForeground(fg);

  CPixelRenderer::setForeground(fg);
}

void
CXLibPixelRenderer::
setBackground(const CRGBA &bg)
{
  window_->setBackground(bg);

  CPixelRenderer::setBackground(bg);
}

void
CXLibPixelRenderer::
setFont(CFontPtr font)
{
  window_->setFont(font);

  CPixelRenderer::setFont(font);
}

void
CXLibPixelRenderer::
clear()
{
  window_->clear();
}

void
CXLibPixelRenderer::
fill()
{
  window_->fill();
}

void
CXLibPixelRenderer::
drawClippedPoint(const CIPoint2D &point)
{
  window_->drawPoint(point.x, point.y);
}

void
CXLibPixelRenderer::
drawClippedChar(const CIPoint2D &point, char c)
{
  std::string str;

  str += c;

  window_->drawText(point.x, point.y, str);
}

void
CXLibPixelRenderer::
drawClippedCharToRenderer(CPixelRenderer *, const CIPoint2D &, char)
{
  assert(0);
}

void
CXLibPixelRenderer::
drawClippedString(const CIPoint2D &point, const std::string &str)
{
  window_->drawText(point.x, point.y, str);
}

void
CXLibPixelRenderer::
drawClippedLine(const CIPoint2D &point1, const CIPoint2D &point2)
{
  if (! getLineDash().isSolid())
    window_->setLineDash(getLineDash());

  if (getLineWidth() > 0)
    window_->setLineWidth(getLineWidth());

  window_->drawLine(point1.x, point1.y, point2.x, point2.y);

  if (getLineWidth() > 0)
    window_->setLineWidth(0);

  if (! getLineDash().isSolid())
    window_->setLineDash(CILineDash());
}

void
CXLibPixelRenderer::
drawClippedRectangle(const CIBBox2D &bbox)
{
  window_->drawRectangle(bbox.getXMin(), bbox.getYMin(),
                         bbox.getWidth() + 1, bbox.getHeight() + 1);
}

void
CXLibPixelRenderer::
fillClippedRectangle(const CIBBox2D &bbox)
{
  window_->fillRectangle(bbox.getXMin(), bbox.getYMin(),
                         bbox.getWidth() + 1, bbox.getHeight() + 1);
}

void
CXLibPixelRenderer::
fillClippedTriangle(const CIPoint2D &point1, const CIPoint2D &point2,
                    const CIPoint2D &point3)
{
  CPixelRenderer::fillClippedTriangle(point1, point2, point3);
}

void
CXLibPixelRenderer::
drawClippedPolygon(const IPointList &points)
{
  CPixelRenderer::drawClippedPolygon(points);
}

void
CXLibPixelRenderer::
fillClippedPolygon(const IPointList &points)
{
  CPixelRenderer::fillClippedPolygon(points);
}

void
CXLibPixelRenderer::
drawClippedArc(int x, int y, int xr, int yr, double a1, double a2)
{
  window_->drawArc(x, y, xr, yr, a1, a2);
}

void
CXLibPixelRenderer::
fillClippedArc(int x, int y, int xr, int yr, double a1, double a2)
{
  window_->fillArc(x, y, xr, yr, a1, a2);
}

void
CXLibPixelRenderer::
drawClippedImage(const CIPoint2D &point, CImagePtr image)
{
  window_->drawImage(image, point.x, point.y);
}

void
CXLibPixelRenderer::
drawClippedAlphaImage(const CIPoint2D &point, CImagePtr image)
{
  window_->drawAlphaImage(image, point.x, point.y);
}

void
CXLibPixelRenderer::
getClip(int *xmin, int *ymin, int *xmax, int *ymax)
{
  *xmin = 0;
  *ymin = 0;
  *xmax = getWidth();
  *ymax = getHeight();
}

int
CXLibPixelRenderer::
getCharWidth()
{
  CFontPtr font;

  getFont(font);

  if (font.isValid())
    return font->getICharWidth();
  else
    return 8;
}

int
CXLibPixelRenderer::
getCharAscent()
{
  CFontPtr font;

  getFont(font);

  if (font.isValid())
    return font->getICharAscent();
  else
    return 10;
}

int
CXLibPixelRenderer::
getCharDescent()
{
  CFontPtr font;

  getFont(font);

  if (font.isValid())
    return font->getICharDescent();
  else
    return 2;
}

int
CXLibPixelRenderer::
getCharHeight()
{
  CFontPtr font;

  getFont(font);

  if (font.isValid())
    return font->getICharHeight();
  else
    return 12;
}

int
CXLibPixelRenderer::
getStringWidth(const std::string &str)
{
  CFontPtr font;

  getFont(font);

  if (font.isValid())
    return font->getIStringWidth(str);
  else
    return str.size()*getCharWidth();
}

void
CXLibPixelRenderer::
flush()
{
  window_->flushEvents();
}

bool
CXLibPixelRenderer::
mmToPixel(double mm, double *pixel)
{
  return CXMachineInst->mmToPixel(mm, pixel);
}
