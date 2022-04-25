#ifndef CGLShaderShape_H
#define CGLShaderShape_H

#include <glad/glad.h>
#include <vector>

class CGLShape {
 public:
  enum class Format {
    POINT,
    POINT_TCOORD
  };

 public:
  CGLShape() { }

  void addPoint(float x, float y, float z) {
    pointDataArray_.emplace_back(x, y, z);
  }

  void addPointTCoord(float x, float y, float z, float tx, float ty) {
    pointTCoordDataArray_.emplace_back(x, y, z, tx, ty);
  }

  void setBufferData() {
    if      (! pointDataArray_.empty())
      glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(pointDataArray_.size()*3*sizeof(float)),
                   &pointDataArray_[0], GL_STATIC_DRAW);
    else if (! pointTCoordDataArray_.empty())
      glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(pointTCoordDataArray_.size()*3*sizeof(float)),
                   &pointTCoordDataArray_[0], GL_STATIC_DRAW);
  }

 private:
  struct PointData {
    float x, y, z;

    PointData(float x, float y, float z) :
     x(x), y(y), z(z) {
    }
  };

  struct PointTCoordData {
    float x, y, z;
    float tx, ty;

    PointTCoordData(float x, float y, float z, float tx, float ty) :
     x(x), y(y), z(z), tx(tx), ty(ty) {
    }
  };

  using PointDataArray       = std::vector<PointData>;
  using PointTCoordDataArray = std::vector<PointTCoordData>;

  PointDataArray       pointDataArray_;
  PointTCoordDataArray pointTCoordDataArray_;
};

#endif
