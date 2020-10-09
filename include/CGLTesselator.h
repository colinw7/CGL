#ifndef CGL_TESSELATOR_H
#define CGL_TESSELATOR_H

#include <CPoint3D.h>
#include <accessor.h>
#include <vector>
#include <map>
#include <sys/types.h>

struct GLUTessVertex {
  CPoint3D  point;
  void     *data;

  GLUTessVertex(const CPoint3D &point1, void *data1) :
   point(point1), data(data1) {
  }
};

struct GLUTessTriangle {
  GLUTessVertex *vertex1;
  GLUTessVertex *vertex2;
  GLUTessVertex *vertex3;

  GLUTessTriangle(GLUTessVertex *v1, GLUTessVertex *v2, GLUTessVertex *v3) :
   vertex1(v1), vertex2(v2), vertex3(v3) {
  }
};

struct GLUTessContour {
 private:
  std::vector<GLUTessVertex *> vertices_;

 public:
  GLUTessContour() { }

 ~GLUTessContour() {
    std::vector<GLUTessVertex *>::iterator p1 = vertices_.begin();
    std::vector<GLUTessVertex *>::iterator p2 = vertices_.end  ();

    for ( ; p1 != p2; ++p1)
      delete *p1;
  }

  void addVertex(const CPoint3D &point, void *data) {
    vertices_.push_back(new GLUTessVertex(point, data));
  }

  const std::vector<GLUTessVertex *> &getVertices() const {
    return vertices_;
  }
};

class GLUTessPolygon {
 public:
  GLUTessPolygon(void *data) :
   data_(data), contour_(NULL) {
  }

 ~GLUTessPolygon() {
    std::vector<GLUTessContour *>::iterator p1 = contours_.begin();
    std::vector<GLUTessContour *>::iterator p2 = contours_.end  ();

    for ( ; p1 != p2; ++p1)
      delete *p1;
  }

  void *data() const { return data_; }

  void startContour() {
    assert(! contour_);

    contour_ = new GLUTessContour();

    contours_.push_back(contour_);
  }

  void endContour() {
    assert(contour_);

    contour_ = NULL;
  }

  void addVertex(const CPoint3D &point, void *data) {
    assert(contour_);

    contour_->addVertex(point, data);
  }

  std::vector< std::vector<GLUTessVertex *> > getVertices() const {
    std::vector< std::vector<GLUTessVertex *> > vertices;

    std::vector<GLUTessContour *>::const_iterator p1 = contours_.begin();
    std::vector<GLUTessContour *>::const_iterator p2 = contours_.end  ();

    for ( ; p1 != p2; ++p1) {
      std::vector<GLUTessVertex *> vertices1 = (*p1)->getVertices();

      vertices.push_back(vertices1);
    }

    return vertices;
  }

 private:
  void                          *data_;
  std::vector<GLUTessContour *>  contours_;
  GLUTessContour *               contour_;
};

class GLUtesselator {
 public:
  typedef void (*CallBackFunc)();

 private:
  uint                           winding_rule_;
  bool                           boundary_only_;
  double                         tolerance_;
  std::map<uint,CallBackFunc>    callbacks_;
  std::vector<GLUTessPolygon *>  polygons_;
  GLUTessPolygon                *polygon_;

 public:
  GLUtesselator();
 ~GLUtesselator();

  ACCESSOR(WindingRule , uint  , winding_rule)
  ACCESSOR(BoundaryOnly, bool  , boundary_only)
  ACCESSOR(Tolerance   , double, tolerance)

  void setCallback(uint type, CallBackFunc func);

  void startPolygon(void *data);
  void endPolygon();

  void startContour();
  void endContour();

  void addVertex(const CPoint3D &point, void *data);

  void triangulate(const std::vector<GLUTessVertex *> &vertices,
                   std::vector<GLUTessTriangle> &triangle_list) const;
};

#endif
