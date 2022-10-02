#include <CGLColorBuffer.h>
#include <CGL.h>
#include <CGeomVertex3D.h>
#include <CGeomLight3D.h>
#include <CPixelRenderer.h>
#include <CMathRound.h>

CGLColorBuffer::
CGLColorBuffer(CGL *gl, uint width, uint height) :
 gl_(gl), width_(0), height_(0), lines_(0), clear_color_(0,0,0,1),
 clear_ind_(0), clear_depth_(100), smooth_(false), line_width_(0),
 depth_writable_(true), stencil_clear_value_(0), stencil_mask_(0xFF)
{
  resize(width, height);
}

CGLColorBuffer::
~CGLColorBuffer()
{
  resize(0, 0);
}

void
CGLColorBuffer::
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
CGLColorBuffer::
clearColor()
{
  for (uint y = 0; y < height_; ++y) {
    Line *line = &lines_[y];

    for (uint x = 0; x < width_; ++x) {
      Point *point = &line->points[x];

      point->rgba = clear_color_;
    }
  }
}

void
CGLColorBuffer::
clearDepth()
{
  for (uint y = 0; y < height_; ++y) {
    Line *line = &lines_[y];

    for (uint x = 0; x < width_; ++x) {
      Point *point = &line->points[x];

      point->z = clear_depth_;
    }
  }
}

void
CGLColorBuffer::
clearStencil()
{
  for (uint y = 0; y < height_; ++y) {
    Line *line = &lines_[y];

    for (uint x = 0; x < width_; ++x) {
      Point *point = &line->points[x];

      point->stencil_value = stencil_clear_value_;
    }
  }
}

