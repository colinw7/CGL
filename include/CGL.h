#ifndef CGL_H
#define CGL_H

#include <CRGBA.h>
#include <CValue.h>
#include <CLineDash.h>
#include <COptVal.h>
#include <CPoint3D.h>
#include <CMatrix3DH.h>
#include <CIBBox2D.h>
#include <CNPlane3D.h>
#include <CImage.h>
#include <CGeomLight3D.h>
#include <CMaterial.h>
#include <CFogData.h>
#include <CXLibPixelRenderer.h>
#include <CGL_glut.h>
#include <CGLMaterial.h>
#include <CGLColorBuffer.h>

class CGL;
class CGLFace3D;
class CGLDisplayList;
class CGLWind;
class CGLMenu;

class CGeomLight3D;
class CXLibPixelRenderer;
class CImagePixelRenderer;

#include <CGLClipPlane.h>

#include <CGLAccumBuffer.h>

#include <CGLLightModel.h>

#include <CGLColorMaterial.h>

#include <CGLBlend.h>

#include <CGLFuncs.h>

#include <CGLAttrib.h>

#include <CGLColorTable.h>
#include <CGLConvolutionFilter2D.h>
#include <CGLHistogram.h>
#include <CGLMinmax.h>

#include <CGLMap1.h>
#include <CGLMap2.h>

#include <CGLPixelStore.h>
#include <CGLTextureData.h>
#include <CGLTextureEnv.h>
#include <CGLTextureGen.h>

#include <CGLCullFace.h>

#include <CGLStencil.h>

#include <CGLFeedbackBuffer.h>
#include <CGLSelectBuffer.h>

class CGL;

struct CGLEnableBits {
  bool alpha_test;
  bool auto_normal;
  bool blend;
  bool color_material;
  bool cull_face;
  bool depth_test;
  bool dither;
  bool fog;
  bool light[8];
  bool lighting;
  bool line_smooth;
  bool line_stipple;
  bool color_logic_op;
  bool index_logic_op;
  bool map1[9];
  bool map2[9];
  bool normalize;
  bool point_smooth;
  bool polygon_offset_line;
  bool polygon_offset_fill;
  bool polygon_offset_point;
  bool polygon_stipple;
  bool polygon_smooth;
  bool scissor_test;
  bool stencil_test;
  bool texture_1d;
  bool texture_2d;
  bool texture_3d;
  bool texture_gen_q;
  bool texture_gen_r;
  bool texture_gen_s;
  bool texture_gen_t;

  void save(CGL *) { }
  void restore(CGL *) { }
};

struct CGLEvalBits {
  bool map1[9];
  bool map2[9];
  // 1d grid endpoints and divisions ?
  // 2d grid endpoints and divisions ?
  bool auto_normal;

  void save(CGL *) { }
  void restore(CGL *) { }
};

struct CGLLightingBits;

class CGLScissor {
 private:
  bool     enabled_;
  CIBBox2D rect_;

 public:
  CGLScissor() :
   enabled_(false) {
  }

  ACCESSOR(Enabled, bool    , enabled)
  ACCESSOR(Rect   , CIBBox2D, rect   )
};

class CGLAlphaTest {
 private:
  bool   enabled_;
  uint   func_;
  double value_;

 public:
  CGLAlphaTest() :
   enabled_(false), func_(GL_ALWAYS), value_(0.0) {
  }

  ACCESSOR(Enabled, bool  , enabled)
  ACCESSOR(Func   , uint  , func   )
  ACCESSOR(Value  , double, value  )

  bool eval(double alpha) const {
    switch (func_) {
      case GL_NEVER:
        return false;
      case GL_ALWAYS:
        return true;
      case GL_LESS:
        return (alpha <  value_);
      case GL_LEQUAL:
        return (alpha <= value_);
      case GL_EQUAL:
        return (alpha == value_);
      case GL_GEQUAL:
        return (alpha >= value_);
      case GL_GREATER:
        return (alpha >  value_);
      case GL_NOTEQUAL:
        return (alpha != value_);
      default:
        return false;
    }
  }
};

class CGLDepthTest {
 private:
  bool   enabled_;
  uint   func_;

 public:
  CGLDepthTest() :
   enabled_(false), func_(GL_LESS) {
  }

