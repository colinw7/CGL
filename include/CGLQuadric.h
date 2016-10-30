#ifndef CGL_QUADRIC_H
#define CGL_QUADRIC_H

#include <CPoint3D.h>
#include <accessor.h>
#include <map>
#include <sys/types.h>

class GLUquadric {
 public:
  typedef void (*CallBackFunc)();

 private:
  uint                        draw_style_;
  uint                        normals_;
  uint                        orientation_;
  bool                        texture_;
  std::map<uint,CallBackFunc> callbacks_;

 public:
  GLUquadric();
 ~GLUquadric();

  ACCESSOR(DrawStyle  , uint, draw_style )
  ACCESSOR(Normals    , uint, normals    )
  ACCESSOR(Orientation, uint, orientation)
  ACCESSOR(Texture    , bool, texture    )

  void setCallback(uint type, CallBackFunc func);
};

#endif
