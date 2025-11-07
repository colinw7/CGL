#ifndef CGL_ACCUM_BUFFER_H
#define CGL_ACCUM_BUFFER_H

#include <CRGBA.h>
#include <accessor.h>

class CGLAccumBuffer {
 private:
  struct Point {
    CRGBA rgba;

    Point(const CRGBA &rgba1=CRGBA(0,0,0)) :
     rgba(rgba1) {
    }
  };

 public:
  CGLAccumBuffer(uint width=100, uint height=100);
 ~CGLAccumBuffer();

  void resize(uint width, uint height);

  uint getWidth () const { return width_ ; }
  uint getHeight() const { return height_; }

  ACCESSOR(ClearColor, CRGBA, clear_color)

  void clear();

  const CRGBA &getPoint(uint x, uint y);

  void setPoint(uint x, uint y, const CRGBA &color);

 private:
  struct Line {
    Point *points { nullptr };
  };

  //------

  uint   width_       { 0 };
  uint   height_      { 0 };
  Line*  lines_       { nullptr };
  CRGBA  clear_color_ { 0, 0, 0, 1 };
};

#endif