void
CGLColorBuffer::
fillPolygon(const std::vector<CGeomVertex3D *> &vertices)
{
  int w = int(getWidth ());
  int h = int(getHeight());

  auto num_vertices = vertices.size();

  // get y limits
  const CPoint3D &pixel = vertices[0]->getPixel();

  int ypmin = CMathRound::RoundUp  (pixel.y);
  int ypmax = CMathRound::RoundDown(pixel.y);

  for (uint i1 = 1; i1 < num_vertices; ++i1) {
    const CPoint3D &vpixel = vertices[i1]->getPixel();

    ypmin = std::min(ypmin, CMathRound::RoundUp  (vpixel.y));
    ypmax = std::max(ypmax, CMathRound::RoundDown(vpixel.y));
  }

  ypmin = std::max(ypmin, 0);
  ypmax = std::min(ypmax, h - 1);

  //------

  double zvmin, zvmax, zpmin, zpmax;
  int    i1min, i2min, i1max, i2max;

  CPoint3D tmin(0,0,0);
  CPoint3D tmax = tmin;

  CRGBA rgba_min = getDefaultColor();
  CRGBA rgba_max = rgba_min;

  CVector3D normal_min = getDefaultNormal();
  CVector3D normal_max = normal_min;

  Point point;

  // rasterize polygon for each value of y in limits
  for (int yp = ypmin; yp <= ypmax; ++yp) {
    bool set = false;

    int xpmin, xpmax;

    double xp;

    // for each polygon line find x range for this y
    for (uint i1 = uint(num_vertices - 1), i2 = 0; i2 < num_vertices; i1 = i2, ++i2) {
      const CPoint3D &pixel1 = vertices[i1]->getPixel();
      const CPoint3D &pixel2 = vertices[i2]->getPixel();

      // skip line if not in y range or zero length line
      if ((pixel1.y < yp && pixel2.y < yp) ||
          (pixel1.y > yp && pixel2.y > yp) ||
          fabs(pixel2.y - pixel1.y) < 1E-6)
        continue;

      // get x intersect
      double fx = (pixel2.x - pixel1.x)/(pixel2.y - pixel1.y);

      xp = (yp - pixel1.y)*fx + pixel1.x;

      // update min and/or max
      if (! set) {
        xpmin = CMathRound::RoundUp  (xp); i1min = int(i1); i2min = int(i2);
        xpmax = CMathRound::RoundDown(xp); i1max = int(i1); i2max = int(i2);

        set = true;
      }
      else {
        int ixp1 = CMathRound::RoundUp  (xp);
        int ixp2 = CMathRound::RoundDown(xp);

        if (ixp1 < xpmin) {
          xpmin = ixp1; i1min = int(i1); i2min = int(i2);
        }

        if (ixp2 > xpmax) {
          xpmax = ixp2; i1max = int(i1); i2max = int(i2);
        }
      }
    }

    //-------

    if (! set)
      continue;

    //-------

    double d, i, id;

    // get relative intersect of y value on min line
    const CPoint3D &pixel1 = vertices[uint(i1min)]->getPixel();
    const CPoint3D &pixel2 = vertices[uint(i2min)]->getPixel();

    if (fabs(pixel2.x - pixel1.x) > fabs(pixel2.y - pixel1.y)) {
      d = xpmin - pixel1.x;
      i = 1.0/(pixel2.x - pixel1.x);
    }
    else {
      d = yp - pixel1.y;
      i = 1.0/(pixel2.y - pixel1.y);
    }

    id = i*d;

    // interp z at minimum
    const CPoint3D &viewed1 = vertices[uint(i1min)]->getViewed();
    const CPoint3D &viewed2 = vertices[uint(i2min)]->getViewed();

    zpmin = (pixel2 .z - pixel1 .z)*id + pixel1 .z;
    zvmin = (viewed2.z - viewed1.z)*id + viewed1.z;

    // interp color at minimum (if required)
    if (! gl_->getFlatShading()) {
      const CRGBA &rgba1 = vertices[uint(i1min)]->getColor();
      const CRGBA &rgba2 = vertices[uint(i2min)]->getColor();

      rgba_min = (rgba2 - rgba1)*id + rgba1;
    }

    // interp normal at maximum (if required)
    if (getSmooth()) {
      const CVector3D &normal1 = vertices[uint(i1min)]->getNormal();
      const CVector3D &normal2 = vertices[uint(i2min)]->getNormal();

      normal_min = (normal2 - normal1)*id + normal1;
    }

    // interp texture map at minimum (if required)
    if (gl_->getTexture1Data().getEnabled() ||
        gl_->getTexture2Data().getEnabled() ||
        gl_->getTexture3Data().getEnabled()) {
      const CPoint3D &point1 = vertices[uint(i1min)]->getTextureMap();
      const CPoint3D &point2 = vertices[uint(i2min)]->getTextureMap();

      tmin.x = (point2.x - point1.x)*id + point1.x;
      tmin.y = (point2.y - point1.y)*id + point1.y;
      tmin.z = (point2.z - point1.z)*id + point1.z;
    }

    //-----------------

    // get relative intersect of y value on max line
    const CPoint3D &pixel3 = vertices[uint(i1max)]->getPixel();
    const CPoint3D &pixel4 = vertices[uint(i2max)]->getPixel();

    if (fabs(pixel4.x - pixel3.x) > fabs(pixel4.y - pixel3.y)) {
      d = xpmax - pixel3.x;
      i = 1.0/(pixel4.x - pixel3.x);
    }
    else {
      d = yp - pixel3.y;
      i = 1.0/(pixel4.y - pixel3.y);
    }

    id = i*d;

    // interp z at maximum
    const CPoint3D &viewed3 = vertices[uint(i1max)]->getViewed();
    const CPoint3D &viewed4 = vertices[uint(i2max)]->getViewed();

    zpmax = (pixel4 .z - pixel3 .z)*id + pixel3 .z;
    zvmax = (viewed4.z - viewed3.z)*id + viewed3.z;

    // interp color at maximum (if required)
    if (! gl_->getFlatShading()) {
      const CRGBA &rgba1 = vertices[uint(i1max)]->getColor();
      const CRGBA &rgba2 = vertices[uint(i2max)]->getColor();

      rgba_max = (rgba2 - rgba1)*id + rgba1;
    }

    // interp normal at maximum (if required)
    if (getSmooth()) {
      const CVector3D &normal1 = vertices[uint(i1max)]->getNormal();
      const CVector3D &normal2 = vertices[uint(i2max)]->getNormal();

      normal_max = (normal2 - normal1)*id + normal1;
    }

    // interp texture map at maximum (if required)
    if (gl_->getTexture1Data().getEnabled() ||
        gl_->getTexture2Data().getEnabled() ||
        gl_->getTexture3Data().getEnabled()) {
      const CPoint3D &point1 = vertices[uint(i1max)]->getTextureMap();
      const CPoint3D &point2 = vertices[uint(i2max)]->getTextureMap();

      tmax.x = (point2.x - point1.x)*id + point1.x;
      tmax.y = (point2.y - point1.y)*id + point1.y;
      tmax.z = (point2.z - point1.z)*id + point1.z;
    }

    //-------

    double    dzp = 0;
    double    dzv = 0;
    CRGBA     drgba;
    CVector3D dnormal;
    CPoint3D  dt;

    double ixpd = 1.0/(xpmax - xpmin);

    if (xpmin < xpmax) {
      dzp = (zpmax - zpmin)*ixpd;
      dzv = (zvmax - zvmin)*ixpd;

      if (! gl_->getFlatShading())
        drgba = (rgba_max - rgba_min)*ixpd;

      if (getSmooth())
        dnormal = (normal_max - normal_min)*ixpd;

      if (gl_->getTexture1Data().getEnabled() ||
          gl_->getTexture2Data().getEnabled() ||
          gl_->getTexture3Data().getEnabled())
        dt = (tmax - tmin)*ixpd;
    }

    double zv = zvmin;

    point.z      = zpmin;
    point.rgba   = rgba_min;
    point.normal = normal_min;
    point.tmap   = tmin;

    if (xpmax < 0)
      continue;

    if (xpmin < 0) {
      int dx = -xpmin;

      if (! gl_->getFlatShading())
        point.rgba += dx*drgba;

      if (getSmooth())
        point.normal += dx*dnormal;

      if (gl_->getTexture1Data().getEnabled() ||
          gl_->getTexture2Data().getEnabled() ||
          gl_->getTexture3Data().getEnabled())
        point.tmap += dx*dt;

      xpmin = 0;
    }

    for (int xp1 = xpmin; xp1 <= xpmax && xp1 < w; ++xp1, point.z += dzp, zv += dzv) {
      if (xp1 >= 0)
        setPoint(uint(xp1), uint(yp), point, zv);

      if (! gl_->getFlatShading())
        point.rgba += drgba;

      if (getSmooth())
        point.normal += dnormal;

      if (gl_->getTexture1Data().getEnabled() ||
          gl_->getTexture2Data().getEnabled() ||
          gl_->getTexture3Data().getEnabled())
        point.tmap += dt;
    }
  }
}