  ACCESSOR(Enabled, bool, enabled)
  ACCESSOR(Func   , uint, func   )

  bool eval(double old_value, double new_value) const {
    switch (func_) {
      case GL_NEVER:
        return false;
      case GL_ALWAYS:
        return false;
      case GL_LESS:
        return (new_value <  old_value);
      case GL_LEQUAL:
        return (new_value <= old_value);
      case GL_EQUAL:
        return (new_value == old_value);
      case GL_GEQUAL:
        return (new_value >= old_value);
      case GL_GREATER:
        return (new_value >  old_value);
      case GL_NOTEQUAL:
        return (new_value != old_value);
      default:
        return false;
    }
  }
};

class CGLPixelTransfer {
 private:
  double r_scale_, r_bias_;
  double g_scale_, g_bias_;
  double b_scale_, b_bias_;
  double a_scale_, a_bias_;

 public:
  CGLPixelTransfer() :
   r_scale_(1.0), r_bias_(0.0),
   g_scale_(1.0), g_bias_(0.0),
   b_scale_(1.0), b_bias_(0.0),
   a_scale_(1.0), a_bias_(0.0) {
  }

  ACCESSOR(RedScale  , double, r_scale)
  ACCESSOR(RedBias   , double, r_bias )
  ACCESSOR(GreenScale, double, g_scale)
  ACCESSOR(GreenBias , double, g_bias )
  ACCESSOR(BlueScale , double, b_scale)
  ACCESSOR(BlueBias  , double, b_bias )
  ACCESSOR(AlphaScale, double, a_scale)
  ACCESSOR(AlphaBias , double, a_bias )
};

#define CGLMgrInst CGLMgr::getInstance()

class CGLMgr {
 private:
  typedef std::map<uint,CGLWind *> WindowMap;

  CGLWind   *current_window_;
  uint       current_window_id_;
  CIBBox2D   window_rect_;
  WindowMap  window_map_;
  uint       error_num_;

 public:
  static CGLMgr *getInstance() {
    static CGLMgr *instance;

    if (! instance)
      instance = new CGLMgr;

    return instance;
  }

  CGLMgr();
 ~CGLMgr();

  CGLWind *getCurrentWindow() const { return current_window_; }

  CGL *getCurrentGL() const;

  //----

  const CIBBox2D &getWindowRect() const { return window_rect_; }

  void setWindowPosition(const CIPoint2D &point);

  CIPoint2D getWindowPosition() const;

  void setWindowSize(const CISize2D &size);

  CISize2D getWindowSize() const;

  //----

  void createWindow(const std::string &name);
  void createSubWindow(int window, int x, int y, int width, int height);

  void deleteWindow(int window);

  uint getCurrentWindowId();
  uint setCurrentWindowId(uint window);

  CGLWind *getWindowFromId(uint id) const;

  //----

  ACCESSOR(ErrorNum, uint, error_num)

  //----

  void mainLoop();

  //----

  void unimplemented(const std::string &proc);

  static const CVector3D &getDefNormal() {
    static CVector3D def_normal;

    return def_normal;
  }
};

class CGL
{
 public:
  typedef std::map<uint,CGLDisplayList *> DisplayListMap;
  typedef std::vector<CMatrix3DH *>       MatrixStack;
  typedef std::vector<CGeomVertex3D *>    VertexList;
  typedef std::vector<CGeomLight3D *>     LightList;
  typedef std::vector<CGLAttrib>          AttribStack;
  typedef std::vector<uint>               NameStack;

 private:
  enum { NUM_LIGHTS = 8 };

  enum { MAX_CLIP_PLANES = 6 };

  CGLWind                *window_;
  CBBox3D                 viewport_;
  CMatrix3DH              pixel_matrix_;
  CMatrix3DH              ipixel_matrix_;
  CGLColorBuffer         *color_buffer_;
  CGLAccumBuffer          accum_buffer_;
  uint                    render_mode_;
  CRGBA                   bg_;
  COptValT<CRGBA>         fg_;
  COptValT<CVector3D>     normal_;
  COptValT<CPoint3D>      tmap_;

