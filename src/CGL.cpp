#include <CGL.h>
#include <CGLDisplayList.h>
#include <CGLWind.h>
#include <CFuncs.h>
#include <COSTimer.h>
#include <CMathGen.h>
#include <CPoint3D.h>
#include <CTriangle3D.h>
#include <CXMachine.h>
#include <CXLibPixelRenderer.h>
#include <CImagePixelRenderer.h>
#include <CGeomUtil3D.h>
#include <CStrUtil.h>
#include <climits>

class CGLEventAdapter : public CXEventAdapter {
 public:
  bool idleEvent();
};

//-------------------

CGLMgr::
CGLMgr() :
 current_window_   (NULL),
 current_window_id_(0),
 window_rect_      (0,0,300,300),
 error_num_        (GL_NO_ERROR)
{
}

CGLMgr::
~CGLMgr()
{
  WindowMap::iterator p1 = window_map_.begin();
  WindowMap::iterator p2 = window_map_.end();

  for ( ; p1 != p2; ++p1)
    delete (*p1).second;
}

CGL *
CGLMgr::
getCurrentGL() const
{
  if (current_window_)
    return current_window_->getGL();
  else
    return NULL;
}

void
CGLMgr::
setWindowPosition(const CIPoint2D &point)
{
  window_rect_.setPosition(point);

  if (current_window_)
    current_window_->setPosition(point);
}

CIPoint2D
CGLMgr::
getWindowPosition() const
{
  if (current_window_)
    return current_window_->getPosition();
  else
    return CIPoint2D(0,0);
}

void
CGLMgr::
setWindowSize(const CISize2D &size)
{
  window_rect_.setSize(size);

  if (current_window_)
    current_window_->setSize(size);
}

CISize2D
CGLMgr::
getWindowSize() const
{
  if (current_window_)
    return current_window_->getSize();
  else
    return CISize2D(1,1);
}

void
CGLMgr::
createWindow(const std::string &name)
{
  int x = window_rect_.getXMin  ();
  int y = window_rect_.getYMin  ();
  int w = window_rect_.getWidth ();
  int h = window_rect_.getHeight();

  if (w <= 0) w = 300;
  if (h <= 0) h = 300;

  current_window_ = new CGLWind(x, y, w, h);

  current_window_->setTitle(name);

  window_map_[++current_window_id_] = current_window_;

  current_window_->setId(current_window_id_);
}

void
CGLMgr::
createSubWindow(int window, int x, int y, int width, int height)
{
  CGLWind *parent = window_map_[window];

  current_window_ = new CGLWind(parent, x, y, width, height);

  window_map_[++current_window_id_] = current_window_;

  current_window_->setId(current_window_id_);
}

void
CGLMgr::
deleteWindow(int window)
{
  WindowMap::iterator p1 = window_map_.find(window);
  WindowMap::iterator p2 = window_map_.end();

  if (p1 != p2) {
    delete (*p1).second;

    window_map_.erase(p1);
  }
}

uint
CGLMgr::
getCurrentWindowId()
{
  return current_window_id_;
}

uint
CGLMgr::
setCurrentWindowId(uint window)
{
  std::swap(current_window_id_, window);

  current_window_ = window_map_[current_window_id_];

  return window;
}

CGLWind *
CGLMgr::
getWindowFromId(uint window) const
{
  WindowMap::const_iterator p1 = window_map_.find(window);
  WindowMap::const_iterator p2 = window_map_.end();

  if (p1 != p2)
    return (*p1).second;
  else
    return NULL;
}

void
CGLMgr::
mainLoop()
{
  CGLEventAdapter adapter;

  CXMachineInst->setEventAdapter(&adapter);

  CXMachineInst->mainLoop();
}

void
CGLMgr::
unimplemented(const std::string &proc)
{
  std::cerr << proc << " Unimplemented" << std::endl;
}

//-------------------

CGL::
CGL(CGLWind *window) :
 window_                (window),
 viewport_              (0.0,0.0,0.0,1.0,1.0,1.0),
 color_buffer_          (NULL),
 render_mode_           (GL_RENDER),
 bg_                    (0.0, 0.0, 0.0, 1.0),
 lighting_              (false),
 flat_shading_          (false),
 line_width_            (1.0),
 current_display_list_  (NULL),
 matrix_mode_           (GL_MODELVIEW),
 double_buffer_         (true),
 point_size_            (1.0),
 depth_near_            (0.0),
 depth_far_             (1.0),
 dither_                (false),
 line_stipple_          (false),
 line_smooth_           (false),
 line_smooth_hint_      (GL_DONT_CARE),
 poly_offset_fill_      (false),
 poly_stipple_          (false),
 poly_smooth_           (false),
 poly_offset_factor_    (0.0),
 poly_offset_units_     (0.0),
 front_face_mode_       (GL_FILL),
 back_face_mode_        (GL_FILL),
 fog_enabled_           (false),
 fog_hint_              (GL_DONT_CARE),
 block_type_            (-1),
 list_base_             (0),
 raster_pos_            (NULL, CPoint3D(0,0,0)),
 color_table_enabled_   (false),
 convolution_2d_enabled_(false),
 histogram_enabled_     (false),
 minmax_enabled_        (false),
 pixel_zoom_x_          (1),
 pixel_zoom_y_          (1),
 texture_next_ind_      (2)
{
  color_buffer_ = new CGLColorBuffer(this);

  model_view_matrix_stack_ .push_back(CMatrix3DH::newIdentityMatrix());
  model_view_imatrix_stack_.push_back(CMatrix3DH::newIdentityMatrix());
  projection_matrix_stack_ .push_back(CMatrix3DH::newIdentityMatrix());
  projection_imatrix_stack_.push_back(CMatrix3DH::newIdentityMatrix());
  texture_matrix_stack_    .push_back(CMatrix3DH::newIdentityMatrix());
  color_matrix_stack_      .push_back(CMatrix3DH::newIdentityMatrix());

  initLights();
  initMaterials();
}

CGL::
~CGL()
{
  for (int i = 0; i < NUM_LIGHTS; ++i)
    delete lights_[i];

  delete color_buffer_;
}

void
CGL::
initLights()
{
  for (int i = 0; i < NUM_LIGHTS; ++i)
    lights_.push_back(new CGeomLight3D(NULL));

  for (int i = 0; i < NUM_LIGHTS; ++i) {
    lights_[i]->setEnabled(false);

    lights_[i]->getObject()->moveTo(CPoint3D(0, 0, 1));

    lights_[i]->setAmbient             (CRGBA(0,0,0));
    lights_[i]->setDiffuse             (CRGBA(0,0,0));
    lights_[i]->setSpecular            (CRGBA(0,0,0));
    lights_[i]->setPosition            (CPoint3D(0,0,1));
    lights_[i]->setSpotDirection       (CVector3D(0,0,-1));
    lights_[i]->setSpotExponent        (0);
    lights_[i]->setSpotCutOff          (180);
    lights_[i]->setDirectional         (true);
    lights_[i]->setConstantAttenuation (1);
    lights_[i]->setLinearAttenuation   (0);
    lights_[i]->setQuadraticAttenuation(0);
  }

  lights_[0]->setDiffuse (CRGBA(1,1,1));
  lights_[0]->setSpecular(CRGBA(1,1,1));
}

void
CGL::
initMaterials()
{
  front_material_.init();
  back_material_ .init();
}

void
CGL::
setDoubleBuffer(bool flag)
{
  double_buffer_ = flag;
}

void
CGL::
setLighting(bool flag)
{
  lighting_ = flag;
}

void
CGL::
setBackground(const CRGBA &bg)
{
  if (executeCommand())
    bg_ = bg;

  if (inDisplayList())
    getCurrentDisplayList()->setBackground(bg);
}

