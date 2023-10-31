#include <CGLAccumBuffer.h>
#include <CGL.h>

CGLAccumBuffer::
CGLAccumBuffer(uint width, uint height) :
 width_(0), height_(0), lines_(nullptr), clear_color_(0,0,0,1)
{
  resize(width, height);
}

CGLAccumBuffer::
~CGLAccumBuffer()
{
  resize(0, 0);
}

void
CGLAccumBuffer::
resize(uint width, uint height)
{
  if (width == width_ && height == height_)
    return;

  for (uint y = 0; y < height_; ++y)
    delete [] lines_[y].points;

  delete [] lines_;

  //-----

  width_  = width;
  height_ = height;

  if (height_ > 0) {
    lines_ = new Line [height_];

    for (uint y = 0; y < height_; ++y) {
      Line *line = &lines_[y];

      line->points = new Point [width_];
    }
  }
  else
    lines_ = NULL;
}

void
CGLAccumBuffer::
clear()
{
  for (uint y = 0; y < height_; ++y) {
    Line *line = &lines_[y];

    for (uint x = 0; x < width_; ++x) {
      Point *point = &line->points[x];

      point->rgba = clear_color_;
    }
  }
}

const CRGBA &
CGLAccumBuffer::
getPoint(uint x, uint y)
{
  Line *line = &lines_[y];

  Point *point = &line->points[x];

  return point->rgba;
}

void
CGLAccumBuffer::
setPoint(uint x, uint y, const CRGBA &color)
{
  Line *line = &lines_[y];

  Point *point = &line->points[x];

  point->rgba = color;
}