  // lighting
  CGLColorMaterial        color_material_;
  CGLLightModel           light_model_;
  LightList               lights_;
  bool                    lighting_;
  CGLMaterial             front_material_;
  CGLMaterial             back_material_;
  bool                    flat_shading_;

  double                  line_width_;
  CLineDash               line_dash_;
  DisplayListMap          display_list_map_;
  CGLDisplayList         *current_display_list_;
  int                     matrix_mode_;
  MatrixStack             model_view_matrix_stack_;
  MatrixStack             model_view_imatrix_stack_;
  MatrixStack             projection_matrix_stack_;
  MatrixStack             projection_imatrix_stack_;
  MatrixStack             texture_matrix_stack_;
  MatrixStack             color_matrix_stack_;
  bool                    double_buffer_;
  double                  point_size_;
  CGLAlphaTest            alpha_test_;
  CGLDepthTest            depth_test_;
  double                  depth_near_, depth_far_;
  bool                    dither_;
  bool                    line_stipple_;
  bool                    line_smooth_;
  int                     line_smooth_hint_;
  bool                    poly_offset_fill_;
  bool                    poly_stipple_;
  CImagePtr               poly_stipple_image_;
  bool                    poly_smooth_;
  double                  poly_offset_factor_;
  double                  poly_offset_units_;
  int                     front_face_mode_;
  int                     back_face_mode_;
  CGLBlend                blend_;
  bool                    fog_enabled_;
  int                     fog_hint_;
  CFogData                fog_data_;
  CGLClipPlane            clip_plane_[MAX_CLIP_PLANES];
  int                     block_type_;
  int                     list_base_;
  VertexList              block_points_;
  CGeomVertex3D           raster_pos_;
  AttribStack             attrib_stack_;
  bool                    color_table_enabled_;
  CGLColorTable           color_table_;
  bool                    convolution_2d_enabled_;
  CGLConvolutionFilter2D  convolution_2d_;
  bool                    histogram_enabled_;
  CGLHistogram            histogram_;
  bool                    minmax_enabled_;
  CGLMinmax               minmax_;
  CGLMap1Data             map1_data_;
  std::map<uint,CGLMap1>  map1_;
  CGLMapGrid1             map_grid1_;
  CGLMap2Data             map2_data_;
  std::map<uint,CGLMap2>  map2_;
  CGLMapGrid2             map_grid2_;
  bool                    map2_auto_normal_;
  bool                    auto_normalize_;
  CGLPixelStore           pixel_store_;
  CGLPixelTransfer        pixel_transfer_;
  double                  pixel_zoom_x_, pixel_zoom_y_;
  uint                    texture_next_ind_;
  CGLTexture1Data         texture1_data_;
  CGLTexture2Data         texture2_data_;
  CGLTexture3Data         texture3_data_;
  CGLTextureEnv           texture_env_;
  CGLTextureGen           texture_gen_;
  CPolygonOrientation     front_face_orient_;
  CGLCullFace             cull_face_;
  CGLScissor              scissor_;
  CGLStencil              stencil_;
  CGLFeedbackBuffer       feedback_buffer_;
  CGLSelectBuffer         select_buffer_;
  NameStack               name_stack_;

 public:
  CGL(CGLWind *window);
 ~CGL();

  //----

  void setBackground(const CRGBA &bg);
  const CRGBA &getBackground() const { return bg_; }

  //----

  ACCESSOR(RenderMode, uint, render_mode)

  //----

  const CRGBA &getForeground() const {
    return fg_.getValue();
  }

  void setForeground(const CRGBA &fg);

  bool getForegroundSet() const {
    return fg_.isValid();
  }

  //----

  const CVector3D &getNormal() const {
    return normal_.getValue();
  }

  void setNormal(const CVector3D &normal);

  bool getNormalSet() const {
    return normal_.isValid();
  }

  //----

  const CPoint3D &getTexCoord() const {
    return tmap_.getValue();
  }

  void setTexCoord(const CPoint3D &tmap);

  bool getTexCoordSet() const {
    return tmap_.isValid();
  }

  //----

  void setMaterial(uint face, uint pname, double param);
  void setMaterialV(uint face, uint pname, double *params, uint num_params);

  void setFrontMaterial(const CGLMaterial &material) {
    front_material_ = material;
  }
  const CGLMaterial &getFrontMaterial() const {
    return front_material_;
  }