void
CGL::
setForeground(const CRGBA &fg)
{
  if (executeCommand()) {
    fg_ = fg;

    //front_material_.material.setAmbient(fg);
    //front_material_.material.setDiffuse(fg);
  }

  if (inDisplayList())
    getCurrentDisplayList()->setForeground(fg);
}

void
CGL::
setNormal(const CVector3D &normal)
{
  if (executeCommand())
    normal_ = normal;

  if (inDisplayList())
    getCurrentDisplayList()->setNormal(normal);
}

void
CGL::
setTexCoord(const CPoint3D &point)
{
  if (executeCommand())
    tmap_ = point;

  if (inDisplayList())
    getCurrentDisplayList()->setTexCoord(point);
}

void
CGL::
setMaterial(uint face, uint pname, double param)
{
  if (executeCommand()) {
    if      (pname == GL_SHININESS) {
      if (face == GL_FRONT || face == GL_FRONT_AND_BACK)
        setFrontMaterialShininess(param);
      if (face == GL_BACK  || face == GL_FRONT_AND_BACK)
        setBackMaterialShininess(param);
    }
    else
      CGLMgrInst->setErrorNum(GL_INVALID_ENUM);
  }

  if (inDisplayList())
    getCurrentDisplayList()->setMaterial(face, pname, param);
}

void
CGL::
setMaterialV(uint face, uint pname, double *params, uint num_params)
{
  if (executeCommand()) {
    if      (pname == GL_AMBIENT) {
      if (face == GL_FRONT || face == GL_FRONT_AND_BACK)
        setFrontMaterialAmbient(
          CRGBA(params[0], params[1], params[2], params[3]));
      if (face == GL_BACK  || face == GL_FRONT_AND_BACK)
        setBackMaterialAmbient(
          CRGBA(params[0], params[1], params[2], params[3]));
    }
    else if (pname == GL_DIFFUSE) {
      if (face == GL_FRONT || face == GL_FRONT_AND_BACK)
        setFrontMaterialDiffuse(
          CRGBA(params[0], params[1], params[2], params[3]));
      if (face == GL_BACK  || face == GL_FRONT_AND_BACK)
        setBackMaterialDiffuse(
          CRGBA(params[0], params[1], params[2], params[3]));
    }
    else if (pname == GL_SPECULAR) {
      if (face == GL_FRONT || face == GL_FRONT_AND_BACK)
        setFrontMaterialSpecular(
          CRGBA(params[0], params[1], params[2], params[3]));
      if (face == GL_BACK  || face == GL_FRONT_AND_BACK)
        setBackMaterialSpecular(
          CRGBA(params[0], params[1], params[2], params[3]));
    }
    else if (pname == GL_EMISSION) {
      if (face == GL_FRONT || face == GL_FRONT_AND_BACK)
        setFrontMaterialEmission(
          CRGBA(params[0], params[1], params[2], params[3]));
      if (face == GL_BACK  || face == GL_FRONT_AND_BACK)
        setBackMaterialEmission(
          CRGBA(params[0], params[1], params[2], params[3]));
    }
    else if (pname == GL_SHININESS) {
      if (face == GL_FRONT || face == GL_FRONT_AND_BACK)
        setFrontMaterialShininess(params[0]);
      if (face == GL_BACK  || face == GL_FRONT_AND_BACK)
        setFrontMaterialShininess(params[0]);
    }
    else if (pname == GL_AMBIENT_AND_DIFFUSE) {
      if (face == GL_FRONT || face == GL_FRONT_AND_BACK) {
        setFrontMaterialAmbient(
          CRGBA(params[0], params[1], params[2], params[3]));
        setFrontMaterialDiffuse(
          CRGBA(params[0], params[1], params[2], params[3]));
      }
      if (face == GL_BACK  || face == GL_FRONT_AND_BACK) {
        setBackMaterialAmbient(
          CRGBA(params[0], params[1], params[2], params[3]));
        setBackMaterialDiffuse(
          CRGBA(params[0], params[1], params[2], params[3]));
      }
    }
    else if (pname == GL_COLOR_INDEXES) {
      if (face == GL_FRONT || face == GL_FRONT_AND_BACK) {
        setFrontMaterialAmbientInd ((int) params[0]);
        setFrontMaterialDiffuseInd ((int) params[1]);
        setFrontMaterialSpecularInd((int) params[2]);
      }
      if (face == GL_BACK  || face == GL_FRONT_AND_BACK) {
        setBackMaterialAmbientInd ((int) params[0]);
        setBackMaterialDiffuseInd ((int) params[1]);
        setBackMaterialSpecularInd((int) params[2]);
      }
    }
    else
      CGLMgrInst->setErrorNum(GL_INVALID_ENUM);
  }

  if (inDisplayList())
    getCurrentDisplayList()->setMaterialV(face, pname, params, num_params);
}

void
CGL::
beginBlock(int mode)
{
  if (executeCommand()) {
    block_type_ = mode;

    for_each(block_points_.begin(), block_points_.end(), CDeletePointer());

    block_points_.clear();
  }

  if (inDisplayList())
    getCurrentDisplayList()->beginBlock(mode);
}

void
CGL::
endBlock()
{
  if (executeCommand()) {
    if      (block_type_ == GL_POINTS) {
      VertexList::iterator p1 = block_points_.begin();
      VertexList::iterator p2 = block_points_.end  ();

      for ( ; p1 != p2; ++p1)
        drawPoint(*p1);
    }
    else if (block_type_ == GL_LINES) {
      int num_lines = block_points_.size()/2;

      VertexList::iterator p2 = block_points_.begin();
      //VertexList::iterator p3 = block_points_.end  ();

      for (int i = 0; i < num_lines; ++i) {
        VertexList::iterator p1 = p2++;

        drawLine(*p1, *p2, i == 0);

        ++p2;
      }
    }
    else if (block_type_ == GL_LINE_STRIP) {
      int num_lines = 0;

      if (block_points_.size() >= 2)
        num_lines = block_points_.size() - 1;

      VertexList::iterator p2 = block_points_.begin();
      //VertexList::iterator p3 = block_points_.end  ();

      for (int i = 0; i < num_lines; ++i) {
        VertexList::iterator p1 = p2++;

        drawLine(*p1, *p2, i == 0);
      }
    }
    else if (block_type_ == GL_LINE_LOOP) {
      if (block_points_.size() <= 2)
        return;

      VertexList::iterator p2 = block_points_.begin();
      VertexList::iterator p1 = p2++;
      VertexList::iterator p3 = block_points_.end  ();

      for (int i = 0; p2 != p3; p1 = p2, ++p2, ++i)
        drawLine(*p1, *p2, i == 0);

      p2 = block_points_.begin();

      if (p2 != p3)
        drawLine(*p1, *p2, false);
    }
    else if (block_type_ == GL_TRIANGLES) {
      uint num = block_points_.size() / 3;

      VertexList points;

      VertexList::iterator p3 = block_points_.begin();

      for (uint i = 0; i < num; ++i) {
        VertexList::iterator p1 = p3++;
        VertexList::iterator p2 = p3++;

        points.clear();

        points.push_back(*p1);
        points.push_back(*p2);
        points.push_back(*p3);

        drawPolygon(block_type_, points);

        ++p3;
      }
    }
    else if (block_type_ == GL_TRIANGLE_STRIP) {
      if (block_points_.size() <= 2)
        return;

      uint num = block_points_.size() - 2;

      VertexList points;

      VertexList::iterator p3 = block_points_.begin();
      VertexList::iterator p1 = p3++;
      VertexList::iterator p2 = p3++;

      for (uint i = 1; i <= num; ++i) {
        points.clear();

        if (i & 1) {
          points.push_back(*p1);
          points.push_back(*p2);
          points.push_back(*p3);
        }
        else {
          points.push_back(*p2);
          points.push_back(*p1);
          points.push_back(*p3);
        }

        drawPolygon(block_type_, points);

        p1 = p2;
        p2 = p3++;
      }
    }
    else if (block_type_ == GL_TRIANGLE_FAN) {
      if (block_points_.size() <= 2)
        return;

      uint num = block_points_.size() - 2;

      VertexList points;

      VertexList::iterator p3 = block_points_.begin();
      VertexList::iterator p1 = p3++;
      VertexList::iterator p2 = p3++;

      for (uint i = 1; i <= num; ++i) {
        points.clear();

        points.push_back(*p1);
        points.push_back(*p2);
        points.push_back(*p3);

        drawPolygon(block_type_, points);

        p2 = p3++;
      }
    }
    else if (block_type_ == GL_QUADS) {
      uint num = block_points_.size() / 4;

      VertexList points;

      VertexList::iterator p4 = block_points_.begin();

      for (uint i = 0; i < num; ++i) {
        VertexList::iterator p1 = p4++;
        VertexList::iterator p2 = p4++;
        VertexList::iterator p3 = p4++;

        points.clear();

        points.push_back(*p1);
        points.push_back(*p2);
        points.push_back(*p3);
        points.push_back(*p4);

        drawPolygon(block_type_, points);

        ++p4;
      }
    }
    else if (block_type_ == GL_QUAD_STRIP) {
      if (block_points_.size() < 4)
        return;

      uint num = block_points_.size()/2 - 1;

      VertexList points;

      VertexList::iterator p4 = block_points_.begin();

      for (uint i = 0; i < num; ++i) {
        VertexList::iterator p1 = p4++;
        VertexList::iterator p2 = p4++;
        VertexList::iterator p3 = p4++;

        points.clear();

        points.push_back(*p1);
        points.push_back(*p2);
        points.push_back(*p4);
        points.push_back(*p3);

        drawPolygon(block_type_, points);

        p4 = p3;
      }
    }
    else if (block_type_ == GL_POLYGON)
      drawPolygon(block_type_, block_points_);

    for_each(block_points_.begin(), block_points_.end(), CDeletePointer());

    block_points_.clear();

    block_type_ = -1;
  }

  if (inDisplayList())
    getCurrentDisplayList()->endBlock();
}

