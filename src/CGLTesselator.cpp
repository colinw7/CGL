#include <CGLTesselator.h>
#include <CLine2D.h>
#include <CGL_glu.h>

class CGLTriangulate {
 private:
  struct EarPoint {
    GLUTessVertex *vertex;
    bool           is_ear;
    CPoint2D       point2;

    EarPoint(GLUTessVertex *v) :
     vertex(v), is_ear(false), point2(v->point.x, v->point.y) {
    }

    bool operator==(const EarPoint &ep) {
      return (vertex == ep.vertex);
    }
  };

  std::vector<GLUTessVertex *> vertices_;

 public:
  CGLTriangulate(const std::vector<GLUTessVertex *> &vertices);

  void exec(std::vector<GLUTessTriangle> &triangle_list);

 private:
  void addTriangle(std::vector<GLUTessTriangle> &triangle_list,
                   const std::list<EarPoint>::const_iterator &ep1,
                   const std::list<EarPoint>::const_iterator &ep2,
                   const std::list<EarPoint>::const_iterator &ep3);

  bool isDiagonal(const std::list<EarPoint> &ear_points,
                  const std::list<EarPoint>::const_iterator &epa,
                  const std::list<EarPoint>::const_iterator &epb);

  bool isDiagonalInOut(const std::list<EarPoint> &ear_points,
                       const std::list<EarPoint>::const_iterator &epa,
                       const std::list<EarPoint>::const_iterator &epb);

  bool inCone(const std::list<EarPoint> &ear_points,
              const std::list<EarPoint>::const_iterator &ep1,
              const std::list<EarPoint>::const_iterator &epb);

  bool Intersects(const std::list<EarPoint>::const_iterator &ep1,
                  const std::list<EarPoint>::const_iterator &ep2,
                  const std::list<EarPoint>::const_iterator &ep3,
                  const std::list<EarPoint>::const_iterator &ep4);

  bool PointLineLeftOn(const std::list<EarPoint>::const_iterator &ep1,
                       const std::list<EarPoint>::const_iterator &ep2,
                       const std::list<EarPoint>::const_iterator &ep3);

  bool PointLineLeft(const std::list<EarPoint>::const_iterator &ep1,
                     const std::list<EarPoint>::const_iterator &ep2,
                     const std::list<EarPoint>::const_iterator &ep3);
};

//-----------

GLUtesselator::
GLUtesselator() :
 winding_rule_(GLU_TESS_WINDING_ODD), boundary_only_(false),
 tolerance_(0.0), polygon_(NULL)
{
}

GLUtesselator::
~GLUtesselator()
{
  std::vector<GLUTessPolygon *>::iterator p1 = polygons_.begin();
  std::vector<GLUTessPolygon *>::iterator p2 = polygons_.end  ();

  for ( ; p1 != p2; ++p1)
    delete *p1;
}

void
GLUtesselator::
setCallback(uint type, CallBackFunc func)
{
  callbacks_[type] = func;
}

void
GLUtesselator::
startPolygon(void *data)
{
  assert(! polygon_);

  polygon_ = new GLUTessPolygon(data);

  polygons_.push_back(polygon_);
}

void
GLUtesselator::
endPolygon()
{
  assert(polygon_);

  //------

  std::vector< std::vector<GLUTessVertex *> > vertices = polygon_->getVertices();

  std::vector< std::vector<GLUTessVertex *> >::iterator p1 = vertices.begin();
  std::vector< std::vector<GLUTessVertex *> >::iterator p2 = vertices.end  ();

  for ( ; p1 != p2; ++p1) {
    (*((void (*)(uint)) callbacks_[GLU_TESS_BEGIN]))(GL_LINE_LOOP);

    std::vector<GLUTessTriangle> triangles;

    triangulate(*p1, triangles);

    std::vector<GLUTessTriangle>::iterator pt1 = triangles.begin();
    std::vector<GLUTessTriangle>::iterator pt2 = triangles.end  ();

    for ( ; pt1 != pt2; ++pt1) {
      (*((void (*)(void *)) callbacks_[GLU_TESS_VERTEX]))((*pt1).vertex1->data);
      (*((void (*)(void *)) callbacks_[GLU_TESS_VERTEX]))((*pt1).vertex2->data);
      (*((void (*)(void *)) callbacks_[GLU_TESS_VERTEX]))((*pt1).vertex3->data);
    }

    (*callbacks_[GLU_TESS_END])();
  }

  //------

  polygon_ = NULL;
}

void
GLUtesselator::
startContour()
{
  assert(! polygons_.empty());

  GLUTessPolygon *polygon = polygons_.back();

  polygon->startContour();
}

void
GLUtesselator::
endContour()
{
  assert(! polygons_.empty());

  GLUTessPolygon *polygon = polygons_.back();

  polygon->endContour();
}

void
GLUtesselator::
addVertex(const CPoint3D &point, void *data)
{
  assert(! polygons_.empty());

  GLUTessPolygon *polygon = polygons_.back();

  polygon->addVertex(point, data);
}

void
GLUtesselator::
triangulate(const std::vector<GLUTessVertex *> &vertices,
            std::vector<GLUTessTriangle> &triangle_list) const
{
  CGLTriangulate triangulate(vertices);

  triangulate.exec(triangle_list);
}

//-----------

CGLTriangulate::
CGLTriangulate(const std::vector<GLUTessVertex *> &vertices) :
 vertices_(vertices)
{
}