  void setBackMaterial(const CGLMaterial &material) {
    back_material_ = material;
  }
  const CGLMaterial &getBackMaterial() const {
    return back_material_;
  }

  void setFrontMaterialAmbient(const CRGBA &ambient) {
    front_material_.material.setAmbient(ambient);
  }

  void setBackMaterialAmbient(const CRGBA &ambient) {
    back_material_.material.setAmbient(ambient);
  }

  void setFrontMaterialDiffuse(const CRGBA &diffuse) {
    front_material_.material.setDiffuse(diffuse);
  }

  void setBackMaterialDiffuse(const CRGBA &diffuse) {
    back_material_.material.setDiffuse(diffuse);
  }

  void setFrontMaterialSpecular(const CRGBA &specular) {
    front_material_.material.setSpecular(specular);
  }

  void setBackMaterialSpecular(const CRGBA &specular) {
    back_material_.material.setSpecular(specular);
  }

  void setFrontMaterialEmission(const CRGBA &emission) {
    front_material_.material.setEmission(emission);
  }

  void setBackMaterialEmission(const CRGBA &emission) {
    back_material_.material.setEmission(emission);
  }

  void setFrontMaterialShininess(double shininess) {
    front_material_.material.setShininess(shininess);
  }

  void setBackMaterialShininess(double shininess) {
    back_material_.material.setShininess(shininess);
  }

  void setFrontMaterialAmbientInd(int ind) {
    front_material_.ambient_ind = ind;
  }

  void setBackMaterialAmbientInd(int ind) {
    back_material_.ambient_ind = ind;
  }

  void setFrontMaterialDiffuseInd(int ind) {
    front_material_.diffuse_ind = ind;
  }

  void setBackMaterialDiffuseInd(int ind) {
    back_material_.diffuse_ind = ind;
  }

  void setFrontMaterialSpecularInd(int ind) {
    front_material_.specular_ind = ind;
  }

  void setBackMaterialSpecularInd(int ind) {
    back_material_.specular_ind = ind;
  }

  void setLight(uint num, const CGeomLight3D &light) {
    *lights_[num] = light;
  }

  const CGeomLight3D &getLight(uint num) {
    return *lights_[num];
  }

  void setLightEnabled(uint num, bool flag) {
    lights_[num]->setEnabled(flag);
  }

  bool getLightEnabled(uint num) {
    return lights_[num]->getEnabled();
  }

  void setLightPosition(uint num, const CPoint3D &point) {
    lights_[num]->setAbsPosition(point);
  }

  void setLightAmbient(uint num, const CRGBA &rgba) {
    lights_[num]->setAmbient(rgba);
  }

  void setLightDiffuse(uint num, const CRGBA &rgba) {
    lights_[num]->setDiffuse(rgba);
  }

  void setLightSpecular(uint num, const CRGBA &rgba) {
    lights_[num]->setSpecular(rgba);
  }

  void setLightSpotDirection(uint num, const CVector3D &direction) {
    lights_[num]->setSpotDirection(direction);
  }

  void setLightSpotExponent(uint num, double exponent) {
    lights_[num]->setSpotExponent(exponent);
  }

  void setLightSpotCutOff(uint num, double cutoff) {
    lights_[num]->setSpotCutOff(cutoff);
  }

  void setLightDirectional(uint num, bool flag) {
    lights_[num]->setDirectional(flag);
  }

  bool getLightDirectional(uint num) {
    return lights_[num]->getDirectional();
  }

  void setLightConstantAttenuation(uint num, double attenuation) {
    lights_[num]->setConstantAttenuation(attenuation);
  }

  void setLightLinearAttenuation(uint num, double attenuation) {
    lights_[num]->setLinearAttenuation(attenuation);
  }

  void setLightQuadraticAttenuation(uint num, double attenuation) {
    lights_[num]->setQuadraticAttenuation(attenuation);
  }

  void setLightModelAmbient(const CRGBA &rgba) {
    light_model_.ambient = rgba;
  }

  const CRGBA &getLightModelAmbient() const {
    return light_model_.ambient;
  }

  void setLightModelLocalViewer(bool local) {
    light_model_.local_viewer = local;
  }