bool
CGL::
inBlock()
{
  return (block_type_ != -1);
}

void
CGL::
addBlockPoint(const CPoint3D &point)
{
  if (executeCommand()) {
    CGeomVertex3D *vertex = new CGeomVertex3D(NULL, point);

    if (getForegroundSet())
      vertex->setColor(getForeground());
    else
      vertex->setColor(CRGBA(1,1,1));

    if (getNormalSet())
      vertex->setNormal(getNormal());

    if (getTexCoordSet())
      vertex->setTextureMap(getTexCoord());

    block_points_.push_back(vertex);
  }

  if (inDisplayList())
    getCurrentDisplayList()->addBlockPoint(point);
}

void
CGL::
addBlockPoint(const CPoint3D &point, const CRGBA &rgba,
              const CVector3D &normal, const CPoint3D &tmap)
{
  CGeomVertex3D *vertex = new CGeomVertex3D(NULL, point);

  vertex->setColor     (rgba);
  vertex->setNormal    (normal);
  vertex->setTextureMap(tmap);

  block_points_.push_back(vertex);
}

void
CGL::
addBlockPoint(const CPoint3D &point,
              const COptValT<CRGBA> &rgba,
              const COptValT<CVector3D> &normal,
              const COptValT<CPoint3D> &tmap)
{
  if (executeCommand()) {
    CGeomVertex3D *vertex = new CGeomVertex3D(NULL, point);

    if (rgba.isValid())
      vertex->setColor(rgba.getValue());
    else
      vertex->setColor(CRGBA(1,1,1));

    if (normal.isValid())
      vertex->setNormal(normal.getValue());

    if (tmap.isValid())
      vertex->setTextureMap(tmap.getValue());

    block_points_.push_back(vertex);
  }

  if (inDisplayList())
    getCurrentDisplayList()->addBlockPoint(point);
}

void
CGL::
updateRasterPos(const CPoint3D &point)
{
  raster_pos_.setModel(point);

  if (getForegroundSet())
    raster_pos_.setColor(getForeground());
  else
    raster_pos_.setColor(CRGBA(1,1,1));

  if (getNormalSet())
    raster_pos_.setNormal(getNormal());

  if (getTexCoordSet())
    raster_pos_.setTextureMap(getTexCoord());

  transformVertex(&raster_pos_);
}

void
CGL::
drawPoint(CGeomVertex3D *vertex)
{
  // transform point
  transformVertex(vertex);

  //-------

  // transform point
  if (vertex->getClipSide() == CCLIP_SIDE_OUTSIDE)
    return;

  //-------

  if      (getRenderMode() == GL_FEEDBACK) {
    float data[2];

    CGLFeedbackBuffer &feedback = modifyFeedbackBuffer();

    data[0] = GL_POINT_TOKEN;

    feedback.append(data, 1);

    feedback.append(vertex);

    return;
  }
#if 0
  else if (getRenderMode() == GL_SELECT) {
    float data[2];

    CGLSelectBuffer &select = modifySelectBuffer();

    select.append(names);

    return;
  }
#endif

  //-------

  const CPoint3D &point = vertex->getPixel();

  const CPoint3D &viewed = vertex->getViewed();

  CGLColorBuffer::Point bpoint(point.z,
                               vertex->getColor(),
                               vertex->getVNormal(),
                               vertex->getTextureMap());

  if (point_size_ > 1.5) {
    uint diameter = CMathGen::Round(point_size_);

    uint radius = diameter >> 1;

    uint radius2 = radius*radius;

    if (diameter & 1) {
      for (uint x = uint(point.x) - radius; x < uint(point.x) + radius; ++x)
        getColorBuffer().setPoint(x, uint(point.y), bpoint, viewed.z);
    }

    for (uint y = 1; y <= radius; ++y) {
      uint width = CMathGen::Round(sqrt(radius2 - y*y));

      for (uint x = uint(point.x) - width; x < uint(point.x) + width; ++x) {
        getColorBuffer().setPoint(x, uint(point.y) + y, bpoint, viewed.z);
        getColorBuffer().setPoint(x, uint(point.y) - y, bpoint, viewed.z);
      }
    }
  }
  else {
    getColorBuffer().setPoint(uint(point.x), uint(point.y), bpoint, viewed.z);
  }
}

void
CGL::
drawLine(CGeomVertex3D *vertex1, CGeomVertex3D *vertex2, bool first)
{
  // transform vertices
  transformVertex(vertex1);
  transformVertex(vertex2);

  //-------

  // clip line (TODO - partial clip)
  if (vertex1->getClipSide() == CCLIP_SIDE_OUTSIDE &&
      vertex2->getClipSide() == CCLIP_SIDE_OUTSIDE)
    return;

  //-------

  if       (getRenderMode() == GL_FEEDBACK) {
    float data[2];

    CGLFeedbackBuffer &feedback = modifyFeedbackBuffer();

    data[0] = first ? GL_LINE_RESET_TOKEN : GL_LINE_TOKEN;

    feedback.append(data, 1);

    feedback.append(vertex1);
    feedback.append(vertex2);

    return;
  }
#if 0
  else if (getRenderMode() == GL_SELECT) {
    float data[2];

    CGLSelectBuffer &select = modifySelectBuffer();

    select.append(names);

    return;
  }
#endif

  //-------

  // light line
  if (getLighting()) {
    CRGBA ambient = getLightModelAmbient();

    const CMaterial &material = getFrontMaterial().material;

    CRGBA rgba = material.getEmission();

    rgba += ambient*material.getAmbient();

    std::vector<CGeomLight3D *>::const_iterator plight1 = lights_.begin();
    std::vector<CGeomLight3D *>::const_iterator plight2 = lights_.end  ();

    for ( ; plight1 != plight2; ++plight1)
      (*plight1)->lightPoint(rgba, vertex1->getViewed(), vertex1->getVNormal(), material);

    vertex1->setColor(rgba.clamp());
    vertex2->setColor(rgba.clamp());
    //getColorBuffer().setDefaultColor(rgba.clamp());
  }

  // draw line
  getColorBuffer().setLineWidth(uint(getLineWidth()));

  if (getLineStipple())
    getColorBuffer().setLineDash(getLineDash().getIDash());
  else
    getColorBuffer().setLineDash(CILineDash());

  // getColorBuffer().setAntiAlias(getLineSmooth());

  getColorBuffer().drawLine(vertex1, vertex2);
}

