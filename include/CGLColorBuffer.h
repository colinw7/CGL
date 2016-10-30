#ifndef CGL_COLOR_BUFFER_H
#define CGL_COLOR_BUFFER_H

#include <CRGBA.h>
#include <CPoint3D.h>
#include <CVector3D.h>
#include <CILineDash.h>
#include <CMaterial.h>
#include <CImagePtr.h>
#include <accessor.h>
#include <vector>

class CGL;
class CGeomVertex3D;
class CGeomLight3D;
class CPixelRenderer;

class CGLColorBuffer {
 public:
  struct Point {
    // interpolated
    double    z;
    CRGBA     rgba;
    CVector3D normal;
    CPoint3D  tmap;

    uint      stencil_value;

    Point() :
     z(0), rgba(0,0,0), normal(0,0,0), tmap(0,0,0), stencil_value(0) {
    }

    Point(double z1, const CRGBA &rgba1, const CVector3D &normal1=CVector3D(0,0,0),
          const CPoint3D &tmap1=CPoint3D(0,0,0)) :
     z(z1), rgba(rgba1), normal(normal1), tmap(tmap1), stencil_value(0) {
    }

    const Point &operator+=(const Point &rhs) {
      z      += rhs.z;
      rgba   += rhs.rgba;
      normal += rhs.normal;
      tmap   += rhs.tmap;

      return *this;
    }

    const Point &operator-=(const Point &rhs) {
      z      -= rhs.z;
      rgba   -= rhs.rgba;
      normal -= rhs.normal;
      tmap   -= rhs.tmap;

      return *this;
    }
  };

 private:
  struct Line {
    Point *points;
  };

  //------

  CGL        *gl_;
  uint        width_;
  uint        height_;
  Line       *lines_;
  Point       def_point_;
  CRGBA       clear_color_;
  uint        clear_ind_;
  double      clear_depth_;
  bool        smooth_;
  uint        line_width_;
  CILineDash  line_dash_;
  bool        depth_writable_;
  uint        stencil_clear_value_;
  uint        stencil_mask_;

 public:
  CGLColorBuffer(CGL *gl, uint width=100, uint height=100);
 ~CGLColorBuffer();

  void resize(uint width, uint height);

  void clearColor();
  void clearDepth();
  void clearStencil();

  uint getWidth () const { return width_ ; }
  uint getHeight() const { return height_; }

  void setDefaultColor(const CRGBA &rgba) {
    def_point_.rgba = rgba;
  }

  const CRGBA &getDefaultColor() const {
    return def_point_.rgba;
  }

  void setDefaultNormal(const CVector3D &normal) {
    def_point_.normal = normal;
  }

  const CVector3D &getDefaultNormal() const {
    return def_point_.normal;
  }

  ACCESSOR(ClearColor, CRGBA , clear_color)
  ACCESSOR(ClearIndex, uint  , clear_ind  )
  ACCESSOR(ClearDepth, double, clear_depth)
  ACCESSOR(Smooth    , bool  , smooth     )

  ACCESSOR(LineWidth, uint      , line_width)
  ACCESSOR(LineDash , CILineDash, line_dash)

  ACCESSOR(StencilClearValue, uint, stencil_clear_value)
  ACCESSOR(StencilMask      , uint, stencil_mask)

  ACCESSOR(DepthWritable, bool, depth_writable)

  void fillPolygon(const std::vector<CGeomVertex3D *> &vertices);

  void drawLines(const std::vector<CGeomVertex3D *> &vertices);

  void drawLine(CGeomVertex3D *vertex1, CGeomVertex3D *vertex2);

  void drawPoints(const std::vector<CGeomVertex3D *> &vertices);

  void setPoint(uint x, uint y, const Point &point, double zv);

  void setColor(uint x, uint y, const CRGBA &rgba);

  const Point &getPoint(uint x, uint y) const;

  void setStencilValue(uint x, uint y, uint value);

  uint getStencilValue(uint x, uint y) const;

  void light(const CMaterial &material, const CRGBA &ambient,
             const std::vector<CGeomLight3D *> &lights);

  void render(CPixelRenderer *renderer);
};

#endif