  void setLightModelTwoSide(bool two_side) {
    light_model_.two_side = two_side;
  }

  void setLightModelColorControl(int mode) {
    light_model_.color_control = mode;
  }

  ACCESSOR(ColorMaterial, CGLColorMaterial, color_material)

  ACCESSOR(LineWidth, double, line_width)

  void setLineDash(const CLineDash &line_dash) { line_dash_ = line_dash; }

  const CLineDash &getLineDash() const { return line_dash_; }

  ACCESSOR(MatrixMode, int, matrix_mode)

  bool getDoubleBuffer() const { return double_buffer_; }
  void setDoubleBuffer(bool flag);

  ACCESSOR(AlphaTest, CGLAlphaTest, alpha_test)
  ACCESSOR(DepthTest, CGLDepthTest, depth_test)

  ACCESSOR(DepthNear, double, depth_near)
  ACCESSOR(DepthFar , double, depth_far )

  ACCESSOR(Dither, bool, dither)

  ACCESSOR(LineStipple, bool, line_stipple)

  ACCESSOR(LineSmooth    , bool, line_smooth)
  ACCESSOR(LineSmoothHint, int , line_smooth_hint)

  bool getPolyOffsetFill() const { return poly_offset_fill_; }
  void setPolyOffsetFill(bool flag) { poly_offset_fill_ = flag; }

  bool getPolyStipple() const { return poly_stipple_; }
  void setPolyStipple(bool flag) { poly_stipple_ = flag; }

  void setPolyStippleImage(const CImagePtr &image) {
    poly_stipple_image_ = image;
  }

  const CImagePtr &getPolyStippleImage() const {
    return poly_stipple_image_;
  }

  ACCESSOR(PolySmooth, bool, poly_smooth)

  ACCESSOR(PolyOffsetFactor, double, poly_offset_factor)

  double getPolyOffsetUnits() const { return poly_offset_units_; }
  void setPolyOffsetUnits(double value) { poly_offset_units_ = value; }

  int getFrontFaceMode() const { return front_face_mode_; }
  void setFrontFaceMode(int mode) { front_face_mode_ = mode; }

  int getBackFaceMode() const { return back_face_mode_; }
  void setBackFaceMode(int mode) { back_face_mode_ = mode; }

  bool getLighting() const { return lighting_; }
  void setLighting(bool flag);

  bool getBlending() const { return blend_.enabled; }
  void setBlending(bool flag) { blend_.enabled = flag; }

  CRGBACombineMode getBlendSrcType() const { return blend_.src_type; }
  void setBlendSrcType(CRGBACombineMode type) { blend_.src_type = type; }

  CRGBACombineMode getBlendDstType() const { return blend_.dst_type; }
  void setBlendDstType(CRGBACombineMode type) { blend_.dst_type = type; }

  const CRGBA &getBlendColor() const { return blend_.color; }
  void setBlendColor(const CRGBA &color) { blend_.color = color; }

  CRGBACombineFunc getBlendFunc() const { return blend_.func; }
  void setBlendFunc(CRGBACombineFunc func) { blend_.func = func; }

  ACCESSOR(FogEnabled, bool, fog_enabled)

  ACCESSOR(FogHint, int, fog_hint)

  const CFogData &getFogData() const { return fog_data_; }
  void setFogData(CFogData &fog_data) { fog_data_ = fog_data; }

  CFogType getFogType() const { return fog_data_.type; }
  void setFogType(CFogType type) { fog_data_.type = type; }

  double getFogStart() const { return fog_data_.start; }
  void setFogStart(double start) { fog_data_.start = start; }

  double getFogEnd() const { return fog_data_.end; }
  void setFogEnd(double end) { fog_data_.end = end; }

  double getFogDensity() const { return fog_data_.density; }
  void setFogDensity(double density) { fog_data_.density = density; }

  const CRGBA &getFogColor() const { return fog_data_.color; }
  void setFogColor(const CRGBA &color) { fog_data_.color = color; }

  ACCESSOR(FlatShading, bool  , flat_shading)
  ACCESSOR(PointSize  , double, point_size  )
  ACCESSOR(ListBase   , int   , list_base   )

  void clearWindow(void);