void
CGL::
drawPolygon(uint mode, const VertexList &vertices)
{
  if (vertices.size() < 3)
    return;

  //----------

  // transform vertices
  VertexList::const_iterator p1 = vertices.begin();
  VertexList::const_iterator p2 = vertices.end  ();

  for ( ; p1 != p2; ++p1)
    transformVertex(*p1);

  //-------

  // cull face (TODO - fix me !!!)
  CGLColorBuffer &color_buffer = getColorBuffer();

  p1 = vertices.begin();

  int min_x = 9999; int max_x = -9999;
  int min_y = 9999; int max_y = -9999;

  for ( ; p1 != p2; ++p1) {
    const CPoint3D &point = (*p1)->getPixel();

    min_x = std::min(min_x, int(point.x)); min_y = std::min(min_y, int(point.y));
    max_x = std::max(max_x, int(point.x)); max_y = std::max(max_y, int(point.y));
  }

  if (max_x < 0 || max_y < 0 ||
      min_x >= (int) color_buffer.getWidth () ||
      min_y >= (int) color_buffer.getHeight())
    return;

  if (getCullFace().getEnabled()) {
    const CPoint3D &point1 = vertices[0]->getPixel();
    const CPoint3D &point2 = vertices[1]->getPixel();
    const CPoint3D &point3 = vertices[2]->getPixel();

    CTriangle3D triangle(point1, point2, point3);

    CPolygonOrientation orientation = triangle.orientationXY();

    bool is_front = (orientation != getFrontFaceOrient());

    if (is_front) {
      if (getCullFace().getFront())
        return;
    }
    else {
      if (getCullFace().getBack())
        return;
    }
  }

  //-------

  // Process Feedback and select

  if      (getRenderMode() == GL_FEEDBACK) {
    float data[2];

    CGLFeedbackBuffer &feedback = modifyFeedbackBuffer();

    data[0] = GL_POLYGON_TOKEN;

    feedback.append(data, 1);

    VertexList::const_iterator p1 = vertices.begin();
    VertexList::const_iterator p2 = vertices.end  ();

    for ( ; p1 != p2; ++p1)
      feedback.append(*p1);

    return;
  }
  else if (getRenderMode() == GL_SELECT) {
    CGLSelectBuffer &select = modifySelectBuffer();

    VertexList::const_iterator p1 = vertices.begin();
    VertexList::const_iterator p2 = vertices.end  ();

    int min_z = INT_MAX; int max_z = -INT_MAX;

    for ( ; p1 != p2; ++p1) {
      const CPoint3D &point = (*p1)->getProjected();

      int iz = int(INT_MAX*((std::min(std::max(point.z, -1.0), 1.0) + 1)/2.0));

      min_z = std::min(min_z, iz);
      max_z = std::max(max_z, iz);
    }

    const CGL::NameStack &stack = getNameStack();

    CGL::NameStack::const_iterator pn1 = stack.begin();
    CGL::NameStack::const_iterator pn2 = stack.end  ();

    CGLSelectBuffer::NameList names;

    for ( ; pn1 != pn2; ++pn1)
      names.push_back(*pn1);

    select.addHit(min_z, max_z, names);

    return;
  }

  //-------

  // draw face
  CPoint3D  mid_point;
  CVector3D normal;

  if (getLighting()) {
    CRGBA ambient = getLightModelAmbient();

    if (getFlatShading()) {
      const CMaterial &material = getFrontMaterial().material;

      CRGBA rgba = material.getEmission();

      rgba += ambient*material.getAmbient();

      mid_point = CGeomUtil3D::getMidPoint(vertices);
      normal    = CGeomUtil3D::getNormal  (vertices);

      std::vector<CGeomLight3D *>::const_iterator plight1 = lights_.begin();
      std::vector<CGeomLight3D *>::const_iterator plight2 = lights_.end  ();

      for ( ; plight1 != plight2; ++plight1)
        (*plight1)->lightPoint(rgba, mid_point, normal, material);

      getColorBuffer().setDefaultColor(rgba.clamp());
    }
    else {
      const CMaterial &material = getFrontMaterial().material;

      normal = CGeomUtil3D::getNormal(vertices);

      VertexList::const_iterator pv1 = vertices.begin();
      VertexList::const_iterator pv2 = vertices.end  ();

      for ( ; pv1 != pv2; ++pv1) {
        CRGBA rgba = material.getEmission();

        rgba += ambient*material.getAmbient();

        std::vector<CGeomLight3D *>::const_iterator plight1 = lights_.begin();
        std::vector<CGeomLight3D *>::const_iterator plight2 = lights_.end  ();

        for ( ; plight1 != plight2; ++plight1) {
          if ((*pv1)->getNormalSet())
            (*plight1)->lightPoint(rgba, (*pv1)->getViewed(), (*pv1)->getVNormal(), material);
          else
            (*plight1)->lightPoint(rgba, (*pv1)->getViewed(), normal, material);
        }

        (*pv1)->setColor(rgba.clamp());
      }
    }
  }
  else {
    // Default color is last vertex color except for
    // single polygon when it's the first (need mode)

    if (mode == GL_POLYGON)
      getColorBuffer().setDefaultColor(vertices.front()->getColor());
    else
      getColorBuffer().setDefaultColor(vertices.back ()->getColor());
  }

  if      (getFrontFaceMode() == GL_FILL) {
    if (poly_offset_fill_) {
      double m = 1.0 ; // TODO calc polgon maximum depth slope
      double r = 1E-6; // TODO better value ?

      double dz = m*getPolyOffsetFactor() + r*getPolyOffsetUnits();

      VertexList::const_iterator pv1 = vertices.begin();
      VertexList::const_iterator pv2 = vertices.end  ();

      for ( ; pv1 != pv2; ++pv1) {
        CPoint3D &pixel = const_cast<CPoint3D &>((*pv1)->getPixel());

        pixel.z += dz;
      }

      getColorBuffer().fillPolygon(vertices);

      pv1 = vertices.begin();

      for ( ; pv1 != pv2; ++pv1) {
        CPoint3D &pixel = const_cast<CPoint3D &>((*pv1)->getPixel());

        pixel.z -= dz;
      }
    }
    else
      getColorBuffer().fillPolygon(vertices);
  }
  else if (getFrontFaceMode() == GL_LINE)
    getColorBuffer().drawLines(vertices);
  else if (getFrontFaceMode() == GL_POINT)
    getColorBuffer().drawPoints(vertices);
}