void
CGLTriangulate::
exec(std::vector<GLUTessTriangle> &triangle_list)
{
  std::list<EarPoint> ear_points;

  std::vector<GLUTessVertex *>::const_iterator ps = vertices_.begin();
  std::vector<GLUTessVertex *>::const_iterator pe = vertices_.end  ();

  for ( ; ps != pe; ++ps)
    ear_points.push_back(EarPoint(*ps));

  std::list<EarPoint>::iterator eps = ear_points.begin();
  std::list<EarPoint>::iterator epe = ear_points.end  ();

  std::list<EarPoint>::iterator ep3 = eps;
  std::list<EarPoint>::iterator ep1 = ep3++;
  std::list<EarPoint>::iterator ep2 = ep3++;

  std::list<EarPoint>::iterator ep4, ep5;

  do {
    (*ep2).is_ear = isDiagonal(ear_points, ep1, ep3);

    ep1 = ep2;
    ep2 = ep3++;

    if (ep3 == epe) ep3 = eps;
  } while (ep1 != eps);

  while (ear_points.size() > 3) {
    ep5 = eps;
    ep1 = ep5++;
    ep2 = ep5++;
    ep3 = ep5++;
    ep4 = ep5++;

    if (ep5 == epe) ep5 = eps;

    do {
      if ((*ep3).is_ear) {
        addTriangle(triangle_list, ep3, ep2, ep4);

        (*ep2).is_ear = isDiagonal(ear_points, ep1, ep4);
        (*ep4).is_ear = isDiagonal(ear_points, ep2, ep5);

        ear_points.erase(ep3);

        eps = ear_points.begin();
        epe = ear_points.end  ();

        break;
      }

      ep1 = ep2;
      ep2 = ep3;
      ep3 = ep4;
      ep4 = ep5++;

      if (ep5 == epe) ep5 = eps;
    } while (ep1 != eps);
  }

  if (ear_points.size() >= 3) {
    ep3 = eps;
    ep1 = ep3++;
    ep2 = ep3++;

    addTriangle(triangle_list, ep1, ep2, ep3);
  }
}

void
CGLTriangulate::
addTriangle(std::vector<GLUTessTriangle> &triangle_list,
            const std::list<EarPoint>::const_iterator &ep1,
            const std::list<EarPoint>::const_iterator &ep2,
            const std::list<EarPoint>::const_iterator &ep3)
{
  std::cout << "Diagonal " << (*ep2).vertex->point << "->" <<
                              (*ep3).vertex->point << std::endl;

  triangle_list.push_back(
    GLUTessTriangle((*ep1).vertex, (*ep2).vertex, (*ep3).vertex));
}

bool
CGLTriangulate::
isDiagonal(const std::list<EarPoint> &ear_points,
           const std::list<EarPoint>::const_iterator &epa,
           const std::list<EarPoint>::const_iterator &epb)
{
  return inCone(ear_points, epa, epb) &&
         inCone(ear_points, epb, epa) &&
         isDiagonalInOut(ear_points, epa, epb);
}

bool
CGLTriangulate::
isDiagonalInOut(const std::list<EarPoint> &ear_points,
                const std::list<EarPoint>::const_iterator &epa,
                const std::list<EarPoint>::const_iterator &epb)
{
  std::list<EarPoint>::const_iterator eps = ear_points.begin();
  std::list<EarPoint>::const_iterator epe = ear_points.end  ();

  std::list<EarPoint>::const_iterator ep2 = eps;
  std::list<EarPoint>::const_iterator ep1 = ep2++;

  do {
    if (ep1 != epa && ep2 != epa && ep1 != epb && ep2 != epb) {
      if (Intersects(epa, epb, ep1, ep2))
        return false;
    }

    ep1 = ep2++;

    if (ep2 == epe) ep2 = eps;
  } while (ep1 != eps);

  return true;
}

bool
CGLTriangulate::
inCone(const std::list<EarPoint> &ear_points,
       const std::list<EarPoint>::const_iterator &ep1,
       const std::list<EarPoint>::const_iterator &epb)
{
  std::list<EarPoint>::const_iterator eps = ear_points.begin();
  std::list<EarPoint>::const_iterator epe = ear_points.end();

  std::list<EarPoint>::const_iterator ep0 = ep1; --ep0;
  std::list<EarPoint>::const_iterator ep2 = ep1; ++ep2;

  if (ep0 == epe) ep0 = (++ear_points.rbegin()).base(); // last element
  if (ep2 == epe) ep2 = eps;

  if (PointLineLeftOn(ep1, ep2, ep0))
    return    PointLineLeft(ep1, epb, ep0) && PointLineLeft(epb, ep1, ep2);
  else
    return ! (PointLineLeft(ep1, epb, ep2) && PointLineLeft(epb, ep1, ep0));
}

bool
CGLTriangulate::
Intersects(const std::list<EarPoint>::const_iterator &ep1,
           const std::list<EarPoint>::const_iterator &ep2,
           const std::list<EarPoint>::const_iterator &ep3,
           const std::list<EarPoint>::const_iterator &ep4)
{
  CLine2D line1((*ep1).point2, (*ep2).point2);
  CLine2D line2((*ep3).point2, (*ep4).point2);

  return line1.intersects(line2);
}

bool
CGLTriangulate::
PointLineLeftOn(const std::list<EarPoint>::const_iterator &ep1,
                const std::list<EarPoint>::const_iterator &ep2,
                const std::list<EarPoint>::const_iterator &ep3)
{
  CLine2D line((*ep1).point2, (*ep2).point2);

  return line.leftOrOn((*ep3).point2);
}

bool
CGLTriangulate::
PointLineLeft(const std::list<EarPoint>::const_iterator &ep1,
              const std::list<EarPoint>::const_iterator &ep2,
              const std::list<EarPoint>::const_iterator &ep3)
{
  CLine2D line((*ep1).point2, (*ep2).point2);

  return line.left((*ep3).point2);
}