void
CGLColorBuffer::
drawLines(const std::vector<CGeomVertex3D *> &vertices)
{
  auto num_vertices = vertices.size();

  CGeomVertex3D *vertex1 = vertices[num_vertices - 1];
  CGeomVertex3D *vertex2 = NULL;

  for (uint i = 0; i < num_vertices; ++i, vertex1 = vertex2) {
    vertex2 = vertices[i];

    drawLine(vertex1, vertex2);
  }
}

void
CGLColorBuffer::
drawLine(CGeomVertex3D *vertex1, CGeomVertex3D *vertex2)
{
  Point point, dpoint;

  const CPoint3D &pixel1 = vertex1->getPixel();
  const CPoint3D &pixel2 = vertex2->getPixel();

  const CPoint3D &viewed1 = vertex1->getViewed();
  const CPoint3D &viewed2 = vertex2->getViewed();

  int x1 = int(pixel1.x); int y1 = int(pixel1.y);
  int x2 = int(pixel2.x); int y2 = int(pixel2.y);

  double           zp1   = pixel1.z;
  double           zv1   = viewed1.z;
  CRGBA            rgba1 = vertex1->getColor();
  const CVector3D &norm1 = vertex1->getNormalSet() ?
                           vertex1->getNormal() :
                           CGLMgrInst->getDefNormal();
  const CPoint3D  &tmap1 = vertex1->getTextureMap();

  double           zp2   = pixel2.z;
  double           zv2   = viewed2.z;
  CRGBA            rgba2 = vertex2->getColor();
  const CVector3D &norm2 = vertex2->getNormalSet() ?
                           vertex2->getNormal() :
                           CGLMgrInst->getDefNormal();
  const CPoint3D  &tmap2 = vertex2->getTextureMap();

  //-----

  if (gl_->getFlatShading())
    rgba1 = rgba2;

  //-----

  double zv = zv1;

  point.z      = zp1;
  point.rgba   = rgba1;
  point.normal = norm1;
  point.tmap   = tmap1;

  //-----

  uint line_width = line_width_;

  if (line_width <= 0)
    line_width = 1;

  int line_width1 = int(line_width >> 1);
  int line_width2 = int(line_width) - line_width1;

  int dx = x2 - x1;

  // vertical line
  if (dx == 0) {
    double iy21 = 1.0/(y2 - y1);

    double dzp = (zv2 - zv1)*iy21;

    dpoint.z      = (zp2   - zp1  )*iy21;
    dpoint.rgba   = (rgba2 - rgba1)*iy21;
    dpoint.normal = (norm2 - norm1)*iy21;
    dpoint.tmap   = (tmap2 - tmap1)*iy21;

    if (y2 > y1) {
      for (int y = y1; y <= y2; ++y, point += dpoint, zv += dzp) {
        if (line_dash_.isDraw())
          setPoint(uint(x1), uint(y), point, zv);

        line_dash_.step(1);
      }
    }
    else {
      for (int y = y1; y >= y2; --y, point += dpoint, zv += dzp) {
        if (line_dash_.isDraw())
          setPoint(uint(x1), uint(y), point, zv);

        line_dash_.step(1);
      }
    }

    return;
  }

  //-----

  int dy = y2 - y1;

  // horizontal line
  if (dy == 0) {
    double ix21 = 1.0/(x2 - x1);

    double dzp = (zv2 - zv1)*ix21;

    dpoint.z      = (zp2   - zp1  )*ix21;
    dpoint.rgba   = (rgba2 - rgba1)*ix21;
    dpoint.normal = (norm2 - norm1)*ix21;
    dpoint.tmap   = (tmap2 - tmap1)*ix21;

    if (x2 > x1) {
      for (int x = x1; x <= x2; ++x, point += dpoint, zv += dzp) {
        if (line_dash_.isDraw()) {
          for (int i = -line_width1; i < line_width2; ++i)
            setPoint(uint(x), uint(y1 + i), point, zv);
        }

        line_dash_.step(1);
      }
    }
    else {
      for (int x = x1; x >= x2; --x, point += dpoint, zv += dzp) {
        if (line_dash_.isDraw()) {
          for (int i = -line_width1; i < line_width2; ++i)
            setPoint(uint(x), uint(y1 + i), point, zv);
        }

        line_dash_.step(1);
      }
    }

    return;
  }

  //-----

  int adx = abs(dx);
  int ady = abs(dy);

  int eps = 0;

  if (adx > ady) {
    double ix21 = 1.0/(x2 - x1);

    double dzp = (zv2 - zv1)*ix21;

    dpoint.z      = (zp2   - zp1  )*ix21;
    dpoint.rgba   = (rgba2 - rgba1)*ix21;
    dpoint.normal = (norm2 - norm1)*ix21;
    dpoint.tmap   = (tmap2 - tmap1)*ix21;

    //---

    int y = y1;

    if (dx > 0) {
      if (dy > 0) {
        for (int x = x1; x <= x2; ++x, point += dpoint, zv += dzp) {
          if (line_dash_.isDraw())
            setPoint(uint(x), uint(y), point, zv);

          line_dash_.step(1);

          eps += ady;

          if ((eps << 1) >= int(adx)) {
            ++y;

            eps -= adx;
          }
        }
      }
      else {
        for (int x = x1; x <= x2; ++x, point += dpoint, zv += dzp) {
          if (line_dash_.isDraw())
            setPoint(uint(x), uint(y), point, zv);

          line_dash_.step(1);

          eps += ady;

          if ((eps << 1) >= int(adx)) {
            --y;

            eps -= adx;
          }
        }
      }
    }
    else {
      if (dy > 0) {
        for (int x = x1; x >= x2; --x, point += dpoint, zv += dzp) {
          if (line_dash_.isDraw())
            setPoint(uint(x), uint(y), point, zv);

          line_dash_.step(1);

          eps += ady;

          if ((eps << 1) >= int(adx)) {
            ++y;

            eps -= adx;
          }
        }
      }
      else {
        for (int x = x1; x >= x2; --x, point += dpoint, zv += dzp) {
          if (line_dash_.isDraw())
            setPoint(uint(x), uint(y), point, zv);

          line_dash_.step(1);

          eps += ady;

          if ((eps << 1) >= int(adx)) {
            --y;

            eps -= adx;
          }
        }
      }
    }
  }
  else {
    double iy21 = 1.0/(y2 - y1);

    double dzp = (zv2 - zv1)*iy21;

    dpoint.z      = (zp2   - zp1  )*iy21;
    dpoint.rgba   = (rgba2 - rgba1)*iy21;
    dpoint.normal = (norm2 - norm1)*iy21;
    dpoint.tmap   = (tmap2 - tmap1)*iy21;

    //---

    int x = x1;

    if (dy > 0) {
      if (dx > 0) {
        for (int y = y1; y <= y2; ++y, point += dpoint, zv += dzp) {
          if (line_dash_.isDraw())
            setPoint(uint(x), uint(y), point, zv);

          line_dash_.step(1);

          eps += adx;

          if ((eps << 1) >= int(ady)) {
            ++x;

            eps -= ady;
          }
        }
      }
      else {
        for (int y = y1; y <= y2; ++y, point += dpoint, zv += dzp) {
          if (line_dash_.isDraw())
            setPoint(uint(x), uint(y), point, zv);

          line_dash_.step(1);

          eps += adx;

          if ((eps << 1) >= int(ady)) {
            --x;

            eps -= ady;
          }
        }
      }
    }
    else {
      if (dx > 0) {
        for (int y = y1; y >= y2; --y, point += dpoint, zv += dzp) {
          if (line_dash_.isDraw())
            setPoint(uint(x), uint(y), point, zv);

          line_dash_.step(1);

          eps += adx;

          if ((eps << 1) >= int(ady)) {
            ++x;

            eps -= ady;
          }
        }
      }
      else {
        for (int y = y1; y >= y2; --y, point += dpoint, zv += dzp) {
          if (line_dash_.isDraw())
            setPoint(uint(x), uint(y), point, zv);

          line_dash_.step(1);

          eps += adx;

          if ((eps << 1) >= int(ady)) {
            --x;

            eps -= ady;
          }
        }
      }
    }
  }
}