void
CGL::
transformVertex(CGeomVertex3D *vertex)
{
#if 0
  // left, right, top, bottom, front, back planes
  static CNPlane3D planex1( 1,  0,  0, 1);
  static CNPlane3D planex2(-1,  0,  0, 1);
  static CNPlane3D planey1( 0,  1,  0, 1);
  static CNPlane3D planey2( 0, -1,  0, 1);
  static CNPlane3D planez1( 0,  0,  1, 1);
  static CNPlane3D planez2( 0,  0, -1, 1);
#endif

  CPoint3D vpoint, jpoint, ppoint;

  // model to view
  CMatrix3DH *model_matrix = getMatrix(GL_MODELVIEW);

  model_matrix->multiplyPoint(vertex->getModel(), vpoint);

  vertex->setViewed(vpoint);

  if (vertex->getNormalSet()) {
    CMatrix3DH *imodel_matrix = getIMatrix(GL_MODELVIEW);

    CMatrix3DH imodel_matrixt = imodel_matrix->transposed();

    CVector3D vnormal;

    imodel_matrixt.multiplyVector(vertex->getNormal(), vnormal);

    vertex->setVNormal(vnormal);
  }

  //--------

  vertex->setClipSide(CCLIP_SIDE_INSIDE);

  // clip to extra matrices
  for (uint i = 0; i < MAX_CLIP_PLANES; ++i) {
    // TODO: transform by transpose inverse modelview
    if (! clipPlaneEnabled(i)) continue;

    if (vertex->getClipSide() != CCLIP_SIDE_OUTSIDE)
      vertex->setClipSide(getClipSide(i, vpoint));
  }

  //--------

  // project
  CMatrix3DH *project_matrix = getMatrix(GL_PROJECTION);

  project_matrix->multiplyPoint(vpoint, jpoint);

  vertex->setProjected(jpoint);

  //--------

  // clip to view plane x,y,z = +/- w
  if (jpoint.x < -1 || jpoint.x > 1 ||
      jpoint.y < -1 || jpoint.y > 1 ||
      jpoint.z < -1 || jpoint.z > 1)
    vertex->setClipSide(CCLIP_SIDE_OUTSIDE);

  //--------

  // to pixel
  pixel_matrix_.multiplyPoint(jpoint, ppoint);

  vertex->setPixel(ppoint);
}

void
CGL::
untransformVertex(CGeomVertex3D *vertex)
{
  CPoint3D mpoint, vpoint, jpoint;

  ipixel_matrix_.multiplyPoint(vertex->getPixel(), jpoint);

  vertex->setProjected(jpoint);

  CMatrix3DH *iproject_matrix = getIMatrix(GL_PROJECTION);

  iproject_matrix->multiplyPoint(jpoint, vpoint);

  vertex->setViewed(vpoint);

  CMatrix3DH *imodel_matrix = getIMatrix(GL_MODELVIEW);

  imodel_matrix->multiplyPoint(vpoint, mpoint);

  vertex->setModel(mpoint);
}

void
CGL::
resize(int w, int h)
{
  getColorBuffer().resize(w, h);
  getAccumBuffer().resize(w, h);
}

#if 0
void
CGL::
updateSize(int width, int height, bool *changed)
{
  if (window_rect_.getSize() == CISize2D(width, height)) {
    *changed = false;

    return;
  }

  *changed = true;

  window_rect_.setSize(CISize2D(width, height));
}
#endif

bool
CGL::
executeCommand() const
{
  return (! inDisplayList() || getCurrentDisplayList()->getExecute());
}

int
CGL::
createDisplayList()
{
  CGLDisplayList *display_list = new CGLDisplayList;

  uint id = display_list->getId();

  display_list_map_[id] = display_list;

  return id;
}

bool
CGL::
startDisplayList(uint id, bool execute)
{
  CGLDisplayList *display_list = lookupDisplayList(id);

  if (! display_list) {
    display_list = new CGLDisplayList(id);

    display_list_map_[id] = display_list;
  }
  else
    display_list->clear();

  display_list->setExecute(execute);

  current_display_list_ = display_list;

  return true;
}

bool
CGL::
endDisplayList()
{
  current_display_list_ = NULL;

  return true;
}

bool
CGL::
inDisplayList() const
{
  return (current_display_list_ != NULL);
}

bool
CGL::
executeDisplayList(uint id)
{
  CGLDisplayList *display_list = lookupDisplayList(id);

  if (display_list)
    return display_list->execute(this);
  else
    return false;
}

bool
CGL::
isDisplayList(uint id)
{
  CGLDisplayList *display_list = lookupDisplayList(id);

  return (display_list != NULL);
}

CGLDisplayList *
CGL::
lookupDisplayList(uint id)
{
  DisplayListMap::const_iterator pdisplay_list1 = display_list_map_.find(id);
  DisplayListMap::const_iterator pdisplay_list2 = display_list_map_.end ();

  if (pdisplay_list1 != pdisplay_list2)
    return (*pdisplay_list1).second;

  return NULL;
}

void
CGL::
deleteDisplayList(uint id)
{
  if (display_list_map_.find(id) != display_list_map_.end()) {
    delete display_list_map_[id];

    display_list_map_[id] = NULL;
  }
}

bool
CGL::
pushMatrix()
{
  CMatrix3DH *matrix = getMatrix();

  if      (matrix_mode_ == GL_MODELVIEW) {
    model_view_matrix_stack_ .push_back(new CMatrix3DH(*matrix));
    model_view_imatrix_stack_.push_back(new CMatrix3DH(matrix->inverse()));
  }
  else if (matrix_mode_ == GL_PROJECTION) {
    projection_matrix_stack_ .push_back(new CMatrix3DH(*matrix));
    projection_imatrix_stack_.push_back(new CMatrix3DH(matrix->inverse()));
  }
  else if (matrix_mode_ == GL_TEXTURE)
    texture_matrix_stack_.push_back(new CMatrix3DH(*matrix));
  else if (matrix_mode_ == GL_COLOR)
    color_matrix_stack_.push_back(new CMatrix3DH(*matrix));

  return true;
}

bool
CGL::
popMatrix()
{
  if      (matrix_mode_ == GL_MODELVIEW) {
    if (model_view_matrix_stack_.size() <= 1)
      return false;

    CMatrix3DH *matrix  = getMatrix();
    CMatrix3DH *imatrix = getIMatrix();

    model_view_matrix_stack_ .pop_back();
    model_view_imatrix_stack_.pop_back();

    delete matrix;
    delete imatrix;
  }
  else if (matrix_mode_ == GL_PROJECTION) {
    if (projection_matrix_stack_.size() <= 1)
      return false;

    CMatrix3DH *matrix  = getMatrix();
    CMatrix3DH *imatrix = getIMatrix();

    projection_matrix_stack_ .pop_back();
    projection_imatrix_stack_.pop_back();

    delete matrix;
    delete imatrix;
  }
  else if (matrix_mode_ == GL_TEXTURE) {
    if (texture_matrix_stack_.size() <= 1)
      return false;

    CMatrix3DH *matrix = getMatrix();

    texture_matrix_stack_.pop_back();

    delete matrix;
  }
  else if (matrix_mode_ == GL_COLOR) {
    if (color_matrix_stack_.size() <= 1)
      return false;

    CMatrix3DH *matrix = getMatrix();

    color_matrix_stack_.pop_back();

    delete matrix;
  }

  return true;
}

void
CGL::
updateIMatrix(CMatrix3DH *matrix)
{
  if (matrix_mode_ == GL_MODELVIEW || matrix_mode_ == GL_PROJECTION) {
    CMatrix3DH *imatrix = getIMatrix();

    *imatrix = matrix->inverse();
  }
}

void
CGL::
rotateMatrix(double angle, double x, double y, double z)
{
  double angle1 = CMathGen::DegToRad(angle);

  CMatrix3DH matrix;

  CVector3D axis(x, y, z);

  matrix.setGenRotation(axis, angle1, CMathGen::LEFT_HANDEDNESS);

  if (executeCommand())
    multMatrix(&matrix);

  if (inDisplayList())
    getCurrentDisplayList()->multMatrix(matrix);
}

