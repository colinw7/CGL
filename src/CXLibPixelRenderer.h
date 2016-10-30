#ifndef CXLIB_PIXEL_RENDERER_H
#define CXLIB_PIXEL_RENDERER_H

#include <CILineDash.h>
#include <CImagePtr.h>

#define CPIXEL_RENDERER_VIRTUAL

#include <CPixelRenderer.h>

class CXWindow;

class CXLibPixelRenderer : public CPixelRenderer {
 private:
  CXWindow *window_;
  CISize2D  window_size_;

 public:
  CXLibPixelRenderer(CXWindow *window);
 ~CXLibPixelRenderer() { }

  CXLibPixelRenderer *dup() const;

  uint getWidth () const { return window_size_.getWidth (); }
  uint getHeight() const { return window_size_.getHeight(); }

  void setWindow(CXWindow *window);

  void updateSize(int width, int height);

  void startDoubleBuffer(bool clear=true);
  void endDoubleBuffer(bool copy=true);
  void copyDoubleBuffer();

  void setForeground(const CRGBA &rgba);
  void setBackground(const CRGBA &rgba);

  void setFont(CFontPtr font);

  void clear();
  void fill();

  void drawClippedPoint(const CIPoint2D &point);

  void drawClippedChar(const CIPoint2D &point, char c);
  void drawClippedCharToRenderer(CPixelRenderer *renderer,
                                 const CIPoint2D &point, char c);

  void drawClippedString(const CIPoint2D &point, const std::string &str);

  void drawClippedLine(const CIPoint2D &point1, const CIPoint2D &point2);

  void drawClippedRectangle(const CIBBox2D &bbox);
  void fillClippedRectangle(const CIBBox2D &bbox);

  void fillClippedTriangle(const CIPoint2D &point1, const CIPoint2D &point2,
                           const CIPoint2D &point3);

  void drawClippedPolygon(const IPointList &points);
  void fillClippedPolygon(const IPointList &points);

  void drawClippedArc(int x, int y, int xr, int yr, double a1, double a2);
  void fillClippedArc(int x, int y, int xr, int yr, double a1, double a2);

  void drawClippedImage(const CIPoint2D &point, CImagePtr image);

  void drawClippedAlphaImage(const CIPoint2D &point, CImagePtr image);

  void getClip(int *xmin, int *ymin, int *xmax, int *ymax);

  int getCharWidth ();
  int getCharAscent();
  int getCharDescent();
  int getCharHeight();

  int getStringWidth(const std::string &str);

  int getCharWidth(char c) {
    std::string str(&c, 1);

    return getStringWidth(str);
  }

  void flush();

  bool mmToPixel(double mm, double *pixel);
};

#endif