void
CGLColorBuffer::
drawPoints(const std::vector<CGeomVertex3D *> &vertices)
{
  Point point;

  auto num_vertices = vertices.size();

  for (uint i = 0; i < num_vertices; ++i) {
    const CGeomVertex3D *vertex = vertices[i];

    const CPoint3D &pixel = vertex->getPixel();

    const CPoint3D &viewed = vertex->getViewed();

    int x = int(pixel.x); int y = int(pixel.y);

    point.z      = pixel.z;
    point.rgba   = vertex->getColor();
    point.normal = vertex->getNormal();
    point.tmap   = vertex->getTextureMap();

    setPoint(uint(x), uint(y), point, viewed.z);
  }
}

void
CGLColorBuffer::
setPoint(uint x, uint y, const Point &point, double zv)
{
  if (x >= width_ || y >= height_) return;

  y = height_ - 1 - y;

  //------

  Line  *line   = &lines_[y];
  Point *ppoint = &line->points[x];

  //------

  // Scissor Test
  if (gl_->getScissor().getEnabled()) {
    if (! gl_->getScissor().getRect().inside(CIPoint2D(int(x), int(y))))
      return;
  }

  //------

  // Alpha Test
  if (gl_->getAlphaTest().getEnabled()) {
    if (! gl_->getAlphaTest().eval(point.rgba.getAlpha()))
      return;
  }

  //------

  // Stencil Test
  if (gl_->getStencil().getEnabled()) {
    uint new_value;

    if (gl_->getStencil().testValue(ppoint->stencil_value)) {
      bool zfail = false;

      if (gl_->getDepthTest().getEnabled()) {
        if (! gl_->getDepthTest().eval(ppoint->z, point.z))
          zfail = true;
      }

      if (zfail)
        new_value = gl_->getStencil().zfailValue(ppoint->stencil_value);
      else
        new_value = gl_->getStencil().zpassValue(ppoint->stencil_value);

      ppoint->stencil_value = new_value;
    }
    else {
      new_value = gl_->getStencil().failValue(ppoint->stencil_value);

      ppoint->stencil_value = new_value;

      return;
    }
  }

  //------

  if (gl_->getDepthTest().getEnabled()) {
    // if (point.z < 0) return;

    if (! gl_->getDepthTest().eval(ppoint->z, point.z))
      return;
  }

  //------

  if (gl_->getPolyStipple()) {
    CImagePtr stipple = gl_->getPolyStippleImage();

    int w = int(stipple->getWidth ());
    int h = int(stipple->getHeight());

    int y1 = int(y) % h;
    int x1 = int(x) % w;

    double gray;

    stipple->getGrayPixel(x1, y1, &gray);

    if (gray <= 0.5)
      return;
  }

  //------

  if (depth_writable_) {
    ppoint->z      = point.z;
    ppoint->normal = point.normal;
    ppoint->tmap   = point.tmap;
  }

  if (gl_->getBlending())
    ppoint->rgba = CRGBA::modeCombine(point.rgba, ppoint->rgba,
                                      gl_->getBlendSrcType(),
                                      gl_->getBlendDstType(),
                                      gl_->getBlendFunc()).clamp();
  else
    ppoint->rgba = point.rgba;

  //------

  if (gl_->getFogEnabled()) {
    double z = fabs(zv);

    const CFogData &fog_data = gl_->getFogData();

    double f = 1.0;

    if      (fog_data.type == CFOG_TYPE_EXP) {
      double dz = fog_data.density*z;

      f = exp(-dz);
    }
    else if (fog_data.type == CFOG_TYPE_EXP2) {
      double dz = fog_data.density*z;

      f = exp(-(dz*dz));
    }
    else if (fog_data.type == CFOG_TYPE_LINEAR)
      f = (fog_data.end - z)/(fog_data.end - fog_data.start);

    f = std::min(std::max(f, 0.0), 1.0);

    ppoint->rgba = (ppoint->rgba*f + (fog_data.color)*(1 - f)).clamp();
  }

  //------

  if (gl_->getTexture1Data().getEnabled()) {
    uint ind    = gl_->getTexture1Data().getCurrentInd();
    uint level1 = 0;
    uint level2 = gl_->modifyTexture1Data().getTextureDepth(ind);

    uint level = std::min(std::max(uint(point.z*level2), level1), level2);

    CGLTextureKey key(ind, level);

    const CImagePtr &texture = gl_->modifyTexture1Data().getTexture(key);

    int w = int(texture->getWidth());

    int tx = std::min(std::max(int(point.tmap.x*(w - 1)), 0), w - 1);

    texture->getRGBAPixel(tx, 0, ppoint->rgba);
  }

  if (gl_->getTexture2Data().getEnabled()) {
    uint ind    = gl_->getTexture2Data().getCurrentInd();
    uint level1 = 0;
    uint level2 = gl_->modifyTexture2Data().getTextureDepth(ind);

    uint level = std::min(std::max(uint(point.z*level2), level1), level2);

    level = 0;

    CGLTextureKey key(ind, level);

    const CImagePtr &texture = gl_->modifyTexture2Data().getTextureImage(key);

     if (texture.isValid() && texture->isValid()) {
      int w = int(texture->getWidth ());
      int h = int(texture->getHeight());

      uint wrap_s = gl_->getTexture2Data().getWrapS();

      double tx { 0.0 };

      if      (wrap_s == GL_CLAMP)
        tx = point.tmap.x;
      else if (wrap_s == GL_CLAMP_TO_EDGE) {
        double in = 0.5/w;

        tx = std::min(std::max(point.tmap.x, in), 1 - in);
      }
      else if (wrap_s == GL_REPEAT) {
        tx = point.tmap.x - int(point.tmap.x);

        if (tx < 0) tx = 1 + tx;
      }

      int xx = std::min(std::max(int(tx*(w - 1)), 0), w - 1);

      uint wrap_t = gl_->getTexture2Data().getWrapT();

      double ty { 0.0 };

      if      (wrap_t == GL_CLAMP)
        ty = point.tmap.y;
      else if (wrap_t == GL_CLAMP_TO_EDGE) {
        double in = 0.5/h;

        ty = std::min(std::max(point.tmap.y, in), 1 - in);
      }
      else if (wrap_t == GL_REPEAT) {
        ty = point.tmap.y - int(point.tmap.y);

        if (ty < 0) ty = 1 + ty;
      }

      int yy = std::min(std::max(int(ty*(h - 1)), 0), h - 1);

      CRGBA rgbat;

      texture->getRGBAPixel(xx, yy, rgbat);

      CRGBA rgbaf = ppoint->rgba;

      int format = gl_->modifyTexture2Data().getTextureFormat(key);
      int mode   = int(gl_->getTextureEnv().getMode());

      switch (mode) {
        case GL_MODULATE: {
          double lumt   = rgbat.getGray();
          double intt   = rgbat.getIntensity();
          double alphaf = rgbaf.getAlpha();
          double alphat = rgbat.getAlpha();

          switch (format) {
            case GL_ALPHA:
              ppoint->rgba = rgbaf;
              ppoint->rgba.setAlpha(alphaf*alphat);
              break;
            case GL_LUMINANCE:
              ppoint->rgba = lumt*rgbaf;
              ppoint->rgba.setAlpha(alphaf);
              break;
            case GL_LUMINANCE_ALPHA:
              ppoint->rgba = lumt*rgbaf;
              ppoint->rgba.setAlpha(alphat*alphaf);
              break;
            case GL_INTENSITY:
              ppoint->rgba = intt*rgbaf;
              ppoint->rgba.setAlpha(intt*alphaf);
              break;
            case GL_RGB:
              ppoint->rgba = rgbat*rgbaf;
              ppoint->rgba.setAlpha(alphaf);
              break;
            case GL_RGBA:
            default:
              ppoint->rgba = rgbat*rgbaf;
              ppoint->rgba.setAlpha(alphat*alphaf);
              break;
          }
          break;
        }
        case GL_DECAL: {
          double alphaf = rgbaf.getAlpha();
          double alphat = rgbat.getAlpha();

          switch (format) {
            case GL_ALPHA:
            case GL_LUMINANCE:
            case GL_LUMINANCE_ALPHA:
            case GL_INTENSITY:
              ppoint->rgba = rgbat; // undefined
              break;
            case GL_RGB:
              ppoint->rgba = rgbat;
              ppoint->rgba.setAlpha(alphaf);
              break;
            case GL_RGBA:
            default:
              ppoint->rgba = (1 - alphat)*rgbaf + alphat*rgbat;
              ppoint->rgba.setAlpha(alphaf);
              break;
          }
          break;
        }
        case GL_BLEND: {
          double lumt   = rgbat.getGray();
          double intt   = rgbat.getIntensity();
          double alphaf = rgbaf.getAlpha();
          double alphat = rgbat.getAlpha();

          CRGBA  rgbac  = gl_->getTextureEnv().getColor();
          double alphac = rgbac.getAlpha();

          switch (format) {
            case GL_ALPHA:
              ppoint->rgba = rgbaf;
              ppoint->rgba.setAlpha(alphat*alphaf);
              break;
            case GL_LUMINANCE: {
              ppoint->rgba = (1 - lumt)*rgbaf + lumt*rgbac;
              ppoint->rgba.setAlpha(alphat*alphaf);
              break;
            }
            case GL_LUMINANCE_ALPHA:
              ppoint->rgba = (1 - lumt)*rgbaf + lumt*rgbac;
              ppoint->rgba.setAlpha(alphat*alphaf);
              break;
            case GL_INTENSITY:
              ppoint->rgba = (1 - intt)*rgbaf + intt*rgbac;
              ppoint->rgba.setAlpha((1 - intt)*alphaf + intt*alphac);
              break;
            case GL_RGB:
              ppoint->rgba = rgbat.inverse()*rgbaf + rgbat*rgbac;
              ppoint->rgba.setAlpha(alphaf);
              break;
            case GL_RGBA:
            default:
              ppoint->rgba = rgbat.inverse()*rgbaf + rgbat*rgbac;
              ppoint->rgba.setAlpha(alphat*alphaf);
              break;
          }
          break;
        }
        case GL_REPLACE:
        default: {
          double lumt   = rgbat.getGray();
          double intt   = rgbat.getIntensity();
          double alphaf = rgbaf.getAlpha();
          double alphat = rgbat.getAlpha();

          switch (format) {
            case GL_ALPHA:
              ppoint->rgba = rgbaf;
              ppoint->rgba.setAlpha(alphat);
              break;
            case GL_LUMINANCE:
              ppoint->rgba.setGray(lumt);
              ppoint->rgba.setAlpha(alphaf);
              break;
            case GL_LUMINANCE_ALPHA:
              ppoint->rgba.setGray(lumt);
              ppoint->rgba.setAlpha(alphat);
              break;
            case GL_INTENSITY:
              ppoint->rgba.setGray(intt);
              ppoint->rgba.setAlpha(intt);
              break;
            case GL_RGB:
              ppoint->rgba = rgbat;
              ppoint->rgba.setAlpha(alphaf);
              break;
            case GL_RGBA:
            default:
              ppoint->rgba = rgbat;
              ppoint->rgba.setAlpha(alphat);
              break;
          }
          break;
        }
      }
    }
  }

  if (gl_->getTexture3Data().getEnabled()) {
    uint ind   = gl_->getTexture3Data().getCurrentInd();
    uint level = 0;

    CGLTextureKey key(ind, level);

    const auto &texture = gl_->modifyTexture3Data().getTexture(key);

    auto d = texture.size();

    if (d > 0) {
     if (texture[0].isValid() && texture[0]->isValid()) {
        int w = int(texture[0]->getWidth ());
        int h = int(texture[0]->getHeight());

        int x1 = std::min(std::max(int(point.tmap.x*   (w - 1)), 0),     w - 1 );
        int y1 = std::min(std::max(int(point.tmap.y*   (h - 1)), 0),     h - 1 );
        int z1 = std::min(std::max(int(point.tmap.z*int(d - 1)), 0), int(d - 1));

        texture[uint(z1)]->getRGBAPixel(x1, y1, ppoint->rgba);
      }
    }
  }
}