void
CGL::
scaleMatrix(double x, double y, double z)
{
  CMatrix3DH matrix;

  matrix.setScale(x, y, z);

  if (executeCommand())
    multMatrix(&matrix);

  if (inDisplayList())
    getCurrentDisplayList()->multMatrix(matrix);
}

void
CGL::
translateMatrix(double x, double y, double z)
{
  CMatrix3DH matrix;

  matrix.setTranslation(x, y, z);

  if (executeCommand())
    multMatrix(&matrix);

  if (inDisplayList())
    getCurrentDisplayList()->multMatrix(matrix);
}

void
CGL::
initMatrix()
{
  CMatrix3DH *current_matrix = getMatrix();

  current_matrix->setIdentity();

  updateIMatrix(current_matrix);
}

void
CGL::
setMatrix(CMatrix3DH *matrix)
{
  CMatrix3DH *current_matrix = getMatrix();

  *current_matrix = *matrix;

  updateIMatrix(current_matrix);
}

void
CGL::
multMatrix(CMatrix3DH *matrix)
{
  CMatrix3DH *current_matrix = getMatrix();

  *current_matrix *= *matrix;

  updateIMatrix(current_matrix);
}

CMatrix3DH *
CGL::
getMatrix() const
{
  return getMatrix(matrix_mode_);
}

CMatrix3DH *
CGL::
getIMatrix() const
{
  return getIMatrix(matrix_mode_);
}

CMatrix3DH *
CGL::
getMatrix(int mode) const
{
  if      (mode == GL_MODELVIEW)
    return model_view_matrix_stack_[model_view_matrix_stack_.size() - 1];
  else if (mode == GL_PROJECTION)
    return projection_matrix_stack_[projection_matrix_stack_.size() - 1];
  else if (mode == GL_TEXTURE)
    return texture_matrix_stack_[texture_matrix_stack_.size() - 1];
  else if (mode == GL_COLOR)
    return color_matrix_stack_[color_matrix_stack_.size() - 1];
  else
    return NULL;
}

CMatrix3DH *
CGL::
getIMatrix(int mode) const
{
  if      (mode == GL_MODELVIEW)
    return model_view_imatrix_stack_[model_view_matrix_stack_.size() - 1];
  else if (mode == GL_PROJECTION)
    return projection_imatrix_stack_[projection_matrix_stack_.size() - 1];
  else {
    std::cerr << "Calc IMatrix" << std::endl;

    static CMatrix3DH imatrix;

    CMatrix3DH *matrix = getMatrix(mode);

    imatrix = matrix->inverse();

    return &imatrix;
  }
}

void
CGL::
drawBitmap(uint width, uint height, double xorig, double yorig,
           double xmove, double ymove, const uchar *bitmap)
{
  if (executeCommand())
    drawBitmapI(width, height, xorig, yorig, xmove, ymove, bitmap);

  if (inDisplayList())
    getCurrentDisplayList()->bitmap(width, height, xorig, yorig, xmove, ymove, bitmap);
}

void
CGL::
drawBitmapI(uint width, uint height, double xorig, double yorig,
            double xmove, double ymove, const uchar *bitmap)
{
  if (bitmap != NULL) {
    CGLColorBuffer &color_buffer = getColorBuffer();

    const CPoint3D &point = getRasterPos().getPixel();

    const CPoint3D &viewed = getRasterPos().getViewed();

    const CRGBA &rgba = getRasterPos().getColor();

    CGLColorBuffer::Point bpoint(1, rgba);

    uint width1 = (width + 7) >> 3;

    const uchar *p = bitmap;

    double yp = point.y + yorig;

    for (uint y = 0; y < height; ++y, ++yp) {
      double xp = point.x + xorig;

      for (uint x1 = 0, x = 0; x1 < width1; ++x1, ++p) {
        for (uint b = 0; b < 8 && x < width; ++b, ++x, ++xp) {
          if (*p & (1 << (7 - b)))
            color_buffer.setPoint(uint(xp), uint(yp), bpoint, viewed.z);
        }
      }
    }
  }

  CPoint3D mpoint = getRasterPos().getModel();

  mpoint.x += xmove;
  mpoint.y += ymove;

  CGeomVertex3D vertex = getRasterPos();

  vertex.setModel(mpoint);

  transformVertex(&vertex);

  setRasterPos(vertex);
}

void
CGL::
drawRGBAImage(uint width, uint height, const uchar *image, uint num_components, bool reverse)
{
  static uint   image_buffer_size;
  static CRGBA *image_buffer1, *image_buffer2;

  if (image == NULL)
    return;

  //-------

  uint size = width*height;

  if (size > image_buffer_size) {
    delete [] image_buffer1;
    delete [] image_buffer2;

    image_buffer_size = size;

    image_buffer1 = new CRGBA [image_buffer_size];
    image_buffer2 = new CRGBA [image_buffer_size];
  }

  //-------

  // draw image into buffer and apply pixel transfer
  double factor = 1.0/255.0;

  const uchar *ip = image;

  CRGBA *op = image_buffer1;

  const CGLPixelTransfer &pixel_transfer = getPixelTransfer();

  if (num_components == 3) {
    double r, g, b;

    for (uint y = 0; y < height; ++y) {
      for (uint x = 0; x < width; ++x, ip += 3, ++op) {
        if (! reverse) {
          r = ip[0]*factor;
          g = ip[1]*factor;
          b = ip[2]*factor;
        }
        else {
          r = ip[2]*factor;
          g = ip[1]*factor;
          b = ip[0]*factor;
        }

        r = r*pixel_transfer.getRedScale  () + pixel_transfer.getRedBias  ();
        g = g*pixel_transfer.getGreenScale() + pixel_transfer.getGreenBias();
        b = b*pixel_transfer.getBlueScale () + pixel_transfer.getBlueBias ();

        *op = CRGBA(r, g, b);
      }
    }
  }
  else {
    double r, g, b, a;

    for (uint y = 0; y < height; ++y) {
      for (uint x = 0; x < width; ++x, ip += 4, ++op) {
        if (! reverse) {
          r = ip[0]*factor;
          g = ip[1]*factor;
          b = ip[2]*factor;
        }
        else {
          r = ip[2]*factor;
          g = ip[1]*factor;
          b = ip[0]*factor;
        }

        a = ip[3]*factor;

        r = r*pixel_transfer.getRedScale  () + pixel_transfer.getRedBias  ();
        g = g*pixel_transfer.getGreenScale() + pixel_transfer.getGreenBias();
        b = b*pixel_transfer.getBlueScale () + pixel_transfer.getBlueBias ();
        a = a*pixel_transfer.getAlphaScale() + pixel_transfer.getAlphaBias();

        *op = CRGBA(r, g, b, a);
      }
    }
  }

  //------

  // apply color table
  if (getColorTableEnabled()) {
    const CGLColorTable &color_table = getColorTable();

    op = image_buffer1;

    for (uint y = 0; y < height; ++y)
      for (uint x = 0; x < width; ++x, ++op)
        *op = color_table.remap(*op);
  }

  //------

  // apply convolution
  if (getConvolution2DEnabled()) {
    const CGLConvolutionFilter2D &filter = getConvolution2D();

    uint fwidth  = filter.getWidth ();
    uint fheight = filter.getHeight();

    int dx = fwidth  >> 1;
    int dy = fheight >> 1;

    memcpy(image_buffer2, image_buffer1, image_buffer_size*sizeof(CRGBA));

    for (uint y = dy; y < height - dy; ++y) {
      op = &image_buffer1[y*width + dx];

      for (uint x = dx; x < width - dx; ++x, ++op) {
        CRGBA rgba(0,0,0);

        for (uint y1 = 0; y1 < fheight; ++y1) {
          for (uint x1 = 0; x1 < fwidth; ++x1) {
            const CRGBA &rgba1 =
              image_buffer2[(y + y1 - dy)*width + x + x1 - dx];

            const CRGBA &rgba2 = filter.getValue(x1, y1);

            rgba += (rgba1*rgba2);
          }
        }

        *op = rgba.clamp();
      }
    }
  }

  //------

  // apply color_matrix
  CMatrix3DH *color_matrix = getMatrix(GL_COLOR);

  if (! color_matrix->isIdentity()) {
    double r, g, b, a;

    op = image_buffer1;

    for (uint y = 0; y < height; ++y) {
      for (uint x = 0; x < width; ++x, ++op) {
        (*op).getRGBA(&r, &g, &b, &a);

        color_matrix->preMultiplyPoint(r, g, b, a, &r, &g, &b, &a);

        (*op).setRGBA(r, g, b, a);
      }
    }
  }

  //------

  // calc histogram
  if (getHistogramEnabled()) {
    calcHistogram(image_buffer1, width, height);

    if (getHistogram().getSink())
      return;
  }

  //------

  // calc min max
  if (getMinmaxEnabled()) {
    calcMinmax(image_buffer1, width, height);

    if (getMinmax().getSink())
      return;
  }

  //------

  // add image to color buffer
  CGLColorBuffer &color_buffer = getColorBuffer();

  CGLColorBuffer::Point bpoint;

  const CPoint3D &point = getRasterPos().getPixel();

  const CPoint3D &viewed = getRasterPos().getViewed();

  CRGBA rgba = getRasterPos().getColor();

  op = image_buffer1;

  double xs = getPixelZoomX();
  double ys = getPixelZoomY();

  double yp1 = point.y;
  double yp2 = yp1 + ys;

  for (uint y = 0; y < height; ++y, yp1 = yp2, yp2 += ys) {
    double xp1 = point.x;
    double xp2 = xp1 + xs;

    for (uint x = 0; x < width; ++x, xp1 = xp2, xp2 += xs, ++op) {
      bpoint.rgba = *op;

      uint nx = abs(uint(xp2) - uint(xp1));

      for (uint ix = 0, xs1 = uint(xp1); ix <= nx; ++ix, ++xs1) {
        uint ny = abs(uint(yp2) - uint(yp1));

        for (uint iy = 0, ys1 = uint(yp1); iy <= ny; ++iy, ++ys1) {
          color_buffer.setPoint(xs1, ys1, bpoint, viewed.z);
        }
      }
    }
  }
}