  void beginBlock(int mode);
  void endBlock();
  bool inBlock();

  void addBlockPoint(const CPoint3D &point);
  void addBlockPoint(const CPoint3D &point, const CRGBA &rgba,
                     const CVector3D &normal, const CPoint3D &tmap);
  void addBlockPoint(const CPoint3D &point, const COptValT<CRGBA> &rgba,
                     const COptValT<CVector3D> &normal, const COptValT<CPoint3D> &tmap);

  ACCESSOR(RasterPos, CGeomVertex3D, raster_pos)

  void updateRasterPos(const CPoint3D &point);

  void drawPoint(CGeomVertex3D *vertex);
  void drawLine(CGeomVertex3D *vertex1, CGeomVertex3D *vertex2, bool first);
  void drawPolygon(uint mode, const VertexList &vertices);

  void transformVertex(CGeomVertex3D *vertex);

  void untransformVertex(CGeomVertex3D *vertex);

  void resize(int w, int h);

  void updateSize(int width, int height, bool *changed);

  void eventExpose();

  CGLColorBuffer &getColorBuffer() { return *color_buffer_; }

  CGLAccumBuffer &getAccumBuffer() { return accum_buffer_; }

  bool executeCommand() const;

  int  createDisplayList();
  int  createDisplayList(uint id, bool execute);
  bool startDisplayList(uint id, bool execute);
  bool endDisplayList();
  bool executeDisplayList(uint id);
  bool inDisplayList() const;
  bool isDisplayList(uint id);

  void deleteDisplayList(uint id);

  CGLDisplayList *getCurrentDisplayList() const {
    return current_display_list_;
  }

  bool pushMatrix();
  bool popMatrix();

  void updateIMatrix(CMatrix3DH *matrix);

  void rotateMatrix(double angle, double x, double y, double z);
  void scaleMatrix(double x, double y, double z);
  void translateMatrix(double x, double y, double z);

  void initMatrix();

  void setMatrix(CMatrix3DH *matrix);

  void multMatrix(CMatrix3DH *matrix);

  CMatrix3DH *getMatrix(int mode) const;
  CMatrix3DH *getIMatrix(int mode) const;
  CMatrix3DH *getMatrix() const;
  CMatrix3DH *getIMatrix() const;

  void drawBitmap(uint width, uint height, double xorig, double yorig,
                  double xmove, double ymove, const uchar *bitmap);
  void drawBitmapI(uint width, uint height, double xorig, double yorig,
                  double xmove, double ymove, const uchar *bitmap);

  void drawRGBAImage(uint width, uint height, const uchar *image,
                     uint num_components, bool reverse);

  void copyColorImage(int x, int y, uint width, uint height, uint num_components);

  void readColorImage(int x, int y, uint width, uint height,
                      uchar *pixels, uint num_components, bool process);

  void calcHistogram(const CRGBA *image, uint width, uint height);

  void calcMinmax(const CRGBA *image, uint width, uint height);

  void setClipPlane(int pid, const double *equation);

  void enableClipPlane(int pid);
  void disableClipPlane(int pid);

  bool clipPlaneEnabled(int pid) const {
    return clip_plane_[pid].getEnabled();
  }

  const CGLClipPlane *getClipPlane(int pid) const { return &clip_plane_[pid]; }

  CClipSide getClipSide(int pid, const CPoint3D &point) const;

  void setViewport(double  x1, double  y1, double  x2, double  y2);
  void getViewport(double *x1, double *y1, double *x2, double *y2);

  int getPixelWidth();
  int getPixelHeight();

  template<typename T>
  void
  pushAttrib(uint type, const T &value) {
    CGLAttrib attrib;

    attrib.type  = type;
    attrib.value = new CValueT<T>(value);

    attrib_stack_.push_back(attrib);
  }

  bool
  peekAttribType(uint *type) {
    if (attrib_stack_.empty())
      return false;

    CGLAttrib attrib = attrib_stack_.back();

    *type = attrib.type;

    return true;
  }

  template<typename T>
  T
  popAttrib() {
    CGLAttrib attrib = attrib_stack_.back();

    attrib_stack_.pop_back();

    T value = static_cast<CValueT<T> *>(attrib.value)->getValue();

    delete attrib.value;

    return value;
  }