const CGLColorBuffer::Point &
CGLColorBuffer::
getPoint(uint x, uint y) const
{
  y = height_ - 1 - y;

  Line *line = &lines_[y];

  return line->points[x];
}

void
CGLColorBuffer::
setColor(uint x, uint y, const CRGBA &rgba)
{
  if (x >= width_ || y >= height_) return;

  y = height_ - 1 - y;

  //------

  Line  *line   = &lines_[y];
  Point *ppoint = &line->points[x];

  ppoint->rgba = rgba;
}

void
CGLColorBuffer::
light(const CMaterial &material, const CRGBA &ambient, const std::vector<CGeomLight3D *> &lights)
{
  CPoint3D gpoint;

  for (uint y = 0; y < height_; ++y) {
    Line *line = &lines_[y];

    gpoint.y = y;

    for (uint x = 0; x < width_; ++x) {
      Point *point = &line->points[x];

      gpoint.x = x;
      gpoint.z = -point->z;

      CRGBA rgba = material.getEmission();

      rgba += ambient*point->rgba;

      std::vector<CGeomLight3D *>::const_iterator plight1 = lights.begin();
      std::vector<CGeomLight3D *>::const_iterator plight2 = lights.end  ();

      for ( ; plight1 != plight2; ++plight1)
        (*plight1)->lightPoint(rgba, gpoint, point->normal, material);

      point->rgba = rgba;
    }
  }
}

void
CGLColorBuffer::
render(CPixelRenderer *renderer)
{
  for (uint y = 0; y < height_; ++y) {
    Line *line = &lines_[y];

    for (uint x = 0; x < width_; ++x) {
      Point *point = &line->points[x];

      renderer->setForeground(point->rgba);

      renderer->drawClippedPoint(CIPoint2D(int(x), int(y)));
    }
  }
}