void
CGL::
copyColorImage(int x, int y, uint width, uint height, uint num_components)
{
  uchar *pixels = new uchar [num_components*width*height];

  readColorImage(x, y, width, height, pixels, num_components, false);

  drawRGBAImage(width, height, pixels, num_components, false);

  delete [] pixels;
}

void
CGL::
readColorImage(int x, int y, uint width, uint height, uchar *pixels,
               uint num_components, bool process)
{
  CGeomVertex3D vertex(NULL, CPoint3D(x, y, 0));

  transformVertex(&vertex);

  const CPoint3D &point = vertex.getPixel();

  uchar *p = pixels;

  CRGBA  rgba;
  double r, g, b, a;

  double factor = 255.0;

  const CGLPixelTransfer &pixel_transfer = getPixelTransfer();

  CGLColorBuffer &color_buffer = getColorBuffer();

  double yp = point.y;

  for (uint y1 = 0; y1 < height; ++y1, yp += 1) {
    double xp = point.x;

    if (num_components == 4) {
      for (uint x1 = 0; x1 < width; ++x1, xp += 1, p += 4) {
        const CGLColorBuffer::Point &point = color_buffer.getPoint(uint(xp), uint(yp));

        r = point.rgba.getRed  ();
        g = point.rgba.getGreen();
        b = point.rgba.getBlue ();
        a = point.rgba.getAlpha();

        if (process) {
          r = r*pixel_transfer.getRedScale  () + pixel_transfer.getRedBias  ();
          g = g*pixel_transfer.getGreenScale() + pixel_transfer.getGreenBias();
          b = b*pixel_transfer.getBlueScale () + pixel_transfer.getBlueBias ();
          a = a*pixel_transfer.getAlphaScale() + pixel_transfer.getAlphaBias();
        }

        p[0] = int(r*factor);
        p[1] = int(g*factor);
        p[2] = int(b*factor);
        p[3] = int(a*factor);
      }
    }
    else {
      for (uint x1 = 0; x1 < width; ++x1, xp += 1, p += 3) {
        const CGLColorBuffer::Point &point = color_buffer.getPoint(uint(xp), uint(yp));

        r = point.rgba.getRed  ();
        g = point.rgba.getGreen();
        b = point.rgba.getBlue ();

        if (process) {
          r = r*pixel_transfer.getRedScale  () + pixel_transfer.getRedBias  ();
          g = g*pixel_transfer.getGreenScale() + pixel_transfer.getGreenBias();
          b = b*pixel_transfer.getBlueScale () + pixel_transfer.getBlueBias ();
        }

        p[0] = int(r*factor);
        p[1] = int(g*factor);
        p[2] = int(b*factor);
      }
    }
  }
}

void
CGL::
calcHistogram(const CRGBA *image, uint width, uint height)
{
  CGLHistogram histogram = getHistogram();

  uint type = histogram.getType();

  uint hw = histogram.getWidth();

  if (hw == 0)
    return;

  for (uint i = 0; i < hw; ++i)
    histogram.setValue(i, CRGBA(0,0,0,0));

  if (type == GL_RGB || type == GL_RGBA) {
    uint  ir, ig, ib, ia;
    CRGBA rgba, hrgba;

    const CRGBA *p = image;

    for (uint y = 0; y < height; ++y) {
      for (uint x = 0; x < width; ++x, ++p) {
        hrgba = (*p)*(hw - 1);

        ir = uint(hrgba.getRed  ());
        ig = uint(hrgba.getGreen());
        ib = uint(hrgba.getBlue ());
        ia = uint(hrgba.getAlpha());

        rgba = histogram.getValue(ir);

        rgba.setRed  (rgba.getRed  () + 1);

        histogram.setValue(ir, rgba);

        //----

        rgba = histogram.getValue(ig);

        rgba.setGreen(rgba.getGreen() + 1);

        histogram.setValue(ig, rgba);

        //----

        rgba = histogram.getValue(ib);

        rgba.setBlue (rgba.getBlue () + 1);

        histogram.setValue(ib, rgba);

        //----

        rgba = histogram.getValue(ia);

        rgba.setAlpha(rgba.getAlpha() + 1);

        histogram.setValue(ia, rgba);
      }
    }
  }

  setHistogram(histogram);
}

void
CGL::
calcMinmax(const CRGBA *image, uint width, uint height)
{
  CGLMinmax minmax = getMinmax();

  uint type = minmax.getType();

  CRGBA minv( 1E50,  1E50,  1E50,  1E50);
  CRGBA maxv(-1E50, -1E50, -1E50, -1E50);

  if (type == GL_RGB || type == GL_RGBA) {
    CRGBA rgba;

    const CRGBA *p = image;

    for (uint y = 0; y < height; ++y) {
      for (uint x = 0; x < width; ++x, ++p) {
        rgba = *p;

        minv.setRed  (std::min(minv.getRed  (), rgba.getRed  ()));
        minv.setGreen(std::min(minv.getGreen(), rgba.getGreen()));
        minv.setBlue (std::min(minv.getBlue (), rgba.getBlue ()));
        minv.setAlpha(std::min(minv.getAlpha(), rgba.getAlpha()));

        maxv.setRed  (std::max(maxv.getRed  (), rgba.getRed  ()));
        maxv.setGreen(std::max(maxv.getGreen(), rgba.getGreen()));
        maxv.setBlue (std::max(maxv.getBlue (), rgba.getBlue ()));
        maxv.setAlpha(std::max(maxv.getAlpha(), rgba.getAlpha()));
      }
    }
  }

  minmax.setMin(minv);
  minmax.setMax(maxv);

  setMinmax(minmax);
}

