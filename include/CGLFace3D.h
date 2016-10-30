#ifndef CGL_FACE_3D_H
#define CGL_FACE_3D_H

#include <CGeomFace3D.h>

class CGLFace3D : public CGeomFace3D {
 private:
  CImagePtr stipple_;

 public:
  CGLFace3D() { }

  void setStipple(const CImagePtr &stipple) {
    stipple_ = stipple;
  }

  const CImagePtr &getStipple() const {
    return stipple_;
  }
};

#endif