  ACCESSOR(ColorTableEnabled, bool         , color_table_enabled)
  ACCESSOR(ColorTable       , CGLColorTable, color_table        )

  ACCESSOR(Convolution2DEnabled, bool                  , convolution_2d_enabled)
  ACCESSOR(Convolution2D       , CGLConvolutionFilter2D, convolution_2d        )

  ACCESSOR(HistogramEnabled, bool        , histogram_enabled)
  ACCESSOR(Histogram       , CGLHistogram, histogram        )

  ACCESSOR(MinmaxEnabled, bool     , minmax_enabled)
  ACCESSOR(Minmax       , CGLMinmax, minmax        )

  ACCESSOR(Map1Data, CGLMap1Data, map1_data)

  bool hasMap1(uint target) const {
    return (map1_.find(target) != map1_.end());
  }

  const CGLMap1 &getMap1(uint target) {
    return map1_[target];
  }

  void setMap1(uint target, const CGLMap1 &map1) {
    map1_[target] = map1;
  }

  ACCESSOR(MapGrid1, CGLMapGrid1, map_grid1)

  ACCESSOR(Map2Data, CGLMap2Data, map2_data)

  bool hasMap2(uint target) const {
    return (map2_.find(target) != map2_.end());
  }

  const CGLMap2 &getMap2(uint target) {
    return map2_[target];
  }

  void setMap2(uint target, const CGLMap2 &map2) {
    map2_[target] = map2;
  }

  ACCESSOR(MapGrid2, CGLMapGrid2, map_grid2)

  ACCESSOR(Map2AutoNormal, bool, map2_auto_normal)

  ACCESSOR(AutoNormalize, bool, auto_normalize)

  ACCESSOR(PixelStore, CGLPixelStore, pixel_store)

  ACCESSOR(PixelTransfer, CGLPixelTransfer, pixel_transfer)

  ACCESSOR(PixelZoomX, double, pixel_zoom_x)
  ACCESSOR(PixelZoomY, double, pixel_zoom_y)

  ACCESSOR(TextureNextInd, uint, texture_next_ind)

  ACCESSOR(Texture1Data, CGLTexture1Data, texture1_data)
  ACCESSOR(Texture2Data, CGLTexture2Data, texture2_data)
  ACCESSOR(Texture3Data, CGLTexture3Data, texture3_data)

  ACCESSOR(TextureEnv, CGLTextureEnv, texture_env)
  ACCESSOR(TextureGen, CGLTextureGen, texture_gen)

  ACCESSOR(FrontFaceOrient, CPolygonOrientation, front_face_orient)

  ACCESSOR(CullFace, CGLCullFace, cull_face)

  ACCESSOR(Scissor, CGLScissor, scissor)

  ACCESSOR(Stencil, CGLStencil, stencil)

  ACCESSOR(FeedbackBuffer, CGLFeedbackBuffer, feedback_buffer)

  ACCESSOR(SelectBuffer, CGLSelectBuffer, select_buffer)

  ACCESSOR(NameStack, NameStack, name_stack)

  void initNames() {
    name_stack_.clear();
  }

  void pushName(uint name) {
    name_stack_.push_back(name);
  }

  void popName() {
    name_stack_.pop_back();
  }

  void loadName(uint name) {
    assert(! name_stack_.empty());

    name_stack_[name_stack_.size() - 1] = name;
  }

  uint numNames() { return uint(name_stack_.size()); }

  void drawChar(char c);
  void setFont(CFontPtr font);

  void accumLoadAdd(double scale);
  void accumLoad(double scale);
  void accumUnload(double scale);
  void accumAdd(double factor);
  void accumMult(double factor);

  void save(CGLLightingBits &bits) const;
  void restore(const CGLLightingBits &bits);

 private:
  void initLights();
  void initMaterials();

  CGLDisplayList *lookupDisplayList(uint id);
};

//---------

struct CGLLightingBits {
  CGLColorMaterial color_material;
  CGLLightModel    light_model;
  CGL::LightList   lights;
  bool             lighting;
  CGLMaterial      front_material;
  CGLMaterial      back_material;
  bool             flat_shading;

  void save(CGL *gl);
  void restore(CGL *gl);
};

#endif