void
CGL::
setClipPlane(int pid, const double *equation)
{
  CMatrix3DH *imatrix = getIMatrix();

  double x, y, z, w;

  imatrix->preMultiplyPoint(equation[0], equation[1], equation[2], equation[3], &x, &y, &z, &w);

  clip_plane_[pid].setPlane(CNPlane3D(x, y, z, w));
}

void
CGL::
enableClipPlane(int pid)
{
  clip_plane_[pid].setEnabled(true);
}

void
CGL::
disableClipPlane(int pid)
{
  clip_plane_[pid].setEnabled(false);
}

CClipSide
CGL::
getClipSide(int pid, const CPoint3D &point) const
{
  const CNPlane3D &plane = clip_plane_[pid].getPlane();

  double f = plane.value(point);

  if      (f > 0)
    return CCLIP_SIDE_INSIDE;
  else if (f < 0)
    return CCLIP_SIDE_OUTSIDE;
  else
    return CCLIP_SIDE_ON;
}

void
CGL::
setViewport(double x1, double y1, double x2, double y2)
{
  // int height = window_->getSize().getHeight();

  // y1 = height - 1 - y1;
  // y2 = height - 1 - y2;

  viewport_.setXMin(x1); viewport_.setYMin(y1);
  viewport_.setXMax(x2); viewport_.setYMax(y2);

  double z1 = viewport_.getZMin();
  double z2 = viewport_.getZMax();

  pixel_matrix_.setTransform(-1, -1, -1, 1, 1, 1, x1, y1, z1, x2, y2, z2);

  ipixel_matrix_ = pixel_matrix_.inverse();
}

void
CGL::
getViewport(double *x1, double *y1, double *x2, double *y2)
{
  *x1 = viewport_.getXMin(); *y1 = viewport_.getYMin();
  *x2 = viewport_.getXMax(); *y2 = viewport_.getYMax();
}

void
CGL::
setFont(CFontPtr font)
{
  CXLibPixelRenderer *renderer = window_->getXRenderer();

  renderer->setFont(font);
}

void
CGL::
drawChar(char c)
{
  CGLColorBuffer &color_buffer = getColorBuffer();

  CPoint3D point = getRasterPos().getPixel();

  const CPoint3D &viewed = getRasterPos().getViewed();

  CRGBA rgba = getRasterPos().getColor();

  CGLColorBuffer::Point bpoint(point.z, rgba);

  CXLibPixelRenderer *renderer = window_->getXRenderer();

  CFontPtr font;

  renderer->getFont(font);

  CImagePtr image = font->getCharImage(c);

  int iwidth  = image->getWidth ();
  int iheight = image->getHeight();
  int ascent  = font->getICharAscent();

  int x1 = 0;
  int x2 = iwidth  - 1;
  int y1 = 0;
  int y2 = iheight - 1;

  CRGBA irgba;

  for (int yy = y1; yy <= y2; ++yy) {
    for (int xx = x1; xx <= x2; ++xx) {
      image->getRGBAPixel(xx, yy, irgba);

      if (irgba.getAlpha() < 0.5)
        continue;

      int x = int(point.x) + xx;
      int y = int(point.y) - yy + ascent;

      color_buffer.setPoint(x, y, bpoint, viewed.z);
    }
  }

  point.x += iwidth;

  CGeomVertex3D vertex = getRasterPos();

  vertex.setPixel(point);

  untransformVertex(&vertex);

  setRasterPos(vertex);
}

void
CGL::
accumLoadAdd(double scale)
{
  CGLColorBuffer &color_buffer = getColorBuffer();
  CGLAccumBuffer &accum_buffer = getAccumBuffer();

  uint w = color_buffer.getWidth ();
  uint h = color_buffer.getHeight();

  for (uint y = 0; y < h; ++y) {
    for (uint x = 0; x < w; ++x) {
      const CGLColorBuffer::Point &point = color_buffer.getPoint(x, y);
      const CRGBA                 &rgba  = accum_buffer.getPoint(x, y);

      accum_buffer.setPoint(x, y, rgba + point.rgba*scale);
    }
  }
}

void
CGL::
accumLoad(double scale)
{
  CGLColorBuffer &color_buffer = getColorBuffer();
  CGLAccumBuffer &accum_buffer = getAccumBuffer();

  uint w = color_buffer.getWidth ();
  uint h = color_buffer.getHeight();

  for (uint y = 0; y < h; ++y) {
    for (uint x = 0; x < w; ++x) {
      const CGLColorBuffer::Point &point = color_buffer.getPoint(x, y);

      accum_buffer.setPoint(x, y, point.rgba*scale);
    }
  }
}

void
CGL::
accumUnload(double scale)
{
  CGLColorBuffer &color_buffer = getColorBuffer();
  CGLAccumBuffer &accum_buffer = getAccumBuffer();

  uint w = color_buffer.getWidth ();
  uint h = color_buffer.getHeight();

  for (uint y = 0; y < h; ++y) {
    for (uint x = 0; x < w; ++x) {
      const CRGBA &rgba = accum_buffer.getPoint(x, y);

      CGLColorBuffer::Point point = color_buffer.getPoint(x, y);

      color_buffer.setColor(x, y, rgba*scale);
    }
  }
}

void
CGL::
accumAdd(double factor)
{
  CGLAccumBuffer &accum_buffer = getAccumBuffer();

  uint w = accum_buffer.getWidth ();
  uint h = accum_buffer.getHeight();

  for (uint y = 0; y < h; ++y) {
    for (uint x = 0; x < w; ++x) {
      const CRGBA &rgba = accum_buffer.getPoint(x, y);

      accum_buffer.setPoint(x, y, rgba + factor);
    }
  }
}

void
CGL::
accumMult(double factor)
{
  CGLAccumBuffer &accum_buffer = getAccumBuffer();

  uint w = accum_buffer.getWidth ();
  uint h = accum_buffer.getHeight();

  for (uint y = 0; y < h; ++y) {
    for (uint x = 0; x < w; ++x) {
      const CRGBA &rgba = accum_buffer.getPoint(x, y);

      accum_buffer.setPoint(x, y, rgba*factor);
    }
  }
}

//--------

bool
CGLEventAdapter::
idleEvent()
{
  CGLWindMgrInst->idle();

  COSTimer::msleep(50);

  return true;
}

//--------

void
CGL::
save(CGLLightingBits &bits) const
{
  bits.color_material = color_material_;
  bits.light_model    = light_model_;
  bits.lights         = lights_;
  bits.lighting       = lighting_;
  bits.front_material = front_material_;
  bits.back_material  = back_material_;
  bits.flat_shading   = flat_shading_;
}

void
CGL::
restore(const CGLLightingBits &bits)
{
  color_material_ = bits.color_material;
  light_model_    = bits.light_model;
  lights_         = bits.lights;
  lighting_       = bits.lighting;
  front_material_ = bits.front_material;
  back_material_  = bits.back_material;
  flat_shading_   = bits.flat_shading;
}

//--------------------

void
CGLLightingBits::
save(CGL *gl)
{
  gl->save(*this);
}

void
CGLLightingBits::
restore(CGL *gl)
{
  gl->restore(*this);
}
