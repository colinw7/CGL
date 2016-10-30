#include <CGL_gl.h>
#include <CGL.h>
#include <CGLWind.h>
#include <CRGBA.h>
#include <CFuncs.h>
#include <CStrUtil.h>
#include <CPoint3D.h>
#include <CMatrix3DH.h>
#include <CImageLib.h>
#include <bool_ops.h>
#include <climits>

#define GL_VERTEX_ARRAY_DATA(t,i) \
  ((gl_vertex_array_stride == 0) ? \
   ((t *)(gl_vertex_array_pointer + ((i)*gl_vertex_array_size*sizeof(t)))) : \
   ((t *)(gl_vertex_array_pointer + (i)*gl_vertex_array_stride)))

#define GL_COLOR_ARRAY_DATA(t,i) \
  ((gl_color_array_stride == 0) ? \
   ((t *)(gl_color_array_pointer + ((i)*gl_color_array_size*sizeof(t)))) : \
   ((t *)(gl_color_array_pointer + (i)*gl_color_array_stride)))

static bool    gl_vertex_array_flag    = false;
static int     gl_vertex_array_size    = 4;
static GLenum  gl_vertex_array_type    = GL_FLOAT;
static int     gl_vertex_array_stride  = 0;
static char   *gl_vertex_array_pointer = NULL;

static bool    gl_color_array_flag    = false;
static int     gl_color_array_size    = 4;
static GLenum  gl_color_array_type    = GL_FLOAT;
static int     gl_color_array_stride  = 0;
static char   *gl_color_array_pointer = NULL;

static bool    gl_index_array_flag         = false;
static bool    gl_normal_array_flag        = false;
static bool    gl_texture_coord_array_flag = false;
static bool    gl_edge_flag_array_flag     = false;

static CGLMaterial gl_back_material;

static void   gli_ArrayElementVertex(GLint i);
static void   gli_ArrayElementColor(GLint i);
static void   gli_Color3cv(const char *c);
static void   gli_Color4cv(const char *c);
static bool   gli_InterleavedArraysIsTexture(GLenum format);
static bool   gli_InterleavedArraysIsColor(GLenum format);
static bool   gli_InterleavedArraysIsNormal(GLenum format);
static GLint  gli_InterleavedArraysGetTextureSize(GLenum format);
static GLint  gli_InterleavedArraysGetColorSize(GLenum format);
static GLint  gli_InterleavedArraysGetVertexSize(GLenum format);
static GLenum gli_InterleavedArraysGetColorType(GLenum format);
static GLint  gli_InterleavedArraysGetColorOffset(GLenum format);
static GLint  gli_InterleavedArraysGetNormalOffset(GLenum format);
static GLint  gli_InterleavedArraysGetVertexOffset(GLenum format);
static GLint  gli_InterleavedArraysGetStride(GLenum format);
static void   gli_Vertex2cv(const char *c);
static void   gli_Vertex3cv(const char *c);
static void   gli_Vertex4cv(const char *c);

static void
gli_SetError(int error)
{
  std::cerr << "GL_ERROR " << error << std::endl;

  CGLMgrInst->setErrorNum(error);
}

static void
gli_ArrayElementVertex(GLint i)
{
  if      (gl_vertex_array_type == GL_BYTE) {
    char *data = GL_VERTEX_ARRAY_DATA(char, i);

    if      (gl_vertex_array_size == 2)
      gli_Vertex2cv(data);
    else if (gl_vertex_array_size == 3)
      gli_Vertex3cv(data);
    else if (gl_vertex_array_size == 4)
      gli_Vertex4cv(data);
  }
  else if (gl_vertex_array_type == GL_UNSIGNED_BYTE) {
    uchar *data = GL_VERTEX_ARRAY_DATA(uchar, i);

    if      (gl_vertex_array_size == 2)
      gli_Vertex2cv((char *) data);
    else if (gl_vertex_array_size == 3)
      gli_Vertex3cv((char *) data);
    else if (gl_vertex_array_size == 4)
      gli_Vertex4cv((char *) data);
  }
  else if (gl_vertex_array_type == GL_SHORT) {
    GLshort *data = GL_VERTEX_ARRAY_DATA(GLshort, i);

    if      (gl_vertex_array_size == 2)
      glVertex2sv(data);
    else if (gl_vertex_array_size == 3)
      glVertex3sv(data);
    else if (gl_vertex_array_size == 4)
      glVertex4sv(data);
  }
  else if (gl_vertex_array_type == GL_UNSIGNED_SHORT) {
    GLushort *data = GL_VERTEX_ARRAY_DATA(GLushort, i);

    if      (gl_vertex_array_size == 2)
      glVertex2sv((GLshort *) data);
    else if (gl_vertex_array_size == 3)
      glVertex3sv((GLshort *) data);
    else if (gl_vertex_array_size == 4)
      glVertex4sv((GLshort *) data);
  }
  else if (gl_vertex_array_type == GL_INT) {
    GLint *data = GL_VERTEX_ARRAY_DATA(GLint, i);

    if      (gl_vertex_array_size == 2)
      glVertex2iv(data);
    else if (gl_vertex_array_size == 3)
      glVertex3iv(data);
    else if (gl_vertex_array_size == 4)
      glVertex4iv(data);
  }
  else if (gl_vertex_array_type == GL_UNSIGNED_INT) {
    GLuint *data = GL_VERTEX_ARRAY_DATA(GLuint, i);

    if      (gl_vertex_array_size == 2)
      glVertex2iv((GLint *) data);
    else if (gl_vertex_array_size == 3)
      glVertex3iv((GLint *) data);
    else if (gl_vertex_array_size == 4)
      glVertex4iv((GLint *) data);
  }
  else if (gl_vertex_array_type == GL_FLOAT) {
    GLfloat *data = GL_VERTEX_ARRAY_DATA(GLfloat, i);

    if      (gl_vertex_array_size == 2)
      glVertex2fv(data);
    else if (gl_vertex_array_size == 3)
      glVertex3fv(data);
    else if (gl_vertex_array_size == 4)
      glVertex4fv(data);
  }
  else if (gl_vertex_array_type == GL_DOUBLE) {
    GLdouble *data = GL_VERTEX_ARRAY_DATA(GLdouble, i);

    if      (gl_vertex_array_size == 2)
      glVertex2dv(data);
    else if (gl_vertex_array_size == 3)
      glVertex3dv(data);
    else if (gl_vertex_array_size == 4)
      glVertex4dv(data);
  }
}

static void
gli_ArrayElementColor(GLint i)
{
  if      (gl_color_array_type == GL_BYTE) {
    char *data = GL_COLOR_ARRAY_DATA(char, i);

    if      (gl_color_array_size == 3)
      gli_Color3cv(data);
    else if (gl_color_array_size == 4)
      gli_Color4cv(data);
  }
  else if (gl_color_array_type == GL_UNSIGNED_BYTE) {
    uchar *data = GL_COLOR_ARRAY_DATA(uchar, i);

    if      (gl_color_array_size == 3)
      gli_Color3cv((char *) data);
    else if (gl_color_array_size == 4)
      gli_Color4cv((char *) data);
  }
  else if (gl_color_array_type == GL_SHORT) {
    GLshort *data = GL_COLOR_ARRAY_DATA(GLshort, i);

    if      (gl_color_array_size == 3)
      glColor3sv(data);
    else if (gl_color_array_size == 4)
      glColor4sv(data);
  }
  else if (gl_color_array_type == GL_UNSIGNED_SHORT) {
    GLushort *data = GL_COLOR_ARRAY_DATA(GLushort, i);

    if      (gl_color_array_size == 3)
      glColor3sv((GLshort *) data);
    else if (gl_color_array_size == 4)
      glColor4sv((GLshort *) data);
  }
  else if (gl_color_array_type == GL_INT) {
    GLint *data = GL_COLOR_ARRAY_DATA(GLint, i);

    if      (gl_color_array_size == 3)
      glColor3iv(data);
    else if (gl_color_array_size == 4)
      glColor4iv(data);
  }
  else if (gl_color_array_type == GL_UNSIGNED_INT) {
    GLuint *data = GL_COLOR_ARRAY_DATA(GLuint, i);

    if      (gl_color_array_size == 3)
      glColor3iv((GLint *) data);
    else if (gl_color_array_size == 4)
      glColor4iv((GLint *) data);
  }
  else if (gl_color_array_type == GL_FLOAT) {
    GLfloat *data = GL_COLOR_ARRAY_DATA(GLfloat, i);

    if      (gl_color_array_size == 3)
      glColor3fv(data);
    else if (gl_color_array_size == 4)
      glColor4fv(data);
  }
  else if (gl_color_array_type == GL_DOUBLE) {
    GLdouble *data = GL_COLOR_ARRAY_DATA(GLdouble, i);

    if      (gl_color_array_size == 3)
      glColor3dv(data);
    else if (gl_color_array_size == 4)
      glColor4dv(data);
  }
}

static void
gli_Color3cv(const char *c)
{
  double factor = 1.0/255.0;

  glColor4d(c[0]*factor, c[1]*factor, c[2]*factor, 1.0);
}

static void
gli_Color4cv(const char *c)
{
  glColor4d(CMathGen::mapToReal(c[0]),
            CMathGen::mapToReal(c[1]),
            CMathGen::mapToReal(c[2]),
            CMathGen::mapToReal(c[3]));
}

static bool
gli_InterleavedArraysIsTexture(GLenum format)
{
  return (format == GL_T2F_V3F         ||
          format == GL_T4F_V4F         ||
          format == GL_T2F_C4UB_V3F    ||
          format == GL_T2F_C3F_V3F     ||
          format == GL_T2F_N3F_V3F     ||
          format == GL_T2F_C4F_N3F_V3F ||
          format == GL_T4F_C4F_N3F_V4F);
}

static bool
gli_InterleavedArraysIsColor(GLenum format)
{
  return (format == GL_C4UB_V2F        ||
          format == GL_C4UB_V3F        ||
          format == GL_C3F_V3F         ||
          format == GL_C4F_N3F_V3F     ||
          format == GL_T2F_C4UB_V3F    ||
          format == GL_T2F_C3F_V3F     ||
          format == GL_T2F_C4F_N3F_V3F ||
          format == GL_T4F_C4F_N3F_V4F);
}

static bool
gli_InterleavedArraysIsNormal(GLenum format)
{
  return (format == GL_N3F_V3F         ||
          format == GL_C4F_N3F_V3F     ||
          format == GL_T2F_N3F_V3F     ||
          format == GL_T2F_C4F_N3F_V3F ||
          format == GL_T4F_C4F_N3F_V4F);
}

static GLint
gli_InterleavedArraysGetTextureSize(GLenum format)
{
  switch (format) {
    case GL_T2F_V3F:
      return 2;
    case GL_T4F_V4F:
      return 4;
    case GL_T2F_C4UB_V3F:
      return 2;
    case GL_T2F_C3F_V3F:
      return 2;
    case GL_T2F_N3F_V3F:
      return 2;
    case GL_T2F_C4F_N3F_V3F:
      return 2;
    case GL_T4F_C4F_N3F_V4F:
      return 4;
  }

  return 0;
}

static GLint
gli_InterleavedArraysGetColorSize(GLenum format)
{
  switch (format) {
    case GL_C4UB_V2F:
      return 4;
    case GL_C4UB_V3F:
      return 4;
    case GL_C3F_V3F:
      return 3;
    case GL_C4F_N3F_V3F:
      return 4;
    case GL_T2F_C4UB_V3F:
      return 4;
    case GL_T2F_C3F_V3F:
      return 3;
    case GL_T2F_C4F_N3F_V3F:
      return 4;
    case GL_T4F_C4F_N3F_V4F:
      return 4;
  }

  return 0;
}

static GLint
gli_InterleavedArraysGetVertexSize(GLenum format)
{
  switch (format) {
    case GL_V2F:
      return 2;
    case GL_V3F:
      return 3;
    case GL_C4UB_V2F:
      return 2;
    case GL_C4UB_V3F:
      return 3;
    case GL_C3F_V3F:
      return 3;
    case GL_N3F_V3F:
      return 3;
    case GL_C4F_N3F_V3F:
      return 3;
    case GL_T2F_V3F:
      return 3;
    case GL_T4F_V4F:
      return 4;
    case GL_T2F_C4UB_V3F:
      return 3;
    case GL_T2F_C3F_V3F:
      return 3;
    case GL_T2F_N3F_V3F:
      return 3;
    case GL_T2F_C4F_N3F_V3F:
      return 3;
    case GL_T4F_C4F_N3F_V4F:
      return 4;
  }

  return 0;
}

static GLenum
gli_InterleavedArraysGetColorType(GLenum format)
{
  switch (format) {
    case GL_C4UB_V2F:
      return GL_UNSIGNED_BYTE;
    case GL_C4UB_V3F:
      return GL_UNSIGNED_BYTE;
    case GL_C3F_V3F:
      return GL_FLOAT;
    case GL_C4F_N3F_V3F:
      return GL_FLOAT;
    case GL_T2F_C4UB_V3F:
      return GL_UNSIGNED_BYTE;
    case GL_T2F_C3F_V3F:
      return GL_FLOAT;
    case GL_T2F_C4F_N3F_V3F:
      return GL_FLOAT;
    case GL_T4F_C4F_N3F_V4F:
      return GL_FLOAT;
  }

  return 0;
}

static GLint
gli_InterleavedArraysGetColorOffset(GLenum format)
{
  switch (format) {
    case GL_C4UB_V2F:
      return 0;
    case GL_C4UB_V3F:
      return 0;
    case GL_C3F_V3F:
      return 0;
    case GL_C4F_N3F_V3F:
      return 0;
    case GL_T2F_C4UB_V3F:
      return 2*sizeof(GLfloat);
    case GL_T2F_C3F_V3F:
      return 2*sizeof(GLfloat);
    case GL_T2F_C4F_N3F_V3F:
      return 2*sizeof(GLfloat);
    case GL_T4F_C4F_N3F_V4F:
      return 4*sizeof(GLfloat);
  }

  return 0;
}

static GLint
gli_InterleavedArraysGetNormalOffset(GLenum format)
{
  switch (format) {
    case GL_N3F_V3F:
      return 0;
    case GL_C4F_N3F_V3F:
      return 4*sizeof(GLfloat);
    case GL_T2F_N3F_V3F:
      return 2*sizeof(GLfloat);
    case GL_T2F_C4F_N3F_V3F:
      return 6*sizeof(GLfloat);
    case GL_T4F_C4F_N3F_V4F:
      return 8*sizeof(GLfloat);
  }

  return 0;
}

static GLint
gli_InterleavedArraysGetVertexOffset(GLenum format)
{
  switch (format) {
    case GL_V2F:
      return 0;
    case GL_V3F:
      return 0;
    case GL_C4UB_V2F:
      return 4*sizeof(GLubyte);
    case GL_C4UB_V3F:
      return 4*sizeof(GLubyte);
    case GL_C3F_V3F:
      return 3*sizeof(GLfloat);
    case GL_N3F_V3F:
      return 3*sizeof(GLfloat);
    case GL_C4F_N3F_V3F:
      return 7*sizeof(GLfloat);
    case GL_T2F_V3F:
      return 2*sizeof(GLfloat);
    case GL_T4F_V4F:
      return 4*sizeof(GLfloat);
    case GL_T2F_C4UB_V3F:
      return 4*sizeof(GLubyte) + 2*sizeof(GLfloat);
    case GL_T2F_C3F_V3F:
      return 5*sizeof(GLfloat);
    case GL_T2F_N3F_V3F:
      return 5*sizeof(GLfloat);
    case GL_T2F_C4F_N3F_V3F:
      return 9*sizeof(GLfloat);
    case GL_T4F_C4F_N3F_V4F:
      return 11*sizeof(GLfloat);
  }

  return 0;
}

static GLint
gli_InterleavedArraysGetStride(GLenum format)
{
  switch (format) {
    case GL_V2F:
      return 2*sizeof(GLfloat);
    case GL_V3F:
      return 3*sizeof(GLfloat);
    case GL_C4UB_V2F:
      return 4*sizeof(GLubyte) + 2*sizeof(GLfloat);
    case GL_C4UB_V3F:
      return 4*sizeof(GLubyte) + 3*sizeof(GLfloat);
    case GL_C3F_V3F:
      return 6*sizeof(GLfloat);
    case GL_N3F_V3F:
      return 6*sizeof(GLfloat);
    case GL_C4F_N3F_V3F:
      return 10*sizeof(GLfloat);
    case GL_T2F_V3F:
      return 5*sizeof(GLfloat);
    case GL_T4F_V4F:
      return 8*sizeof(GLfloat);
    case GL_T2F_C4UB_V3F:
      return 4*sizeof(GLubyte) + 5*sizeof(GLfloat);
    case GL_T2F_C3F_V3F:
      return 8*sizeof(GLfloat);
    case GL_T2F_N3F_V3F:
      return 8*sizeof(GLfloat);
    case GL_T2F_C4F_N3F_V3F:
      return 12*sizeof(GLfloat);
    case GL_T4F_C4F_N3F_V4F:
      return 15*sizeof(GLfloat);
  }

  return 0;
}

static void
gli_Vertex2cv(const char *c)
{
  glVertex3d(c[0], c[1], 0.0);
}

static void
gli_Vertex3cv(const char *c)
{
  glVertex3d(c[0], c[1], c[2]);
}

static void
gli_Vertex4cv(const char *c)
{
  glVertex4d(c[0], c[1], c[2], c[3]);
}

static uint
gli_ParamDim(GLenum pname)
{
  switch (pname) {
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_AMBIENT_AND_DIFFUSE:
    case GL_SPECULAR:
    case GL_EMISSION:
    case GL_POSITION:
    case GL_LIGHT_MODEL_AMBIENT:
    case GL_TEXTURE_ENV_COLOR:
    case GL_OBJECT_PLANE:
    case GL_EYE_PLANE:
      return 4;
    case GL_COLOR_INDEXES:
    case GL_SPOT_DIRECTION:
      return 3;
    case GL_SHININESS:
    default:
      return 1;
  }
}

////////////////////////////////////

void
glAccum(GLenum op, GLfloat value)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  if      (op == GL_ACCUM)
    gl->accumLoadAdd(value);
  else if (op == GL_LOAD)
    gl->accumLoad(value);
  else if (op == GL_ADD)
    gl->accumAdd(value);
  else if (op == GL_MULT)
    gl->accumMult(value);
  else if (op == GL_RETURN)
    gl->accumUnload(value);
  else {
    gli_SetError(GL_INVALID_ENUM);
    return;
  }
}

void
glActiveTextureARB(GLenum /*texture*/)
{
  CGLMgrInst->unimplemented("glActiveTextureARB");
}

void
glAlphaFunc(GLenum func, GLclampf ref)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  gl->modifyAlphaTest().setFunc (func);
  gl->modifyAlphaTest().setValue(ref);
}

GLboolean
glAreTexturesResident(GLsizei /*n*/, const GLuint * /*textures*/, GLboolean * /*residences*/)
{
  CGLMgrInst->unimplemented("glActiveTextureARB");

  return false;
}

void
glArrayElement(GLint i)
{
  if (gl_color_array_flag)
    gli_ArrayElementColor(i);

  if (gl_index_array_flag)
    CGLMgrInst->unimplemented("glArrayElement:GL_INDEX_ARRAY");

  if (gl_normal_array_flag)
    CGLMgrInst->unimplemented("glArrayElement:GL_NORMAL_ARRAY");

  if (gl_texture_coord_array_flag)
    CGLMgrInst->unimplemented("glArrayElement:GL_TEXTURE_COORD_ARRAY");

  if (gl_edge_flag_array_flag)
    CGLMgrInst->unimplemented("glArrayElement:GL_EDGE_FLAG_ARRAY");

  if (gl_vertex_array_flag)
    gli_ArrayElementVertex(i);
}

void
glBegin(GLenum mode)
{
  static int all_modes =
    GL_POINTS | GL_LINES | GL_LINE_STRIP | GL_LINE_LOOP |
    GL_TRIANGLES | GL_TRIANGLE_STRIP | GL_TRIANGLE_FAN |
    GL_QUADS | GL_QUAD_STRIP | GL_POLYGON;

  CGL *gl = CGLMgrInst->getCurrentGL();

  if (! gl->inDisplayList()) {
    if ((mode & ~all_modes) != 0) {
      gli_SetError(GL_INVALID_ENUM);
      return;
    }

    if (gl->inBlock()) {
      gli_SetError(GL_INVALID_OPERATION);
      return;
    }
  }

  gl->beginBlock(mode);
}

void
glBindTexture(GLenum target, GLuint texture)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (target == GL_TEXTURE_1D)
    gl->modifyTexture1Data().setCurrentInd(texture);
  else if (target == GL_TEXTURE_2D)
    gl->modifyTexture2Data().setCurrentInd(texture);
  else if (target == GL_TEXTURE_3D)
    gl->modifyTexture3Data().setCurrentInd(texture);
  else
    CGLMgrInst->unimplemented("glBindTexture");
}

void
glBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig,
         GLfloat xmove, GLfloat ymove, const GLubyte *bitmap)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->drawBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);
}

void
glBlendColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  CRGBA rgba(r, g, b, a);

  rgba.clamp();

  gl->setBlendColor(rgba);
}

void
glBlendEquation(GLenum mode)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  switch (mode) {
    case GL_FUNC_ADD:
      gl->setBlendFunc(CRGBA_COMBINE_ADD);
      break;
    case GL_FUNC_SUBTRACT:
      gl->setBlendFunc(CRGBA_COMBINE_SUBTRACT);
      break;
    case GL_FUNC_REVERSE_SUBTRACT:
      gl->setBlendFunc(CRGBA_COMBINE_REVERSE_SUBTRACT);
      break;
    case GL_MIN:
      gl->setBlendFunc(CRGBA_COMBINE_MIN);
      break;
    case GL_MAX:
      gl->setBlendFunc(CRGBA_COMBINE_MAX);
      break;
    default:
      gli_SetError(GL_INVALID_ENUM);
      break;
  }
}

void
glBlendFunc(GLenum sfactor, GLenum dfactor)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  switch (sfactor) {
    case GL_ZERO:
      gl->setBlendSrcType(CRGBA_COMBINE_ZERO);
      break;
    case GL_ONE:
      gl->setBlendSrcType(CRGBA_COMBINE_ONE);
      break;
    case GL_DST_COLOR:
      gl->setBlendSrcType(CRGBA_COMBINE_DST_COLOR);
      break;
    case GL_ONE_MINUS_DST_COLOR:
      gl->setBlendSrcType(CRGBA_COMBINE_ONE_MINUS_DST_COLOR);
      break;
    case GL_SRC_ALPHA:
      gl->setBlendSrcType(CRGBA_COMBINE_SRC_ALPHA);
      break;
    case GL_ONE_MINUS_SRC_ALPHA:
      gl->setBlendSrcType(CRGBA_COMBINE_ONE_MINUS_SRC_ALPHA);
      break;
    case GL_DST_ALPHA:
      gl->setBlendSrcType(CRGBA_COMBINE_DST_ALPHA);
      break;
    case GL_ONE_MINUS_DST_ALPHA:
      gl->setBlendSrcType(CRGBA_COMBINE_ONE_MINUS_DST_ALPHA);
      break;
    case GL_SRC_ALPHA_SATURATE:
      gl->setBlendSrcType(CRGBA_COMBINE_SRC_ALPHA_SATURATE);
      break;
    case GL_CONSTANT_COLOR:
      gl->setBlendSrcType(CRGBA_COMBINE_CONSTANT_COLOR);
      break;
    case GL_ONE_MINUS_CONSTANT_COLOR:
      gl->setBlendSrcType(CRGBA_COMBINE_ONE_MINUS_CONSTANT_COLOR);
      break;
    case GL_CONSTANT_ALPHA:
      gl->setBlendSrcType(CRGBA_COMBINE_CONSTANT_ALPHA);
      break;
    case GL_ONE_MINUS_CONSTANT_ALPHA:
      gl->setBlendSrcType(CRGBA_COMBINE_ONE_MINUS_CONSTANT_ALPHA);
      break;
    default:
      gli_SetError(GL_INVALID_ENUM);
      return;
  }

  switch (dfactor) {
    case GL_ZERO:
      gl->setBlendDstType(CRGBA_COMBINE_ZERO);
      break;
    case GL_ONE:
      gl->setBlendDstType(CRGBA_COMBINE_ONE);
      break;
    case GL_SRC_COLOR:
      gl->setBlendDstType(CRGBA_COMBINE_SRC_COLOR);
      break;
    case GL_ONE_MINUS_SRC_COLOR:
      gl->setBlendDstType(CRGBA_COMBINE_ONE_MINUS_SRC_COLOR);
      break;
    case GL_SRC_ALPHA:
      gl->setBlendDstType(CRGBA_COMBINE_SRC_ALPHA);
      break;
    case GL_ONE_MINUS_SRC_ALPHA:
      gl->setBlendDstType(CRGBA_COMBINE_ONE_MINUS_SRC_ALPHA);
      break;
    case GL_DST_ALPHA:
      gl->setBlendDstType(CRGBA_COMBINE_DST_ALPHA);
      break;
    case GL_ONE_MINUS_DST_ALPHA:
      gl->setBlendDstType(CRGBA_COMBINE_ONE_MINUS_DST_ALPHA);
      break;
    case GL_CONSTANT_COLOR:
      gl->setBlendDstType(CRGBA_COMBINE_CONSTANT_COLOR);
      break;
    case GL_ONE_MINUS_CONSTANT_COLOR:
      gl->setBlendDstType(CRGBA_COMBINE_ONE_MINUS_CONSTANT_COLOR);
      break;
    case GL_CONSTANT_ALPHA:
      gl->setBlendDstType(CRGBA_COMBINE_CONSTANT_ALPHA);
      break;
    case GL_ONE_MINUS_CONSTANT_ALPHA:
      gl->setBlendDstType(CRGBA_COMBINE_ONE_MINUS_CONSTANT_ALPHA);
      break;
    default:
      gli_SetError(GL_INVALID_ENUM);
      return;
  }
}

void
glCallList(GLuint list)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->executeDisplayList(list + gl->getListBase());
}

void
glCallLists(GLsizei n, GLenum type, const GLvoid *lists)
{
  if      (type == GL_BYTE) {
    for (int i = 0; i < n; ++i)
      glCallList(((GLbyte *) lists)[i]);
  }
  else if (type == GL_UNSIGNED_BYTE) {
    for (int i = 0; i < n; ++i)
      glCallList(((GLubyte *) lists)[i]);
  }
  else if (type == GL_SHORT) {
    for (int i = 0; i < n; ++i)
      glCallList(((GLshort *) lists)[i]);
  }
  else if (type == GL_UNSIGNED_SHORT) {
    for (int i = 0; i < n; ++i)
      glCallList(((GLushort *) lists)[i]);
  }
  else if (type == GL_INT) {
    for (int i = 0; i < n; ++i)
      glCallList(((GLint *) lists)[i]);
  }
  else if (type == GL_UNSIGNED_INT) {
    for (int i = 0; i < n; ++i)
      glCallList(((GLuint *) lists)[i]);
  }
  else if (type == GL_FLOAT) {
    for (int i = 0; i < n; ++i)
      glCallList(int(((GLfloat *) lists)[i]));
  }
  else
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glCallLists %d %d", n, type));
}

void
glClear(GLbitfield mask)
{
  static GLbitfield all_masks =
    (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  |
     GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  if ((mask & ~all_masks) != 0) {
    gli_SetError(GL_INVALID_VALUE);
    return;
  }

  if (mask & GL_COLOR_BUFFER_BIT)
    gl->getColorBuffer().clearColor();

  if (mask & GL_DEPTH_BUFFER_BIT)
    gl->getColorBuffer().clearDepth();

  if (mask & GL_ACCUM_BUFFER_BIT)
    gl->getAccumBuffer().clear();

  if (mask & GL_STENCIL_BUFFER_BIT)
    gl->getColorBuffer().clearStencil();
}

void
glClearAccum(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  CRGBA rgba(r, g, b, a);

  rgba.clamp(-1, 1);

  gl->getAccumBuffer().setClearColor(rgba);
}

void
glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  CRGBA rgba(r, g, b, a);

  rgba.clamp();

  gl->getColorBuffer().setClearColor(rgba);
}

void
glClearDepth(GLclampd depth)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  depth = std::min(std::max(depth,0.0),1.0);

  gl->getColorBuffer().setClearDepth(depth);
}

void
glClearIndex(GLfloat ind)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  // TODO: clamp
  gl->getColorBuffer().setClearIndex(uint(ind));
}

void
glClearStencil(GLint s)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  // TODO: clamp
  gl->getColorBuffer().setStencilClearValue(s);
}

void
glClientActiveTextureARB(GLenum /*texture*/)
{
  CGLMgrInst->unimplemented("glClientActiveTextureARB");
}

void
glClipPlane(GLenum plane, const GLdouble *equation)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (plane == GL_CLIP_PLANE0)
    gl->setClipPlane(0, equation);
  else if (plane == GL_CLIP_PLANE1)
    gl->setClipPlane(1, equation);
  else if (plane == GL_CLIP_PLANE2)
    gl->setClipPlane(2, equation);
  else if (plane == GL_CLIP_PLANE3)
    gl->setClipPlane(3, equation);
  else if (plane == GL_CLIP_PLANE4)
    gl->setClipPlane(4, equation);
  else if (plane == GL_CLIP_PLANE5)
    gl->setClipPlane(5, equation);
}

void
glColor3b(GLbyte r, GLbyte g, GLbyte b)
{
  glColor4d(CMathGen::mapToReal(r),
            CMathGen::mapToReal(g),
            CMathGen::mapToReal(b), 1.0);
}

void
glColor3d(GLdouble r, GLdouble g, GLdouble b)
{
  glColor4d(r, g, b, 1.0);
}

void
glColor3dv(const GLdouble *c)
{
  glColor4d(c[0], c[1], c[2], 1.0);
}

void
glColor3f(GLfloat r, GLfloat g, GLfloat b)
{
  glColor4d(r, g, b, 1.0);
}

void
glColor3fv(const GLfloat *c)
{
  glColor4d(c[0], c[1], c[2], 1.0);
}

void
glColor3i(GLint r, GLint g, GLint b)
{
  glColor4d(CMathGen::mapToReal(r),
            CMathGen::mapToReal(g),
            CMathGen::mapToReal(b), 1.0);
}

void
glColor3iv(const GLint *c)
{
  glColor4d(CMathGen::mapToReal(c[0]),
            CMathGen::mapToReal(c[1]),
            CMathGen::mapToReal(c[2]), 1.0);
}

void
glColor3s(GLshort r, GLshort g, GLshort b)
{
  glColor4d(CMathGen::mapToReal(r),
            CMathGen::mapToReal(g),
            CMathGen::mapToReal(b), 1.0);
}

void
glColor3ub(GLubyte r, GLubyte g, GLubyte b)
{
  glColor4d(CMathGen::mapToReal(r),
            CMathGen::mapToReal(g),
            CMathGen::mapToReal(b), 1.0);
}

void
glColor3ubv(const GLubyte *c)
{
  glColor4d(CMathGen::mapToReal(c[0]),
            CMathGen::mapToReal(c[1]),
            CMathGen::mapToReal(c[2]), 1.0);
}

void
glColor3sv(const GLshort *c)
{
  glColor4d(CMathGen::mapToReal(c[0]),
            CMathGen::mapToReal(c[1]),
            CMathGen::mapToReal(c[2]), 1.0);
}

void
glColor4d(GLdouble r, GLdouble g, GLdouble b, GLdouble a)
{
  CRGBA rgba(r, g, b, a);

  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->getColorMaterial().getEnabled()) {
    std::vector<GLfloat> params = { (GLfloat) r, (GLfloat) g, (GLfloat) b, (GLfloat) a };

    glMaterialfv(gl->getColorMaterial().getFace(),
                 gl->getColorMaterial().getMode(),
                 &params[0]);
  }
  else
    gl->setForeground(rgba);
}

void
glColor4dv(const GLdouble *c)
{
  glColor4d(c[0], c[1], c[2], c[3]);
}

void
glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
  glColor4d(r, g, b, a);
}

void
glColor4fv(const GLfloat *c)
{
  glColor4d(c[0], c[1], c[2], c[3]);
}

void
glColor4i(GLint r, GLint g, GLint b, GLint a)
{
  glColor4d(CMathGen::mapToReal(r),
            CMathGen::mapToReal(g),
            CMathGen::mapToReal(b),
            CMathGen::mapToReal(a));
}

void
glColor4iv(const GLint *c)
{
  glColor4d(CMathGen::mapToReal(c[0]),
            CMathGen::mapToReal(c[1]),
            CMathGen::mapToReal(c[2]),
            CMathGen::mapToReal(c[3]));
}

void
glColor4s(GLshort r, GLshort g, GLshort b, GLshort a)
{
  glColor4d(CMathGen::mapToReal(r),
            CMathGen::mapToReal(g),
            CMathGen::mapToReal(b),
            CMathGen::mapToReal(a));
}

void
glColor4sv(const GLshort *c)
{
  glColor4d(CMathGen::mapToReal(c[0]),
            CMathGen::mapToReal(c[1]),
            CMathGen::mapToReal(c[2]),
            CMathGen::mapToReal(c[3]));
}

void
glColor4ub(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
  glColor4d(CMathGen::mapToReal(r),
            CMathGen::mapToReal(g),
            CMathGen::mapToReal(b),
            CMathGen::mapToReal(a));
}

void
glColor4ubv(const GLubyte *c)
{
  glColor4d(CMathGen::mapToReal(c[0]),
            CMathGen::mapToReal(c[1]),
            CMathGen::mapToReal(c[2]),
            CMathGen::mapToReal(c[3]));
}

void
glColorMask(GLboolean /*red*/, GLboolean /*green*/, GLboolean /*blue*/, GLboolean /*alpha*/)
{
  CGLMgrInst->unimplemented("glColorMask");
}

void
glColorMaterial(GLenum face, GLenum mode)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (! gl) return;

  gl->modifyColorMaterial().setFace(face);
  gl->modifyColorMaterial().setMode(mode);
}

void
glColorPointer(GLint size, GLenum type, GLsizei stride,
               const GLvoid *pointer)
{
  gl_color_array_size    = size;
  gl_color_array_type    = type;
  gl_color_array_stride  = stride;
  gl_color_array_pointer = (char *) pointer;
}

void
glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum buffer)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (buffer == GL_COLOR)
    gl->copyColorImage(x, y, width, height, 4);
  else
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glCopyPixels"));
}

void
glColorTable(GLenum /*target*/, GLenum internalFormat, GLsizei width,
             GLenum format, GLenum type, const GLvoid *data)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CGLColorTable color_table;

  color_table.setType(internalFormat);
  color_table.setSize(width);

  if (format == GL_RGB) {
    if (type == GL_UNSIGNED_BYTE) {
      double f = 1.0/255.0;

      CRGBA rgba;

      uchar *p = (uchar *) data;

      for (int i = 0; i < width; ++i, p += 3) {
        rgba = CRGBA(p[0]*f, p[1]*f, p[2]*f);

        // scale and bias

        rgba.clamp();

        if (internalFormat == GL_RGB)
          color_table.setValue(i, rgba);
        else
          CGLMgrInst->unimplemented(CStrUtil::strprintf("glColorTable"));
      }

      gl->setColorTable(color_table);
    }
    else
      CGLMgrInst->unimplemented("glColorTable");
  }
  else
    CGLMgrInst->unimplemented("glColorTable");
}

void
glConvolutionFilter2D(GLenum /*target*/, GLenum internalFormat, GLsizei width,
                      GLsizei height, GLenum format, GLenum type,
                      const GLvoid *image)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CGLConvolutionFilter2D filter;

  filter.setType(internalFormat);
  filter.setSize(width, height);

  if (format == GL_LUMINANCE) {
    if (type == GL_FLOAT) {
      float *p = (float *) image;

      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x, ++p) {
          if (internalFormat == GL_LUMINANCE)
            filter.setValue(x, y, CRGBA(*p,*p,*p,1));
          else
            CGLMgrInst->unimplemented("glConvolutionFilter2D");
        }
      }

      gl->setConvolution2D(filter);
    }
    else
      CGLMgrInst->unimplemented("glConvolutionFilter2D");
  }
  else
    CGLMgrInst->unimplemented("glConvolutionFilter2D");
}

void
glConvolutionParameteri(GLenum /*target*/, GLenum /*pname*/, GLint /*value*/)
{
  CGLMgrInst->unimplemented("glConvolutionParameteri");
}

void
glCopyTexImage2D(GLenum /*target*/, GLint /*level*/, GLenum /*internalFormat*/,
                 GLint /*x*/, GLint /*y*/, GLsizei /*width*/, GLsizei /*height*/,
                 GLint /*border*/)
{
  CGLMgrInst->unimplemented("glCopyTexImage2D");
}

void
glCullFace(GLenum mode)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->modifyCullFace().setFront(false);
  gl->modifyCullFace().setBack (false);

  if      (mode == GL_FRONT)
    gl->modifyCullFace().setFront(true);
  else if (mode == GL_BACK )
    gl->modifyCullFace().setBack (true);
  else if (mode == GL_FRONT_AND_BACK) {
    gl->modifyCullFace().setFront(true);
    gl->modifyCullFace().setBack (true);
  }
  else
    CGLMgrInst->unimplemented("glCullFace");
}

void
glDeleteLists(GLuint list, GLsizei range)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  for (int i = 0; i < range; ++i)
    gl->deleteDisplayList(list + i);
}

void
glDeleteTextures(GLsizei /*n*/, const GLuint * /*textures*/)
{
  CGLMgrInst->unimplemented("glDeleteTextures");
}

void
glDepthFunc(GLenum func)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->modifyDepthTest().setFunc(func);
}

void
glDepthMask(GLboolean flag)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->getColorBuffer().setDepthWritable(flag);
}

void
glDepthRange(GLclampd near_val, GLclampd far_val)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setDepthNear(near_val);
  gl->setDepthFar (far_val);
}

void
glDisable(GLenum cap)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (cap == GL_ALPHA_TEST)
    gl->modifyAlphaTest().setEnabled(false);
  else if (cap == GL_AUTO_NORMAL)
    gl->setMap2AutoNormal(false);
  else if (cap == GL_BLEND)
    gl->setBlending(false);
  else if (cap == GL_CLIP_PLANE0)
    gl->disableClipPlane(0);
  else if (cap == GL_CLIP_PLANE1)
    gl->disableClipPlane(1);
  else if (cap == GL_CLIP_PLANE2)
    gl->disableClipPlane(2);
  else if (cap == GL_CLIP_PLANE3)
    gl->disableClipPlane(3);
  else if (cap == GL_CLIP_PLANE4)
    gl->disableClipPlane(4);
  else if (cap == GL_CLIP_PLANE5)
    gl->disableClipPlane(5);
  else if (cap == GL_COLOR_MATERIAL)
    gl->modifyColorMaterial().setEnabled(false);
  else if (cap == GL_COLOR_TABLE)
    gl->setColorTableEnabled(false);
  else if (cap == GL_CONVOLUTION_2D)
    gl->setConvolution2DEnabled(false);
  else if (cap == GL_CULL_FACE)
    gl->modifyCullFace().setEnabled(false);
  else if (cap == GL_DEPTH_TEST)
    gl->modifyDepthTest().setEnabled(false);
  else if (cap == GL_DITHER)
    gl->setDither(false);
  else if (cap == GL_FOG)
    gl->setFogEnabled(false);
  else if (cap == GL_HISTOGRAM)
    gl->setHistogramEnabled(false);
  else if (cap == GL_LIGHTING)
    gl->setLighting(false);
  else if (cap == GL_LIGHT0)
    gl->setLightEnabled(0, false);
  else if (cap == GL_LIGHT1)
    gl->setLightEnabled(1, false);
  else if (cap == GL_LIGHT2)
    gl->setLightEnabled(2, false);
  else if (cap == GL_LIGHT3)
    gl->setLightEnabled(3, false);
  else if (cap == GL_LIGHT4)
    gl->setLightEnabled(4, false);
  else if (cap == GL_LIGHT5)
    gl->setLightEnabled(5, false);
  else if (cap == GL_LIGHT6)
    gl->setLightEnabled(6, false);
  else if (cap == GL_LIGHT7)
    gl->setLightEnabled(7, false);
  else if (cap == GL_LINE_SMOOTH)
    gl->setLineSmooth(false);
  else if (cap == GL_LINE_STIPPLE)
    gl->setLineStipple(false);
  else if (cap == GL_MAP1_COLOR_4)
    gl->modifyMap1Data().setGenColor(false);
  else if (cap == GL_MAP1_INDEX)
    gl->modifyMap1Data().setGenIndex(false);
  else if (cap == GL_MAP1_NORMAL)
    gl->modifyMap1Data().setGenNormal(false);
  else if (cap == GL_MAP1_TEXTURE_COORD_1)
    gl->modifyMap1Data().setGenTexture1(false);
  else if (cap == GL_MAP1_TEXTURE_COORD_2)
    gl->modifyMap1Data().setGenTexture2(false);
  else if (cap == GL_MAP1_TEXTURE_COORD_3)
    gl->modifyMap1Data().setGenTexture3(false);
  else if (cap == GL_MAP1_TEXTURE_COORD_4)
    gl->modifyMap1Data().setGenTexture4(false);
  else if (cap == GL_MAP1_VERTEX_3)
    gl->modifyMap1Data().setGenVertex3(false);
  else if (cap == GL_MAP1_VERTEX_4)
    gl->modifyMap1Data().setGenVertex4(false);
  else if (cap == GL_MAP2_COLOR_4)
    gl->modifyMap2Data().setGenColor(false);
  else if (cap == GL_MAP2_INDEX)
    gl->modifyMap2Data().setGenIndex(false);
  else if (cap == GL_MAP2_NORMAL)
    gl->modifyMap2Data().setGenNormal(false);
  else if (cap == GL_MAP2_TEXTURE_COORD_1)
    gl->modifyMap2Data().setGenTexture1(false);
  else if (cap == GL_MAP2_TEXTURE_COORD_2)
    gl->modifyMap2Data().setGenTexture2(false);
  else if (cap == GL_MAP2_TEXTURE_COORD_3)
    gl->modifyMap2Data().setGenTexture3(false);
  else if (cap == GL_MAP2_TEXTURE_COORD_4)
    gl->modifyMap2Data().setGenTexture4(false);
  else if (cap == GL_MAP2_VERTEX_3)
    gl->modifyMap2Data().setGenVertex3(false);
  else if (cap == GL_MAP2_VERTEX_4)
    gl->modifyMap2Data().setGenVertex4(false);
  else if (cap == GL_MINMAX)
    gl->setMinmaxEnabled(false);
  else if (cap == GL_NORMALIZE)
    gl->setAutoNormalize(false);
  else if (cap == GL_POLYGON_OFFSET_FILL)
    gl->setPolyOffsetFill(false);
  else if (cap == GL_POLYGON_SMOOTH)
    gl->setPolySmooth(false);
  else if (cap == GL_POLYGON_STIPPLE)
    gl->setPolyStipple(false);
  else if (cap == GL_SCISSOR_TEST)
    gl->modifyScissor().setEnabled(false);
  else if (cap == GL_STENCIL_TEST)
    gl->modifyStencil().setEnabled(false);
  else if (cap == GL_TEXTURE_1D)
    gl->modifyTexture1Data().setEnabled(false);
  else if (cap == GL_TEXTURE_2D)
    gl->modifyTexture2Data().setEnabled(false);
  else if (cap == GL_TEXTURE_3D)
    gl->modifyTexture3Data().setEnabled(false);
  else if (cap == GL_TEXTURE_GEN_Q)
    gl->modifyTextureGen().setQEnabled(false);
  else if (cap == GL_TEXTURE_GEN_R)
    gl->modifyTextureGen().setREnabled(false);
  else if (cap == GL_TEXTURE_GEN_S)
    gl->modifyTextureGen().setSEnabled(false);
  else if (cap == GL_TEXTURE_GEN_T)
    gl->modifyTextureGen().setTEnabled(false);
  else
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glDisable %d", cap));
}

void
glDisableClientState(GLenum array)
{
  if      (array == GL_VERTEX_ARRAY)
    gl_vertex_array_flag = false;
  else if (array == GL_COLOR_ARRAY)
    gl_color_array_flag = false;
  else if (array == GL_INDEX_ARRAY)
    gl_index_array_flag = false;
  else if (array == GL_NORMAL_ARRAY)
    gl_normal_array_flag = false;
  else if (array == GL_TEXTURE_COORD_ARRAY)
    gl_texture_coord_array_flag = false;
  else if (array == GL_EDGE_FLAG_ARRAY)
    gl_edge_flag_array_flag = false;
}

void
glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  glBegin(mode);

  for (int i = 0; i < count; ++i)
    glArrayElement(first + i);

  glEnd();
}

void
glDrawBuffer(GLenum /*mode*/)
{
  CGLMgrInst->unimplemented("glDrawBuffer");
}

void
glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
  glBegin(mode);

  if      (type == GL_UNSIGNED_BYTE) {
    for (int i = 0; i < count; ++i)
      glArrayElement(((GLubyte *) indices)[i]);
  }
  else if (type == GL_UNSIGNED_SHORT) {
    for (int i = 0; i < count; ++i)
      glArrayElement(((GLushort *) indices)[i]);
  }
  else if (type == GL_UNSIGNED_INT) {
    for (int i = 0; i < count; ++i)
      glArrayElement(((GLuint *) indices)[i]);
  }

  glEnd();
}

void
glDrawPixels(GLsizei width, GLsizei height, GLenum format,
             GLenum type, const GLvoid *pixels)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (format == GL_RGB  || format == GL_BGR ) {
    if (type == GL_UNSIGNED_BYTE)
      gl->drawRGBAImage(width, height, (uchar *) pixels, 3, format == GL_RGB);
    else
      CGLMgrInst->unimplemented(CStrUtil::strprintf("glDrawPixels"));
  }
  else if (format == GL_RGBA || format == GL_BGRA) {
    if (type == GL_UNSIGNED_BYTE)
      gl->drawRGBAImage(width, height, (uchar *) pixels, 4, format == GL_RGBA);
    else
      CGLMgrInst->unimplemented(CStrUtil::strprintf("glDrawPixels"));
  }
  else
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glDrawPixels"));
}

void
glEnable(GLenum cap)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (! gl) return;

  if      (cap == GL_ALPHA_TEST)
    gl->modifyAlphaTest().setEnabled(true);
  else if (cap == GL_AUTO_NORMAL)
    gl->setMap2AutoNormal(true);
  else if (cap == GL_BLEND)
    gl->setBlending(true);
  else if (cap == GL_CLIP_PLANE0)
    gl->enableClipPlane(0);
  else if (cap == GL_CLIP_PLANE1)
    gl->enableClipPlane(1);
  else if (cap == GL_CLIP_PLANE2)
    gl->enableClipPlane(2);
  else if (cap == GL_CLIP_PLANE3)
    gl->enableClipPlane(3);
  else if (cap == GL_CLIP_PLANE4)
    gl->enableClipPlane(4);
  else if (cap == GL_CLIP_PLANE5)
    gl->enableClipPlane(5);
  else if (cap == GL_COLOR_MATERIAL)
    gl->modifyColorMaterial().setEnabled(true);
  else if (cap == GL_COLOR_TABLE)
    gl->setColorTableEnabled(true);
  else if (cap == GL_CONVOLUTION_2D)
    gl->setConvolution2DEnabled(true);
  else if (cap == GL_CULL_FACE)
    gl->modifyCullFace().setEnabled(true);
  else if (cap == GL_DEPTH_TEST)
    gl->modifyDepthTest().setEnabled(true);
  else if (cap == GL_DITHER)
    gl->setDither(true);
  else if (cap == GL_FOG)
    gl->setFogEnabled(true);
  else if (cap == GL_HISTOGRAM)
    gl->setHistogramEnabled(true);
  else if (cap == GL_LIGHTING)
    gl->setLighting(true);
  else if (cap == GL_LIGHT0)
    gl->setLightEnabled(0, true);
  else if (cap == GL_LIGHT1)
    gl->setLightEnabled(1, true);
  else if (cap == GL_LIGHT2)
    gl->setLightEnabled(2, true);
  else if (cap == GL_LIGHT3)
    gl->setLightEnabled(3, true);
  else if (cap == GL_LIGHT4)
    gl->setLightEnabled(4, true);
  else if (cap == GL_LIGHT5)
    gl->setLightEnabled(5, true);
  else if (cap == GL_LIGHT6)
    gl->setLightEnabled(6, true);
  else if (cap == GL_LIGHT7)
    gl->setLightEnabled(7, true);
  else if (cap == GL_LINE_SMOOTH)
    gl->setLineSmooth(true);
  else if (cap == GL_LINE_STIPPLE)
    gl->setLineStipple(true);
  else if (cap == GL_MAP1_COLOR_4)
    gl->modifyMap1Data().setGenColor(true);
  else if (cap == GL_MAP1_INDEX)
    gl->modifyMap1Data().setGenIndex(true);
  else if (cap == GL_MAP1_NORMAL)
    gl->modifyMap1Data().setGenNormal(true);
  else if (cap == GL_MAP1_TEXTURE_COORD_1)
    gl->modifyMap1Data().setGenTexture1(true);
  else if (cap == GL_MAP1_TEXTURE_COORD_2)
    gl->modifyMap1Data().setGenTexture2(true);
  else if (cap == GL_MAP1_TEXTURE_COORD_3)
    gl->modifyMap1Data().setGenTexture3(true);
  else if (cap == GL_MAP1_TEXTURE_COORD_4)
    gl->modifyMap1Data().setGenTexture4(true);
  else if (cap == GL_MAP1_VERTEX_3)
    gl->modifyMap1Data().setGenVertex3(true);
  else if (cap == GL_MAP1_VERTEX_4)
    gl->modifyMap1Data().setGenVertex4(true);
  else if (cap == GL_MAP2_COLOR_4)
    gl->modifyMap2Data().setGenColor(true);
  else if (cap == GL_MAP2_INDEX)
    gl->modifyMap2Data().setGenIndex(true);
  else if (cap == GL_MAP2_NORMAL)
    gl->modifyMap2Data().setGenNormal(true);
  else if (cap == GL_MAP2_TEXTURE_COORD_1)
    gl->modifyMap2Data().setGenTexture1(true);
  else if (cap == GL_MAP2_TEXTURE_COORD_2)
    gl->modifyMap2Data().setGenTexture2(true);
  else if (cap == GL_MAP2_TEXTURE_COORD_3)
    gl->modifyMap2Data().setGenTexture3(true);
  else if (cap == GL_MAP2_TEXTURE_COORD_4)
    gl->modifyMap2Data().setGenTexture4(true);
  else if (cap == GL_MAP2_VERTEX_3)
    gl->modifyMap2Data().setGenVertex3(true);
  else if (cap == GL_MAP2_VERTEX_4)
    gl->modifyMap2Data().setGenVertex4(true);
  else if (cap == GL_MINMAX)
    gl->setMinmaxEnabled(true);
  else if (cap == GL_NORMALIZE)
    gl->setAutoNormalize(true);
  else if (cap == GL_POLYGON_OFFSET_FILL)
    gl->setPolyOffsetFill(true);
  else if (cap == GL_POLYGON_SMOOTH)
    gl->setPolySmooth(true);
  else if (cap == GL_POLYGON_STIPPLE)
    gl->setPolyStipple(true);
  else if (cap == GL_SCISSOR_TEST)
    gl->modifyScissor().setEnabled(true);
  else if (cap == GL_STENCIL_TEST)
    gl->modifyStencil().setEnabled(true);
  else if (cap == GL_TEXTURE_1D)
    gl->modifyTexture1Data().setEnabled(true);
  else if (cap == GL_TEXTURE_2D)
    gl->modifyTexture2Data().setEnabled(true);
  else if (cap == GL_TEXTURE_3D)
    gl->modifyTexture3Data().setEnabled(true);
  else if (cap == GL_TEXTURE_GEN_Q)
    gl->modifyTextureGen().setQEnabled(true);
  else if (cap == GL_TEXTURE_GEN_R)
    gl->modifyTextureGen().setREnabled(true);
  else if (cap == GL_TEXTURE_GEN_S)
    gl->modifyTextureGen().setSEnabled(true);
  else if (cap == GL_TEXTURE_GEN_T)
    gl->modifyTextureGen().setTEnabled(true);
  else
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glEnable %d", cap));
}

void
glEnableClientState(GLenum array)
{
  if      (array == GL_VERTEX_ARRAY)
    gl_vertex_array_flag = true;
  else if (array == GL_COLOR_ARRAY)
    gl_color_array_flag = true;
  else if (array == GL_INDEX_ARRAY)
    gl_index_array_flag = true;
  else if (array == GL_NORMAL_ARRAY)
    gl_normal_array_flag = true;
  else if (array == GL_TEXTURE_COORD_ARRAY)
    gl_texture_coord_array_flag = true;
  else if (array == GL_EDGE_FLAG_ARRAY)
    gl_edge_flag_array_flag = true;
  else
    CGLMgrInst->unimplemented(
      CStrUtil::strprintf("glEnableClientState %d", array));
}

void
glEnd()
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (! gl->inDisplayList()) {
    if (! gl->inBlock()) {
      gli_SetError(GL_INVALID_OPERATION);
      return;
    }
  }

  gl->endBlock();
}

void
glEndList()
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->endDisplayList();
}

void
glEvalCoord1d(GLdouble u)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  COptValT<CRGBA>     rgba;
  COptValT<CVector3D> normal;
  COptValT<CPoint3D>  tmap;

  if (gl->getMap1Data().getGenColor() &&
      gl->hasMap1(GL_MAP1_COLOR_4)) {
    const CGLMap1 &map = gl->getMap1(GL_MAP1_COLOR_4);

    double r, g, b, a;

    map.calcBernstein4(u, &r, &g, &b, &a);

    rgba = CRGBA(r, g, b, a);
  }

  if (gl->getMap1Data().getGenIndex() &&
      gl->hasMap1(GL_MAP1_INDEX)) {
    CGLMgrInst->unimplemented("glEvalCoord1d GL_MAP1_INDEX");
  }

  if (gl->getMap1Data().getGenNormal() &&
      gl->hasMap1(GL_MAP1_NORMAL)) {
    const CGLMap1 &map = gl->getMap1(GL_MAP1_NORMAL);

    double x, y, z;

    map.calcBernstein3(u, &x, &y, &z);

    normal = CVector3D(x, y, z);
  }

  if      (gl->getMap1Data().getGenTexture4() &&
           gl->hasMap1(GL_MAP1_TEXTURE_COORD_4)) {
    const CGLMap1 &map = gl->getMap1(GL_MAP1_TEXTURE_COORD_4);

    double x, y, z, w;

    map.calcBernstein4(u, &x, &y, &z, &w);

    tmap = CPoint3D(x/w, y/w, z/w);
  }
  else if (gl->getMap1Data().getGenTexture3() &&
           gl->hasMap1(GL_MAP1_TEXTURE_COORD_3)) {
    const CGLMap1 &map = gl->getMap1(GL_MAP1_TEXTURE_COORD_3);

    double x, y, z;

    map.calcBernstein3(u, &x, &y, &z);

    tmap = CPoint3D(x, y, z);
  }
  else if (gl->getMap1Data().getGenTexture2() &&
           gl->hasMap1(GL_MAP1_TEXTURE_COORD_2)) {
    const CGLMap1 &map = gl->getMap1(GL_MAP1_TEXTURE_COORD_2);

    double x, y;

    map.calcBernstein2(u, &x, &y);

    tmap = CPoint3D(x, y, 0);
  }
  else if (gl->getMap1Data().getGenTexture1() &&
           gl->hasMap1(GL_MAP1_TEXTURE_COORD_1)) {
    const CGLMap1 &map = gl->getMap1(GL_MAP1_TEXTURE_COORD_1);

    double x;

    map.calcBernstein1(u, &x);

    tmap = CPoint3D(x, 0, 0);
  }

  if      (gl->getMap1Data().getGenVertex4() &&
           gl->hasMap1(GL_MAP1_VERTEX_4)) {
    const CGLMap1 &map = gl->getMap1(GL_MAP1_VERTEX_4);

    double x, y, z, w;

    map.calcBernstein4(u, &x, &y, &z, &w);

    gl->addBlockPoint(CPoint3D(x/w, y/w, z/w), rgba, normal, tmap);
  }
  else if (gl->getMap1Data().getGenVertex3() &&
           gl->hasMap1(GL_MAP1_VERTEX_3)) {
    const CGLMap1 &map = gl->getMap1(GL_MAP1_VERTEX_3);

    double x, y, z;

    map.calcBernstein3(u, &x, &y, &z);

    gl->addBlockPoint(CPoint3D(x, y, z), rgba, normal, tmap);
  }
}

void
glEvalCoord1f(GLfloat u)
{
  glEvalCoord1d(u);
}

void
glEvalCoord2d(GLdouble u, GLdouble v)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  COptValT<CRGBA>     rgba;
  COptValT<CVector3D> normal;
  COptValT<CPoint3D>  tmap;

  if (gl->getMap2Data().getGenColor() &&
      gl->hasMap2(GL_MAP2_COLOR_4)) {
    const CGLMap2 &map = gl->getMap2(GL_MAP2_COLOR_4);

    double r, g, b, a;

    map.calcBernstein4(u, v, &r, &g, &b, &a);

    rgba = CRGBA(r, g, b, a);
  }

  if (gl->getMap2Data().getGenIndex() &&
      gl->hasMap2(GL_MAP2_INDEX)) {
    CGLMgrInst->unimplemented("glEvalCoord1d GL_MAP2_INDEX");
  }

  if (gl->getMap2Data().getGenNormal() &&
      gl->hasMap2(GL_MAP2_NORMAL)) {
    const CGLMap2 &map = gl->getMap2(GL_MAP2_NORMAL);

    double x, y, z;

    map.calcBernstein3(u, v, &x, &y, &z);

    normal = CVector3D(x, y, z);
  }

  if      (gl->getMap2Data().getGenTexture4() &&
           gl->hasMap2(GL_MAP2_TEXTURE_COORD_4)) {
    const CGLMap2 &map = gl->getMap2(GL_MAP2_TEXTURE_COORD_4);

    double x, y, z, w;

    map.calcBernstein4(u, v, &x, &y, &z, &w);

    tmap = CPoint3D(x/w, y/w, z/w);
  }
  else if (gl->getMap2Data().getGenTexture3() &&
           gl->hasMap2(GL_MAP2_TEXTURE_COORD_3)) {
    const CGLMap2 &map = gl->getMap2(GL_MAP2_TEXTURE_COORD_3);

    double x, y, z;

    map.calcBernstein3(u, v, &x, &y, &z);

    tmap = CPoint3D(x, y, z);
  }
  else if (gl->getMap2Data().getGenTexture2() &&
           gl->hasMap2(GL_MAP2_TEXTURE_COORD_2)) {
    const CGLMap2 &map = gl->getMap2(GL_MAP2_TEXTURE_COORD_2);

    double x, y;

    map.calcBernstein2(u, v, &x, &y);

    tmap = CPoint3D(x, y, 0);
  }
  else if (gl->getMap2Data().getGenTexture1() &&
           gl->hasMap2(GL_MAP2_TEXTURE_COORD_1)) {
    const CGLMap2 &map = gl->getMap2(GL_MAP2_TEXTURE_COORD_1);

    double x;

    map.calcBernstein1(u, v, &x);

    tmap = CPoint3D(x, 0, 0);
  }

  if (! rgba.isValid())
    rgba = CRGBA(0,0,0);

  if      (gl->getMap2Data().getGenVertex4() &&
           gl->hasMap2(GL_MAP2_VERTEX_4)) {
    const CGLMap2 &map = gl->getMap2(GL_MAP2_VERTEX_4);

    double x, y, z, w;

    map.calcBernstein4(u, v, &x, &y, &z, &w);

    gl->addBlockPoint(CPoint3D(x/w, y/w, z/w), rgba, normal, tmap);
  }
  else if (gl->getMap2Data().getGenVertex3() &&
           gl->hasMap2(GL_MAP2_VERTEX_3)) {
    const CGLMap2 &map = gl->getMap2(GL_MAP2_VERTEX_3);

    double x, y, z;

    map.calcBernstein3(u, v, &x, &y, &z);

    gl->addBlockPoint(CPoint3D(x, y, z), rgba, normal, tmap);
  }
}

void
glEvalCoord2f(GLfloat u, GLfloat v)
{
  glEvalCoord2d(u, v);
}

void
glEvalMesh1(GLenum mode, GLint i1, GLint i2)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  const CGLMapGrid1 &map = gl->getMapGrid1();

  double u1 = map.getU1(); double u2 = map.getU2(); int un = map.getUN();

  double du = (u2 - u1)/un;

  glBegin(mode == GL_POINT ? GL_POINTS : GL_LINES);

  for (int i = i1; i <= i2; ++i)
    glEvalCoord1d(i*du + u1);

  glEnd();
}

void
glEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  const CGLMapGrid2 &map = gl->getMapGrid2();

  double u1 = map.getU1(); double u2 = map.getU2(); int un = map.getUN();
  double v1 = map.getV1(); double v2 = map.getV2(); int vn = map.getVN();

  double du = (u2 - u1)/un;
  double dv = (v2 - v1)/vn;

  if      (mode == GL_FILL) {
    for (int j = j1; j <= j2; ++j) {
      glBegin(GL_QUAD_STRIP);

      for (int i = i1; i <= i2; ++i) {
        glEvalCoord2d(i*du + u1,  j     *dv + v1);
        glEvalCoord2d(i*du + u1, (j + 1)*dv + v1);
      }

      glEnd();
    }
  }
  else if (mode == GL_LINE) {
    for (int j = j1; j <= j2; ++j) {
      glBegin(GL_LINE_STRIP);

      for (int i = i1; i <= i2; ++i)
        glEvalCoord2d(i*du + u1, j*dv + v1);

      glEnd();
    }

    for (int i = i1; i <= i2; ++i) {
      glBegin(GL_LINE_STRIP);

      for (int j = j1; j <= j2; ++j)
        glEvalCoord2d(i*du + u1, j*dv + v1);

      glEnd();
    }
  }
  else {
    glBegin(GL_POINTS);

    for (int j = j1; j <= j2; ++j)
      for (int i = i1; i <= i2; ++i)
        glEvalCoord2d(i*du + u1, j*dv + v1);

    glEnd();
  }
}

void
glEvalPoint1(GLint i)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  const CGLMapGrid1 &map = gl->getMapGrid1();

  double u1 = map.getU1(); double u2 = map.getU2(); int un = map.getUN();

  double du = (u2 - u1)/un;

  glEvalCoord1d(i*du + u1);
}

void
glEvalPoint2(GLint i, GLint j)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  const CGLMapGrid2 &map = gl->getMapGrid2();

  double u1 = map.getU1(); double u2 = map.getU2(); int un = map.getUN();
  double v1 = map.getV1(); double v2 = map.getV2(); int vn = map.getVN();

  double du = (u2 - u1)/un;
  double dv = (v2 - v1)/vn;

  glEvalCoord2d(i*du + u1, j*dv + v1);
}

void
glFeedbackBuffer(GLsizei size, GLenum type, GLfloat *buffer)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CGLFeedbackBuffer feedback(size, type, buffer);

  gl->setFeedbackBuffer(feedback);
}

void
glFinish()
{
  CGLMgrInst->getCurrentWindow()->flushEvents();
}

void
glFlush()
{
  CGLMgrInst->getCurrentWindow()->flushEvents();
}

void
glFogf(GLenum pname, GLfloat param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  switch (pname) {
    case GL_FOG_MODE:
      switch (int(param)) {
        case GL_EXP:
          gl->setFogType(CFOG_TYPE_EXP);
          break;
        case GL_EXP2:
          gl->setFogType(CFOG_TYPE_EXP2);
          break;
        case GL_LINEAR:
          gl->setFogType(CFOG_TYPE_LINEAR);
          break;
        default:
          gli_SetError(GL_INVALID_ENUM);
          break;
      }
      break;
    case GL_FOG_DENSITY:
      gl->setFogDensity(param);
      break;
    case GL_FOG_START:
      gl->setFogStart(param);
      break;
    case GL_FOG_END:
      gl->setFogEnd(param);
      break;
    case GL_FOG_INDEX:
      CGLMgrInst->unimplemented(
        CStrUtil::strprintf("glFogf %d %f", pname, param));
      break;
    default:
      break;
  }
}

void
glFogi(GLenum pname, GLint param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  switch (pname) {
    case GL_FOG_MODE:
      switch (param) {
        case GL_EXP:
          gl->setFogType(CFOG_TYPE_EXP);
          break;
        case GL_EXP2:
          gl->setFogType(CFOG_TYPE_EXP2);
          break;
        case GL_LINEAR:
          gl->setFogType(CFOG_TYPE_LINEAR);
          break;
        default:
          gli_SetError(GL_INVALID_ENUM);
          break;
      }
      break;
    case GL_FOG_DENSITY:
      gl->setFogDensity(param);
      break;
    case GL_FOG_START:
      gl->setFogStart(param);
      break;
    case GL_FOG_END:
      gl->setFogEnd(param);
      break;
    case GL_FOG_INDEX:
      CGLMgrInst->unimplemented(
        CStrUtil::strprintf("glFogf %d %d", pname, param));
      break;
    default:
      break;
  }
}

void
glFogfv(GLenum pname, const GLfloat *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  switch (pname) {
    case GL_FOG_MODE:
      switch (int(params[0])) {
        case GL_EXP:
          gl->setFogType(CFOG_TYPE_EXP);
          break;
        case GL_EXP2:
          gl->setFogType(CFOG_TYPE_EXP2);
          break;
        case GL_LINEAR:
          gl->setFogType(CFOG_TYPE_LINEAR);
          break;
        default:
          gli_SetError(GL_INVALID_ENUM);
          break;
      }
      break;
    case GL_FOG_DENSITY:
      gl->setFogDensity(params[0]);
      break;
    case GL_FOG_START:
      gl->setFogStart(params[0]);
      break;
    case GL_FOG_END:
      gl->setFogEnd(params[0]);
      break;
    case GL_FOG_INDEX:
      CGLMgrInst->unimplemented(
        CStrUtil::strprintf("glFogf %d %f", pname, params[0]));
      break;
    case GL_FOG_COLOR:
      gl->setFogColor(CRGBA(params[0], params[1], params[2], params[3]));
      break;
    default:
      break;
  }
}

void
glFogiv(GLenum pname, const GLint *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  switch (pname) {
    case GL_FOG_MODE:
      switch (params[0]) {
        case GL_EXP:
          gl->setFogType(CFOG_TYPE_EXP);
          break;
        case GL_EXP2:
          gl->setFogType(CFOG_TYPE_EXP2);
          break;
        case GL_LINEAR:
          gl->setFogType(CFOG_TYPE_LINEAR);
          break;
        default:
          gli_SetError(GL_INVALID_ENUM);
          break;
      }
      break;
    case GL_FOG_DENSITY:
      gl->setFogDensity(params[0]);
      break;
    case GL_FOG_START:
      gl->setFogStart(params[0]);
      break;
    case GL_FOG_END:
      gl->setFogEnd(params[0]);
      break;
    case GL_FOG_INDEX:
      CGLMgrInst->unimplemented(
        CStrUtil::strprintf("glFogf %d %d", pname, params[0]));
      break;
    case GL_FOG_COLOR:
      gl->setFogColor(CRGBA(params[0], params[1], params[2], params[3]));
    default:
      break;
  }
}

void
glFrontFace(GLenum mode)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (mode == GL_CW)
    gl->setFrontFaceOrient(CPOLYGON_ORIENTATION_CLOCKWISE);
  else if (mode == GL_CCW)
    gl->setFrontFaceOrient(CPOLYGON_ORIENTATION_ANTICLOCKWISE);
  else
    CGLMgrInst->unimplemented("glFrontFace");
}

void
glFrustum(GLdouble left, GLdouble right, GLdouble bottom,
          GLdouble top, GLdouble near, GLdouble far)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CMatrix3DH matrix;

  matrix.buildFrustrum(left, right, bottom, top, near, far);

  gl->multMatrix(&matrix);
}

GLuint
glGenLists(GLsizei range)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  GLuint id;

  for (int i = 0; i < range; ++i)
    id = gl->createDisplayList();

  return id - range + 1;
}

void
glGenTextures(GLsizei n, GLuint *textures)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  for (int i = 0; i < n; ++i) {
    textures[i] = gl->getTextureNextInd();

    gl->setTextureNextInd(textures[i] + 1);
  }
}

void
glGetBooleanv(GLenum /*pname*/, GLboolean * /*params*/)
{
  CGLMgrInst->unimplemented("glGetBooleanv");
}

void
glGetDoublev(GLenum pname, GLdouble *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (pname == GL_MODELVIEW_MATRIX) {
    CMatrix3DH *matrix = gl->getMatrix(GL_MODELVIEW);

    matrix->getValues(&params[ 0], &params[ 1], &params[ 2], &params[ 3],
                      &params[ 4], &params[ 5], &params[ 6], &params[ 7],
                      &params[ 8], &params[ 9], &params[10], &params[11],
                      &params[12], &params[13], &params[14], &params[15]);
  }
  else if (pname == GL_PROJECTION_MATRIX) {
    CMatrix3DH *matrix = gl->getMatrix(GL_PROJECTION);

    matrix->getValues(&params[ 0], &params[ 1], &params[ 2], &params[ 3],
                      &params[ 4], &params[ 5], &params[ 6], &params[ 7],
                      &params[ 8], &params[ 9], &params[10], &params[11],
                      &params[12], &params[13], &params[14], &params[15]);
  }
  else if (pname == GL_ACCUM_CLEAR_VALUE) {
    const CRGBA &rgba = gl->getAccumBuffer().getClearColor();

    params[0] = rgba.getRed  ();
    params[1] = rgba.getGreen();
    params[2] = rgba.getBlue ();
    params[3] = rgba.getAlpha();
  }
  else if (pname == GL_COLOR_CLEAR_VALUE) {
    const CRGBA &rgba = gl->getColorBuffer().getClearColor();

    params[0] = rgba.getRed  ();
    params[1] = rgba.getGreen();
    params[2] = rgba.getBlue ();
    params[3] = rgba.getAlpha();
  }
  else if (pname == GL_DEPTH_CLEAR_VALUE) {
    params[0] = gl->getColorBuffer().getClearDepth();
  }
  else
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glGetDoublev %d", pname));
}

GLenum
glGetError()
{
  return CGLMgrInst->getErrorNum();
}

void
glGetFloatv(GLenum pname, GLfloat *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (pname == GL_LINE_WIDTH_GRANULARITY) {
    params[0] = 0.1;
  }
  else if (pname == GL_LINE_WIDTH_RANGE) {
    params[0] =  0.5;
    params[1] = 10.0;
  }
  else if (pname == GL_CURRENT_RASTER_POSITION) {
    const CPoint3D &point = gl->getRasterPos().getPixel();

    params[0] = point.x;
    params[1] = point.y;
    params[2] = point.z;
    params[3] = 1.0;
  }
  else if (pname == GL_MODELVIEW_MATRIX) {
    CMatrix3DH *matrix = gl->getMatrix(GL_MODELVIEW);

    double v[16];

    matrix->getValues(v, 16);

    for (int i = 0; i < 16; ++i)
      params[i] = v[i];
  }
  else {
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glGetFloatv %d", pname));
  }
}

void
glGetHistogram(GLenum /*target*/, GLboolean /*reset*/, GLenum /*format*/, GLenum type,
               GLvoid *values)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  const CGLHistogram &histogram = gl->getHistogram();

  uint hw = histogram.getWidth();

  if (type == GL_UNSIGNED_SHORT) {
    double m = 0.0;

    for (uint i = 0; i < hw; ++i) {
      const CRGBA &rgba = histogram.getValue(i);

      m = std::max(std::max(std::max(m, rgba.getRed()), rgba.getGreen()), rgba.getBlue());
    }

    double f = USHRT_MAX/m;

    ushort *p = (ushort *) values;

    for (uint i = 0; i < hw; ++i, p += 3) {
      p[0] = ushort(histogram.getValue(i).getRed  ()*f);
      p[1] = ushort(histogram.getValue(i).getGreen()*f);
      p[2] = ushort(histogram.getValue(i).getBlue ()*f);
    }
  }
  else
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glGetHistogram"));
}

void
glGetIntegerv(GLenum pname, GLint *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (pname == GL_VIEWPORT) {
    double x1, y1, x2, y2;

    gl->getViewport(&x1, &y1, &x2, &y2);

    params[0] = (int) x1;
    params[1] = (int) y1;
    params[2] = (int) (x2 - x1 + 1.0);
    params[3] = (int) (y2 - y1 + 1.0);
  }
  else if (pname == GL_INDEX_MODE) {
    params[0] = 0;
  }
  else {
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glGetIntegerv %d", pname));
  }
}

void
glGetMinmax(GLenum /*target*/, GLboolean /*reset*/, GLenum /*format*/, GLenum type, GLvoid *values)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  const CGLMinmax &minmax = gl->getMinmax();

  if (type == GL_UNSIGNED_BYTE) {
    const CRGBA &minv = minmax.getMin();
    const CRGBA &maxv = minmax.getMax();

    double m = std::max(std::max(std::max(m, maxv.getRed()), maxv.getGreen()), maxv.getBlue());

    double f = 255/m;

    uchar *p = (uchar *) values;

    p[0] = uchar(minv.getRed  ()*f);
    p[1] = uchar(minv.getGreen()*f);
    p[2] = uchar(minv.getBlue ()*f);
    p[3] = uchar(maxv.getRed  ()*f);
    p[4] = uchar(maxv.getGreen()*f);
    p[5] = uchar(maxv.getBlue ()*f);
  }
  else
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glGetMinmax"));
}

const GLubyte *
glGetString(GLenum name)
{
  switch (name) {
    case GL_EXTENSIONS:
      return (const GLubyte *) "GL_ARB_texture_cube_map";
    case GL_RENDERER:
      return (const GLubyte *) "CGL";
    case GL_VERSION:
      return (const GLubyte *) "0.1";
    default:
      CGLMgrInst->unimplemented(CStrUtil::strprintf("glGetString %d", name));
      return (const GLubyte *) "";
  }
}

void
glGetTexLevelParameteriv(GLenum /*target*/, GLint /*level*/, GLenum /*pname*/, GLint * /*params*/)
{
  CGLMgrInst->unimplemented("glGetTexLevelParameteriv");
}

void
glHint(GLenum target, GLenum mode)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  switch (target) {
    case GL_FOG_HINT:
      gl->setFogHint(mode);
      break;
    case GL_LINE_SMOOTH_HINT:
      gl->setLineSmoothHint(mode);
      break;
    default:
      CGLMgrInst->unimplemented(
        CStrUtil::strprintf("glHint %d %d", target, mode));
      break;
  }
}

void
glHistogram(GLenum /*target*/, GLsizei width, GLenum internalFormat,
            GLboolean sink)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CGLHistogram histogram = gl->getHistogram();

  histogram.setType (internalFormat);
  histogram.setWidth(width);
  histogram.setSink (sink);

  gl->setHistogram(histogram);
}

void
glIndexf(GLfloat /*c*/)
{
  CGLMgrInst->unimplemented("glIndexf");
}

void
glIndexi(GLint /*c*/)
{
  CGLMgrInst->unimplemented("glIndexi");
}

void
glInitNames()
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->initNames();
}

void
glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid *pointer)
{
  GLsizei str = stride;

  if (str == 0)
    str = gli_InterleavedArraysGetStride(format);

  GLubyte *p = (GLubyte *) pointer;

  glDisableClientState(GL_EDGE_FLAG_ARRAY);
  glDisableClientState(GL_INDEX_ARRAY);

  if (gli_InterleavedArraysIsTexture(format)) {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glTexCoordPointer(gli_InterleavedArraysGetTextureSize(format),
                      GL_FLOAT, str, p);
  }
  else
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  if (gli_InterleavedArraysIsColor(format)) {
    glEnableClientState(GL_COLOR_ARRAY);

    glColorPointer(gli_InterleavedArraysGetColorSize(format),
                   gli_InterleavedArraysGetColorType(format),
                   str, p + gli_InterleavedArraysGetColorOffset(format));
  }
  else
    glDisableClientState(GL_COLOR_ARRAY);

  if (gli_InterleavedArraysIsNormal(format)) {
    glEnableClientState(GL_NORMAL_ARRAY);

    glNormalPointer(GL_FLOAT,
                    str, p + gli_InterleavedArraysGetNormalOffset(format));
  }
  else
    glDisableClientState(GL_NORMAL_ARRAY);

  glEnableClientState(GL_VERTEX_ARRAY);

  glVertexPointer(gli_InterleavedArraysGetVertexSize(format),
                  GL_FLOAT,
                  str, p + gli_InterleavedArraysGetVertexOffset(format));
}

GLboolean
glIsEnabled(GLenum cap)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (cap == GL_ALPHA_TEST)
    return gl->getAlphaTest().getEnabled();
  else if (cap == GL_AUTO_NORMAL)
    return gl->getMap2AutoNormal();
  else if (cap == GL_BLEND)
    return gl->getBlending();
  else if (cap == GL_CLIP_PLANE0)
    return gl->clipPlaneEnabled(0);
  else if (cap == GL_CLIP_PLANE1)
    return gl->clipPlaneEnabled(1);
  else if (cap == GL_CLIP_PLANE2)
    return gl->clipPlaneEnabled(2);
  else if (cap == GL_CLIP_PLANE3)
    return gl->clipPlaneEnabled(3);
  else if (cap == GL_CLIP_PLANE4)
    return gl->clipPlaneEnabled(4);
  else if (cap == GL_CLIP_PLANE5)
    return gl->clipPlaneEnabled(5);
  else if (cap == GL_COLOR_MATERIAL)
    return gl->getColorMaterial().getEnabled();
  else if (cap == GL_COLOR_TABLE)
    return gl->getColorTableEnabled();
  else if (cap == GL_CONVOLUTION_2D)
    return gl->getConvolution2DEnabled();
  else if (cap == GL_CULL_FACE)
    return gl->getCullFace().getEnabled();
  else if (cap == GL_DEPTH_TEST)
    return gl->getDepthTest().getEnabled();
  else if (cap == GL_DITHER)
    return gl->getDither();
  else if (cap == GL_FOG)
    return gl->getFogEnabled();
  else if (cap == GL_HISTOGRAM)
    return gl->getHistogramEnabled();
  else if (cap == GL_LIGHTING)
    return gl->getLighting();
  else if (cap == GL_LIGHT0)
    return gl->getLightEnabled(0);
  else if (cap == GL_LIGHT1)
    return gl->getLightEnabled(1);
  else if (cap == GL_LIGHT2)
    return gl->getLightEnabled(2);
  else if (cap == GL_LIGHT3)
    return gl->getLightEnabled(3);
  else if (cap == GL_LIGHT4)
    return gl->getLightEnabled(4);
  else if (cap == GL_LIGHT5)
    return gl->getLightEnabled(5);
  else if (cap == GL_LIGHT6)
    return gl->getLightEnabled(6);
  else if (cap == GL_LIGHT7)
    return gl->getLightEnabled(7);
  else if (cap == GL_LINE_SMOOTH)
    return gl->getLineSmooth();
  else if (cap == GL_LINE_STIPPLE)
    return gl->getLineStipple();
  else if (cap == GL_MAP1_COLOR_4)
    return gl->getMap1Data().getGenColor();
  else if (cap == GL_MAP1_INDEX)
    return gl->getMap1Data().getGenIndex();
  else if (cap == GL_MAP1_NORMAL)
    return gl->getMap1Data().getGenNormal();
  else if (cap == GL_MAP1_TEXTURE_COORD_1)
    return gl->getMap1Data().getGenTexture1();
  else if (cap == GL_MAP1_TEXTURE_COORD_2)
    return gl->getMap1Data().getGenTexture2();
  else if (cap == GL_MAP1_TEXTURE_COORD_3)
    return gl->getMap1Data().getGenTexture3();
  else if (cap == GL_MAP1_TEXTURE_COORD_4)
    return gl->getMap1Data().getGenTexture4();
  else if (cap == GL_MAP1_VERTEX_3)
    return gl->getMap1Data().getGenVertex3();
  else if (cap == GL_MAP1_VERTEX_4)
    return gl->getMap1Data().getGenVertex4();
  else if (cap == GL_MAP2_COLOR_4)
    return gl->getMap2Data().getGenColor();
  else if (cap == GL_MAP2_INDEX)
    return gl->getMap2Data().getGenIndex();
  else if (cap == GL_MAP2_NORMAL)
    return gl->getMap2Data().getGenNormal();
  else if (cap == GL_MAP2_TEXTURE_COORD_1)
    return gl->getMap2Data().getGenTexture1();
  else if (cap == GL_MAP2_TEXTURE_COORD_2)
    return gl->getMap2Data().getGenTexture2();
  else if (cap == GL_MAP2_TEXTURE_COORD_3)
    return gl->getMap2Data().getGenTexture3();
  else if (cap == GL_MAP2_TEXTURE_COORD_4)
    return gl->getMap2Data().getGenTexture4();
  else if (cap == GL_MAP2_VERTEX_3)
    return gl->getMap2Data().getGenVertex3();
  else if (cap == GL_MAP2_VERTEX_4)
    return gl->getMap2Data().getGenVertex4();
  else if (cap == GL_MINMAX)
    return gl->getMinmaxEnabled();
  else if (cap == GL_NORMALIZE)
    return gl->getAutoNormalize();
  else if (cap == GL_POLYGON_OFFSET_FILL)
    return gl->getPolyOffsetFill();
  else if (cap == GL_POLYGON_SMOOTH)
    return gl->getPolySmooth();
  else if (cap == GL_POLYGON_STIPPLE)
    return gl->getPolyStipple();
  else if (cap == GL_SCISSOR_TEST)
    return gl->getScissor().getEnabled();
  else if (cap == GL_STENCIL_TEST)
    return gl->getStencil().getEnabled();
  else if (cap == GL_TEXTURE_1D)
    return gl->getTexture1Data().getEnabled();
  else if (cap == GL_TEXTURE_2D)
    return gl->getTexture2Data().getEnabled();
  else if (cap == GL_TEXTURE_3D)
    return gl->getTexture3Data().getEnabled();
  else if (cap == GL_TEXTURE_GEN_Q)
    return gl->getTextureGen().getQEnabled();
  else if (cap == GL_TEXTURE_GEN_R)
    return gl->getTextureGen().getREnabled();
  else if (cap == GL_TEXTURE_GEN_S)
    return gl->getTextureGen().getSEnabled();
  else if (cap == GL_TEXTURE_GEN_T)
    return gl->getTextureGen().getTEnabled();
  else {
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glIsEnabled %d", cap));
    return false;
  }
}

GLboolean
glIsList(GLuint list)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  return gl->isDisplayList(list);
}

GLboolean
glIsTexture(GLuint /*texture*/)
{
  CGLMgrInst->unimplemented("glIsTexture");

  return false;
}

void
glLightf(GLenum light, GLenum pname, const GLfloat param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  light -= GL_LIGHT0;

  if      (pname == GL_SPOT_EXPONENT)
    gl->setLightSpotExponent(light, param);
  else if (pname == GL_SPOT_CUTOFF)
    gl->setLightSpotCutOff(light, param);
  else if (pname == GL_CONSTANT_ATTENUATION)
    gl->setLightConstantAttenuation(light, param);
  else if (pname == GL_LINEAR_ATTENUATION)
    gl->setLightLinearAttenuation(light, param);
  else if (pname == GL_QUADRATIC_ATTENUATION)
    gl->setLightQuadraticAttenuation(light, param);
}

void
glLighti(GLenum light, GLenum pname, const GLint param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  light -= GL_LIGHT0;

  if      (pname == GL_SPOT_EXPONENT)
    gl->setLightSpotExponent(light, param);
  else if (pname == GL_SPOT_CUTOFF)
    gl->setLightSpotCutOff(light, param);
  else if (pname == GL_CONSTANT_ATTENUATION)
    gl->setLightConstantAttenuation(light, param);
  else if (pname == GL_LINEAR_ATTENUATION)
    gl->setLightLinearAttenuation(light, param);
  else if (pname == GL_QUADRATIC_ATTENUATION)
    gl->setLightQuadraticAttenuation(light, param);
}

void
glLightfv(GLenum light, GLenum pname, const GLfloat *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (! gl) return;

  light -= GL_LIGHT0;

  if      (pname == GL_AMBIENT)
    gl->setLightAmbient(light,
      CRGBA(params[0], params[1], params[2], params[3]));
  else if (pname == GL_DIFFUSE)
    gl->setLightDiffuse(light,
      CRGBA(params[0], params[1], params[2], params[3]));
  else if (pname == GL_SPECULAR)
    gl->setLightSpecular(light,
      CRGBA(params[0], params[1], params[2], params[3]));
  else if (pname == GL_POSITION) {
    CPoint3D point(params[0], params[1], params[2]);

    CMatrix3DH *matrix = gl->getMatrix(GL_MODELVIEW);

    CPoint3D position;

    matrix->multiplyPoint(point, position);

    gl->setLightPosition(light, position);

    if (params[3] == 0.0)
      gl->setLightDirectional(light, true);
    else
      gl->setLightDirectional(light, false);
  }
  else if (pname == GL_SPOT_CUTOFF)
    gl->setLightSpotCutOff(light, params[0]);
  else if (pname == GL_SPOT_DIRECTION) {
    CVector3D vector(params[0], params[1], params[2]);

    CMatrix3DH *imatrix = gl->getIMatrix(GL_MODELVIEW);

    CVector3D direction;

    imatrix->preMultiplyVector(vector, direction);

    gl->setLightSpotDirection(light, direction);
  }
  else if (pname == GL_SPOT_EXPONENT)
    gl->setLightSpotExponent(light, params[0]);
  else if (pname == GL_CONSTANT_ATTENUATION)
    gl->setLightConstantAttenuation(light, params[0]);
  else if (pname == GL_LINEAR_ATTENUATION)
    gl->setLightLinearAttenuation(light, params[0]);
  else if (pname == GL_QUADRATIC_ATTENUATION)
    gl->setLightQuadraticAttenuation(light, params[0]);
}

void
glLightiv(GLenum light, GLenum pname, const GLint *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  light -= GL_LIGHT0;

  if      (pname == GL_AMBIENT)
    gl->setLightAmbient(light,
      CRGBA(params[0], params[1], params[2], params[3]));
  else if (pname == GL_DIFFUSE)
    gl->setLightDiffuse(light,
      CRGBA(params[0], params[1], params[2], params[3]));
  else if (pname == GL_SPECULAR)
    gl->setLightSpecular(light,
      CRGBA(params[0], params[1], params[2], params[3]));
  else if (pname == GL_POSITION) {
    CPoint3D point(params[0], params[1], params[2]);

    CMatrix3DH *matrix = gl->getMatrix(GL_MODELVIEW);

    CPoint3D position;

    matrix->multiplyPoint(point, position);

    gl->setLightPosition(light, position);

    if (params[3] == 0)
      gl->setLightDirectional(light, true);
    else
      gl->setLightDirectional(light, false);
  }
  else if (pname == GL_SPOT_CUTOFF)
    gl->setLightSpotCutOff(light, params[0]);
  else if (pname == GL_SPOT_DIRECTION) {
    CVector3D vector(params[0], params[1], params[2]);

    CMatrix3DH *imatrix = gl->getIMatrix(GL_MODELVIEW);

    CVector3D direction;

    imatrix->preMultiplyVector(vector, direction);

    gl->setLightSpotDirection(light, direction);
  }
  else if (pname == GL_SPOT_EXPONENT)
    gl->setLightSpotExponent(light, params[0]);
  else if (pname == GL_CONSTANT_ATTENUATION)
    gl->setLightConstantAttenuation(light, params[0]);
  else if (pname == GL_LINEAR_ATTENUATION)
    gl->setLightLinearAttenuation(light, params[0]);
  else if (pname == GL_QUADRATIC_ATTENUATION)
    gl->setLightQuadraticAttenuation(light, params[0]);
  else
    gli_SetError(GL_INVALID_ENUM);
}

void
glLightModelf(GLenum pname, GLfloat param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (pname == GL_LIGHT_MODEL_LOCAL_VIEWER)
    gl->setLightModelLocalViewer(param);
  else if (pname == GL_LIGHT_MODEL_TWO_SIDE)
    gl->setLightModelTwoSide(param);
  else
    gli_SetError(GL_INVALID_ENUM);
}

void
glLightModelfv(GLenum pname, const GLfloat *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (pname == GL_LIGHT_MODEL_AMBIENT)
    gl->setLightModelAmbient(
      CRGBA(params[0], params[1], params[2], params[3]));
  else if (pname == GL_LIGHT_MODEL_LOCAL_VIEWER)
    gl->setLightModelLocalViewer(params[0]);
  else if (pname == GL_LIGHT_MODEL_TWO_SIDE)
    gl->setLightModelTwoSide(params[0]);
  else
    gli_SetError(GL_INVALID_ENUM);
}

void
glLightModeli(GLenum pname, GLint param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (pname == GL_LIGHT_MODEL_LOCAL_VIEWER)
    gl->setLightModelLocalViewer(param);
  else if (pname == GL_LIGHT_MODEL_TWO_SIDE)
    gl->setLightModelTwoSide(param);
  else if (pname == GL_LIGHT_MODEL_COLOR_CONTROL)
    gl->setLightModelColorControl(param);
  else
    gli_SetError(GL_INVALID_ENUM);
}

void
glLightModeliv(GLenum pname, const GLint *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (pname == GL_LIGHT_MODEL_AMBIENT)
    gl->setLightModelAmbient(
      CRGBA(CMathGen::mapToReal(params[0]),
            CMathGen::mapToReal(params[1]),
            CMathGen::mapToReal(params[2]),
            CMathGen::mapToReal(params[3])));
  else if (pname == GL_LIGHT_MODEL_LOCAL_VIEWER)
    gl->setLightModelLocalViewer(params[0]);
  else if (pname == GL_LIGHT_MODEL_TWO_SIDE)
    gl->setLightModelTwoSide(params[0]);
  else if (pname == GL_LIGHT_MODEL_COLOR_CONTROL)
    gl->setLightModelColorControl(params[0]);
  else
    gli_SetError(GL_INVALID_ENUM);
}

void
glLineStipple(GLint factor, GLushort pattern)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CLineDash line_dash(pattern);

  line_dash.scale(factor);

  gl->setLineDash(line_dash);
}

void
glLineWidth(GLfloat width)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setLineWidth(width);
}

void
glListBase(GLuint base)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setListBase(base);
}

void
glLoadIdentity()
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  gl->initMatrix();
}

void
glLoadMatrixf(const GLfloat *m)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  GLdouble d[16];

  for (int i = 0; i < 16; ++i)
    d[i] = m[i];

  CMatrix3DH matrix(d, 16);

  gl->setMatrix(&matrix);
}

void
glLoadMatrixd(const GLdouble *m)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CMatrix3DH matrix(m, 16);

  gl->setMatrix(&matrix);
}

void
glLoadName(GLuint name)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->numNames() > 0)
    gl->loadName(name);
  else
    gli_SetError(GL_INVALID_OPERATION);
}

void
glLogicOp(GLenum /*opcode*/)
{
  CGLMgrInst->unimplemented("glLogicOp");
}

void
glMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
        const GLdouble *points)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  std::vector<double> values;

  if      (target == GL_MAP1_VERTEX_4        ||
           target == GL_MAP1_COLOR_4         ||
           target == GL_MAP1_TEXTURE_COORD_4) {
    values.resize(4*uorder);

    for (int i = 0, j = 0; i < 4*uorder; j += ustride) {
      values[i++] = points[j + 0];
      values[i++] = points[j + 1];
      values[i++] = points[j + 2];
      values[i++] = points[j + 3];
    }
  }
  else if (target == GL_MAP1_VERTEX_3        ||
           target == GL_MAP1_NORMAL          ||
           target == GL_MAP1_TEXTURE_COORD_3) {
    values.resize(3*uorder);

    for (int i = 0, j = 0; i < 3*uorder; j += ustride) {
      values[i++] = points[j + 0];
      values[i++] = points[j + 1];
      values[i++] = points[j + 2];
    }
  }
  else if (target == GL_MAP1_TEXTURE_COORD_2) {
    values.resize(2*uorder);

    for (int i = 0, j = 0; i < 2*uorder; j += ustride) {
      values[i++] = points[j + 0];
      values[i++] = points[j + 1];
    }
  }
  else if (target == GL_MAP1_INDEX ||
           target == GL_MAP1_TEXTURE_COORD_1) {
    values.resize(1*uorder);

    for (int i = 0, j = 0; i < 1*uorder; j += ustride)
      values[i++] = points[j + 0];
  }

  CGLMap1 map = gl->getMap1(target);

  map.setU1        (u1);
  map.setU2        (u2);
  map.setNumUValues(uorder);
  map.setValues    (values);

  gl->setMap1(target, map);
}

void
glMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
        const GLfloat *points)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  std::vector<double> values;

  if      (target == GL_MAP1_VERTEX_4        ||
           target == GL_MAP1_COLOR_4         ||
           target == GL_MAP1_TEXTURE_COORD_4) {
    values.resize(4*uorder);

    for (int u = 0, i = 0, j = 0; u < uorder; ++u, j += ustride) {
      values[i++] = points[j + 0];
      values[i++] = points[j + 1];
      values[i++] = points[j + 2];
      values[i++] = points[j + 3];
    }
  }
  else if (target == GL_MAP1_VERTEX_3        ||
           target == GL_MAP1_NORMAL          ||
           target == GL_MAP1_TEXTURE_COORD_3) {
    values.resize(3*uorder);

    for (int u = 0, i = 0, j = 0; u < uorder; ++u, j += ustride) {
      values[i++] = points[j + 0];
      values[i++] = points[j + 1];
      values[i++] = points[j + 2];
    }
  }
  else if (target == GL_MAP1_TEXTURE_COORD_2) {
    values.resize(2*uorder);

    for (int u = 0, i = 0, j = 0; u < uorder; ++u, j += ustride) {
      values[i++] = points[j + 0];
      values[i++] = points[j + 1];
    }
  }
  else if (target == GL_MAP1_INDEX ||
           target == GL_MAP1_TEXTURE_COORD_1) {
    values.resize(1*uorder);

    for (int u = 0, i = 0, j = 0; u < uorder; ++u, j += ustride)
      values[i++] = points[j + 0];
  }

  CGLMap1 map = gl->getMap1(target);

  map.setU1        (u1);
  map.setU2        (u2);
  map.setNumUValues(uorder);
  map.setValues    (values);

  gl->setMap1(target, map);
}

void
glMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
        GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
        const GLdouble *points)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  std::vector<double> values;

  if      (target == GL_MAP2_VERTEX_4        ||
           target == GL_MAP2_COLOR_4         ||
           target == GL_MAP2_TEXTURE_COORD_4) {
    values.resize(4*uorder*vorder);

    for (int v = 0, i = 0, j = 0; v < vorder; ++v) {
      int j1 = j;

      for (int u = 0; u < uorder; ++u, j += ustride) {
        values[i++] = points[j + 0];
        values[i++] = points[j + 1];
        values[i++] = points[j + 2];
        values[i++] = points[j + 3];
      }

      j = j1 + vstride;
    }
  }
  else if (target == GL_MAP2_VERTEX_3        ||
           target == GL_MAP2_NORMAL          ||
           target == GL_MAP2_TEXTURE_COORD_3) {
    values.resize(3*uorder*vorder);

    for (int v = 0, i = 0, j = 0; v < vorder; ++v) {
      int j1 = j;

      for (int u = 0; u < uorder; ++u, j += ustride) {
        values[i++] = points[j + 0];
        values[i++] = points[j + 1];
        values[i++] = points[j + 2];
      }

      j = j1 + vstride;
    }
  }
  else if (target == GL_MAP2_TEXTURE_COORD_2) {
    values.resize(2*uorder*vorder);

    for (int v = 0, i = 0, j = 0; v < vorder; ++v) {
      int j1 = j;

      for (int u = 0; u < uorder; ++u, j += ustride) {
        values[i++] = points[j + 0];
        values[i++] = points[j + 1];
      }

      j = j1 + vstride;
    }
  }
  else if (target == GL_MAP2_INDEX ||
           target == GL_MAP2_TEXTURE_COORD_1) {
    values.resize(1*uorder*vorder);

    for (int v = 0, i = 0, j = 0; v < vorder; ++v) {
      int j1 = j;

      for (int u = 0; u < uorder; ++u, j += ustride) {
        values[i++] = points[j + 0];
      }

      j = j1 + vstride;
    }
  }

  CGLMap2 map = gl->getMap2(target);

  map.setU1        (u1);
  map.setU2        (u2);
  map.setV1        (v1);
  map.setV2        (v2);
  map.setNumUValues(uorder);
  map.setNumVValues(vorder);
  map.setValues    (values);

  gl->setMap2(target, map);
}

void
glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
        GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
        const GLfloat *points)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  std::vector<double> values;

  if      (target == GL_MAP2_VERTEX_4        ||
           target == GL_MAP2_COLOR_4         ||
           target == GL_MAP2_TEXTURE_COORD_4) {
    values.resize(4*uorder*vorder);

    for (int v = 0, i = 0, j = 0; v < vorder; ++v) {
      int j1 = j;

      for (int u = 0; u < uorder; ++u, j += ustride) {
        values[i++] = points[j + 0];
        values[i++] = points[j + 1];
        values[i++] = points[j + 2];
        values[i++] = points[j + 3];
      }

      j = j1 + vstride;
    }
  }
  else if (target == GL_MAP2_VERTEX_3        ||
           target == GL_MAP2_NORMAL          ||
           target == GL_MAP2_TEXTURE_COORD_3) {
    values.resize(3*uorder*vorder);

    for (int v = 0, i = 0, j = 0; v < vorder; ++v) {
      int j1 = j;

      for (int u = 0; u < uorder; ++u, j += ustride) {
        values[i++] = points[j + 0];
        values[i++] = points[j + 1];
        values[i++] = points[j + 2];
      }

      j = j1 + vstride;
    }
  }
  else if (target == GL_MAP2_TEXTURE_COORD_2) {
    values.resize(2*uorder*vorder);

    for (int v = 0, i = 0, j = 0; v < vorder; ++v) {
      int j1 = j;

      for (int u = 0; u < uorder; ++u, j += ustride) {
        values[i++] = points[j + 0];
        values[i++] = points[j + 1];
      }

      j = j1 + vstride;
    }
  }
  else if (target == GL_MAP2_INDEX ||
           target == GL_MAP2_TEXTURE_COORD_1) {
    values.resize(1*uorder*vorder);

    for (int v = 0, i = 0, j = 0; v < vorder; ++v) {
      int j1 = j;

      for (int u = 0; u < uorder; ++u, j += ustride) {
        values[i++] = points[j + 0];
      }

      j = j1 + vstride;
    }
  }

  CGLMap2 map = gl->getMap2(target);

  map.setU1        (u1);
  map.setU2        (u2);
  map.setV1        (v1);
  map.setV2        (v2);
  map.setNumUValues(uorder);
  map.setNumVValues(vorder);
  map.setValues    (values);

  gl->setMap2(target, map);
}

void
glMapGrid1d(GLint un, GLdouble u1, GLdouble u2)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CGLMapGrid1 map = gl->getMapGrid1();

  map.setUN(un); map.setU1(u1); map.setU2(u2);

  gl->setMapGrid1(map);
}

void
glMapGrid1f(GLint un, GLfloat u1, GLfloat u2)
{
  glMapGrid1d(un, u1, u2);
}

void
glMapGrid2d(GLint un, GLdouble u1, GLdouble u2,
            GLint vn, GLdouble v1, GLdouble v2)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CGLMapGrid2 map = gl->getMapGrid2();

  map.setUN(un); map.setU1(u1); map.setU2(u2);
  map.setVN(vn); map.setV1(v1); map.setV2(v2);

  gl->setMapGrid2(map);
}

void
glMapGrid2f(GLint un, GLfloat u1, GLfloat u2,
            GLint vn, GLfloat v1, GLfloat v2)
{
  glMapGrid2d(un, u1, u2, vn, v1, v2);
}

void
glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setMaterial(face, pname, param);
}

void
glMateriali(GLenum face, GLenum pname, GLint param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setMaterial(face, pname, param);
}

void
glMaterialfv(GLenum face, GLenum pname, const GLfloat *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  uint num_params = gli_ParamDim(pname);

  double *params1 = new double [num_params];

  for (uint i = 0; i < num_params; ++i)
    params1[i] = params[i];

  gl->setMaterialV(face, pname, params1, num_params);

  delete [] params1;
}

void
glMaterialiv(GLenum face, GLenum pname, const GLint *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  uint num_params = gli_ParamDim(pname);

  double *params1 = new double [num_params];

  for (uint i = 0; i < num_params; ++i)
    params1[i] = params[i];

  gl->setMaterialV(face, pname, params1, num_params);

  delete [] params1;
}

void
glMatrixMode(GLenum mode)
{
  static int all_modes =
    GL_MODELVIEW | GL_PROJECTION | GL_TEXTURE | GL_COLOR;

  CGL *gl = CGLMgrInst->getCurrentGL();

  if ((mode & ~all_modes) != 0) {
    gli_SetError(GL_INVALID_ENUM);
    return;
  }

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  gl->setMatrixMode(mode);
}

void
glMinmax(GLenum /*target*/, GLenum internalFormat, GLboolean sink)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CGLMinmax minmax = gl->getMinmax();

  minmax.setType(internalFormat);
  minmax.setSink(sink);

  gl->setMinmax(minmax);
}

void
glMultMatrixf(const GLfloat *m)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  GLdouble d[16];

  for (int i = 0; i < 16; ++i)
    d[i] = m[i];

  CMatrix3DH matrix(d, 16);

  gl->multMatrix(&matrix);
}

void
glMultMatrixd(const GLdouble *m)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CMatrix3DH matrix(m, 16);

  gl->multMatrix(&matrix);
}

void
glNewList(GLuint list, GLenum mode)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (mode == GL_COMPILE)
    gl->startDisplayList(list, false);
  else if (mode == GL_COMPILE_AND_EXECUTE)
    gl->startDisplayList(list, true);
  else {
    gli_SetError(GL_INVALID_ENUM);
    return;
  }
}

void
glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setNormal(CVector3D(nx, ny, nz));
}

void
glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
  glNormal3d(nx, ny, nz);
}

void
glNormal3dv(const GLdouble *v)
{
  glNormal3d(v[0], v[1], v[2]);
}

void
glNormal3fv(const GLfloat *v)
{
  glNormal3d(v[0], v[1], v[2]);
}

void
glNormalPointer(GLenum /*type*/, GLsizei /*stride*/, const GLvoid * /*pointer*/)
{
  CGLMgrInst->unimplemented("glNormalPointer");
}

void
glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top,
        GLdouble near, GLdouble far)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CMatrix3DH matrix;

  matrix.buildOrtho(left, right, bottom, top, near, far);

  gl->multMatrix(&matrix);
}

void
glPassThrough(GLfloat token)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->getRenderMode() == GL_FEEDBACK) {
    CGLFeedbackBuffer &feedback = gl->modifyFeedbackBuffer();

    float data[2];

    data[0] = GL_PASS_THROUGH_TOKEN;
    data[1] = token;

    feedback.append(data, 2);
  }
}

void
glPixelStoref(GLenum /*pname*/, GLfloat /*param*/)
{
  CGLMgrInst->unimplemented("glPixelStoref");
}

void
glPixelStorei(GLenum pname, GLint param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  switch (pname) {
    case GL_UNPACK_ALIGNMENT:
      gl->modifyPixelStore().setUnpackAlignment(param);
      break;
    case GL_UNPACK_ROW_LENGTH:
      gl->modifyPixelStore().setUnpackRowLength(param);
      break;
    case GL_PACK_ALIGNMENT:
      gl->modifyPixelStore().setPackAlignment(param);
      break;
    case GL_PACK_ROW_LENGTH:
      gl->modifyPixelStore().setPackRowLength(param);
      break;
    default:
      CGLMgrInst->unimplemented("glPixelStorei");
      break;
  }
}

void
glPixelTransferf(GLenum pname, GLfloat param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  switch (pname) {
    case GL_RED_SCALE:
      gl->modifyPixelTransfer().setRedScale(param);
      break;
    case GL_RED_BIAS:
      gl->modifyPixelTransfer().setRedBias(param);
      break;
    case GL_GREEN_SCALE:
      gl->modifyPixelTransfer().setGreenScale(param);
      break;
    case GL_GREEN_BIAS:
      gl->modifyPixelTransfer().setGreenBias(param);
      break;
    case GL_BLUE_SCALE:
      gl->modifyPixelTransfer().setBlueScale(param);
      break;
    case GL_BLUE_BIAS:
      gl->modifyPixelTransfer().setBlueBias(param);
      break;
    case GL_ALPHA_SCALE:
      gl->modifyPixelTransfer().setAlphaScale(param);
      break;
    case GL_ALPHA_BIAS:
      gl->modifyPixelTransfer().setAlphaBias(param);
      break;
    default:
      CGLMgrInst->unimplemented("glPixelTransferf");
      break;
  }
}

void
glPixelZoom(GLfloat zoomx, GLfloat zoomy)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setPixelZoomX(zoomx);
  gl->setPixelZoomY(zoomy);
}

void
glPointSize(GLfloat size)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (size <= 0) {
    gli_SetError(GL_INVALID_VALUE);
    return;
  }

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  gl->setPointSize(size);
}

void
glPolygonMode(GLenum face, GLenum mode)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (face == GL_FRONT || face == GL_FRONT_AND_BACK) {
    gl->setFrontFaceMode(mode);
  }
  if (face == GL_BACK  || face == GL_FRONT_AND_BACK) {
    gl->setBackFaceMode(mode);
  }
}

void
glPolygonOffset(GLfloat factor, GLfloat units)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setPolyOffsetFactor(factor);
  gl->setPolyOffsetUnits (units );
}

void
glPolygonStipple(const GLubyte *mask)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CImageNameSrc src("glPolygonStipple32x32");

  GLuint bit_mask;

  CImagePtr image = CImageMgrInst->createImage(src);

  image->setDataSize(32, 32);

  for (int y = 31, i = 0; y >= 0; --y) {
    for (int w = 0, x = 0; w < 4; ++w, ++i) {
      for (int b = 0; b < 8; ++b, ++x) {
        bit_mask = (1 << (7 - b));

        if (bit_mask & mask[i])
          image->setGrayPixel(x, y, 1.0);
        else
          image->setGrayPixel(x, y, 0.0);
      }
    }
  }
#if 0
  for (int i = 0, ind = 0; i < 128; ++i) {
    for (int j = 0; j < 8; ++j, ++ind) {
      bit_mask = (1 << (7 - j));

      if (bit_mask & mask[i])
        image->setGrayPixel(ind, 1.0);
      else
        image->setGrayPixel(ind, 0.0);
    }
  }
#endif

  gl->setPolyStippleImage(image);
}

void
glPopAttrib()
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  uint type;

  if (! gl->peekAttribType(&type))
    gli_SetError(GL_STACK_UNDERFLOW);

  if      (type == GL_LIST_BIT)
    gl->setListBase(gl->popAttrib<int>());
  else if (type == GL_ENABLE_BIT) {
    CGLEnableBits enable_bits = gl->popAttrib<CGLEnableBits>();

    enable_bits.restore(gl);
  }
  else if (type == GL_EVAL_BIT) {
    CGLEvalBits eval_bits = gl->popAttrib<CGLEvalBits>();

    eval_bits.restore(gl);
  }
  else if (type == GL_LIGHTING_BIT) {
    CGLLightingBits lighting_bits = gl->popAttrib<CGLLightingBits>();

    lighting_bits.restore(gl);
  }
  else
    CGLMgrInst->unimplemented("glPopAttrib");
}

void
glPopMatrix()
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  if (! gl->popMatrix())
    gli_SetError(GL_STACK_UNDERFLOW);
}

void
glPopName()
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->popName();
}

void
glPushAttrib(GLbitfield mask)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (TST_FLAG(mask, GL_LIST_BIT)) {
    gl->pushAttrib<int>(GL_LIST_BIT, gl->getListBase());

    RST_FLAG(mask, GL_LIST_BIT);
  }

  if (TST_FLAG(mask, GL_ENABLE_BIT)) {
    CGLEnableBits enable_bits;

    enable_bits.save(gl);

    gl->pushAttrib<CGLEnableBits>(GL_ENABLE_BIT, enable_bits);

    RST_FLAG(mask, GL_ENABLE_BIT);
  }

  if (TST_FLAG(mask, GL_EVAL_BIT)) {
    CGLEvalBits eval_bits;

    eval_bits.save(gl);

    gl->pushAttrib<CGLEvalBits>(GL_EVAL_BIT, eval_bits);

    RST_FLAG(mask, GL_EVAL_BIT);
  }

  if (TST_FLAG(mask, GL_LIGHTING_BIT)) {
    CGLLightingBits lighting_bits;

    lighting_bits.save(gl);

    gl->pushAttrib<CGLLightingBits>(GL_LIGHTING_BIT, lighting_bits);

    RST_FLAG(mask, GL_LIGHTING_BIT);
  }

  if (mask)
    CGLMgrInst->unimplemented("glPushAttrib");
}

void
glPushMatrix()
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  gl->pushMatrix();
}

void
glPushName(GLuint name)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->pushName(name);
}

void
glRasterPos2d(GLdouble x, GLdouble y)
{
  glRasterPos3d(x, y, 0.0);
}

void
glRasterPos2dv(const GLdouble *c)
{
  glRasterPos3d(c[0], c[1], 0.0);
}

void
glRasterPos2f(GLfloat x, GLfloat y)
{
  glRasterPos3d(x, y, 0.0);
}

void
glRasterPos2fv(const GLfloat *v)
{
  glRasterPos3d(v[0], v[1], 0.0);
}

void
glRasterPos2i(GLint x, GLint y)
{
  glRasterPos3d(x, y, 0.0);
}

void
glRasterPos2iv(const GLint *c)
{
  glRasterPos3d(c[0], c[1], 0.0);
}

void
glRasterPos2s(GLshort x, GLshort y)
{
  glRasterPos3d(x, y, 0.0);
}

void
glRasterPos2sv(const GLshort *s)
{
  glRasterPos3d(s[0], s[1], 0.0);
}

void
glRasterPos3d(GLdouble x, GLdouble y, GLdouble z)
{
  glRasterPos4d(x, y, z, 1);
}

void
glRasterPos3dv(const GLdouble *v)
{
  glRasterPos3d(v[0], v[1], v[2]);
}

void
glRasterPos3f(GLfloat x, GLfloat y, GLfloat z)
{
  glRasterPos3d(x, y, z);
}

void
glRasterPos3fv(const GLfloat *v)
{
  glRasterPos3d(v[0], v[1], v[2]);
}

void
glRasterPos3i(GLint x, GLint y, GLint z)
{
  glRasterPos3d(x, y, z);
}

void
glRasterPos3iv(const GLint *v)
{
  glRasterPos3d(v[0], v[1], v[2]);
}

void
glRasterPos3s(GLshort x, GLshort y, GLshort z)
{
  glRasterPos3d(x, y, z);
}

void
glRasterPos3sv(const GLshort *s)
{
  glRasterPos3d(s[0], s[1], s[2]);
}

void
glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->updateRasterPos(CPoint3D(x/w, y/w, z/w));
}

void
glRasterPos4dv(const GLdouble *s)
{
  glRasterPos4d(s[0], s[1], s[2], s[3]);
}

void
glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  glRasterPos4d(x, y, z, w);
}

void
glRasterPos4fv(const GLfloat *s)
{
  glRasterPos4d(s[0], s[1], s[2], s[3]);
}

void
glRasterPos4i(GLint x, GLint y, GLint z, GLint w)
{
  glRasterPos4d(x, y, z, w);
}

void
glRasterPos4iv(const GLint *s)
{
  glRasterPos4d(s[0], s[1], s[2], s[3]);
}

void
glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
  glRasterPos4d(x, y, z, w);
}

void
glRasterPos4sv(const GLshort *s)
{
  glRasterPos4d(s[0], s[1], s[2], s[3]);
}

void
glReadBuffer(GLenum /*mode*/)
{
  CGLMgrInst->unimplemented("glReadBuffer");
}

void
glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height,
             GLenum format, GLenum type, GLvoid *pixels)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (format == GL_RGBA) {
    if (type == GL_UNSIGNED_BYTE)
      gl->readColorImage(x, y, width, height, (uchar *) pixels, 4, true);
    else
      CGLMgrInst->unimplemented("glReadPixels");
  }
  else
    CGLMgrInst->unimplemented("glReadPixels");
}

void
glRectdv(const GLdouble *v1, const GLdouble *v2)
{
  glRectd(v1[0], v1[1], v2[0], v2[1]);
}

void
glRectfv(const GLfloat *v1, const GLfloat *v2)
{
  glRectd(v1[0], v1[1], v2[0], v2[1]);
}

void
glRectiv(const GLint *v1, const GLint *v2)
{
  glRectd(v1[0], v1[1], v2[0], v2[1]);
}

void
glRectsv(const GLshort *v1, const GLshort *v2)
{
  glRectd(v1[0], v1[1], v2[0], v2[1]);
}

void
glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  glBegin(GL_POLYGON);

  glVertex2d(x1, y1);
  glVertex2d(x2, y1);
  glVertex2d(x2, y2);
  glVertex2d(x1, y2);

  glEnd();
}

void
glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
  glRectd(x1, y1, x2, y2);
}

void
glRecti(GLint x1, GLint y1, GLint x2, GLint y2)
{
  glRectd(x1, y1, x2, y2);
}

void
glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
  glRectd(x1, y1, x2, y2);
}

GLint
glRenderMode(GLenum mode)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  GLint size = 0;

  if      (gl->getRenderMode() == GL_FEEDBACK)
    size = gl->getFeedbackBuffer().pos;
  else if (gl->getRenderMode() == GL_SELECT) {
    gl->modifySelectBuffer().setData();

    size = gl->getSelectBuffer().num_hits;
  }

  gl->setRenderMode(mode);

  if (mode == GL_SELECT)
    gl->modifySelectBuffer().reset();

  return size;
}

void
glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->rotateMatrix(angle, x, y, z);
}

void
glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  glRotated(angle, x, y, z);
}

void
glScaled(GLdouble x, GLdouble y, GLdouble z)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->scaleMatrix(x, y, z);
}

void
glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  glScaled(x, y, z);
}

void
glScissor(GLint x, GLint y, GLint width, GLint height)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CIPoint2D point1(x, y);
  CIPoint2D point2(x + width -1, y + height - 1);

  gl->modifyScissor().setRect(CIBBox2D(point1, point2));
}

void
glSelectBuffer(GLsizei size, GLuint *buffer)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CGLSelectBuffer select(size, buffer);

  gl->setSelectBuffer(select);
}

void
glShadeModel(GLenum mode)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (gl->inBlock()) {
    gli_SetError(GL_INVALID_OPERATION);
    return;
  }

  if      (mode == GL_FLAT)
    gl->setFlatShading(true);
  else if (mode == GL_SMOOTH)
    gl->setFlatShading(false);
  else
    gli_SetError(GL_INVALID_ENUM);
}

void
glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->modifyStencil().setFunc(func);
  gl->modifyStencil().setRef (ref);
  gl->modifyStencil().setMask(mask);
}

void
glStencilMask(GLuint mask)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->getColorBuffer().setStencilMask(mask);
}

void
glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->modifyStencil().setFail (fail);
  gl->modifyStencil().setZFail(zfail);
  gl->modifyStencil().setZPass(zpass);
}

void
glTexCoord1d(GLdouble s)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setTexCoord(CPoint3D(s, 0, 0));
}

void
glTexCoord1f(GLfloat s)
{
  glTexCoord1d(s);
}

void
glTexCoord2d(GLdouble s, GLdouble t)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setTexCoord(CPoint3D(s, t, 0));
}

void
glTexCoord2i(GLint s, GLint t)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setTexCoord(CPoint3D(s, t, 0));
}

void
glTexCoord2f(GLfloat s, GLfloat t)
{
  glTexCoord2d(s, t);
}

void
glTexCoord2dv(const GLdouble *values)
{
  glTexCoord2d(values[0], values[1]);
}

void
glTexCoord2fv(const GLfloat *values)
{
  glTexCoord2d(values[0], values[1]);
}

void
glTexCoord3d(GLdouble s, GLdouble t, GLdouble r)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setTexCoord(CPoint3D(s, t, r));
}

void
glTexCoord3f(GLfloat s, GLfloat t, GLfloat r)
{
  glTexCoord3d(s, t, r);
}

void
glTexCoord3fv(const GLfloat *v)
{
  glTexCoord3d(v[0], v[1], v[2]);
}

void
glTexCoordPointer(GLint /*size*/, GLenum /*type*/, GLsizei /*stride*/, const GLvoid * /*pointer*/)
{
  CGLMgrInst->unimplemented("glTexCoordPointer");
}

void
glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (target != GL_TEXTURE_ENV || pname != GL_TEXTURE_ENV_MODE)
    CGLMgrInst->unimplemented("glTexEnvf");

  gl->modifyTextureEnv().setMode(uint(param));
}

void
glTexEnvi(GLenum target, GLenum pname, GLint param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (target != GL_TEXTURE_ENV || pname != GL_TEXTURE_ENV_MODE)
    CGLMgrInst->unimplemented("glTexEnvi");

  gl->modifyTextureEnv().setMode(param);
}

void
glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (target != GL_TEXTURE_ENV)
    CGLMgrInst->unimplemented("glTexEnvfv");

  if      (pname == GL_TEXTURE_ENV_MODE)
    gl->modifyTextureEnv().setMode(uint(params[0]));
  else if (pname == GL_TEXTURE_ENV_COLOR)
    gl->modifyTextureEnv().setColor(
      CRGBA(params[0], params[1], params[2], params[3]));
  else
    CGLMgrInst->unimplemented("glTexEnvfv");
}

void
glTexEnviv(GLenum target, GLenum pname, const GLint *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (target != GL_TEXTURE_ENV)
    CGLMgrInst->unimplemented("glTexEnviv");

  if      (pname == GL_TEXTURE_ENV_MODE)
    gl->modifyTextureEnv().setMode(params[0]);
  else if (pname == GL_TEXTURE_ENV_COLOR)
    gl->modifyTextureEnv().setColor(
      CRGBA(params[0], params[1], params[2], params[3]));
  else
    CGLMgrInst->unimplemented("glTexEnviv");
}

void
glTexGendv(GLenum /*coord*/, GLenum /*pname*/, const GLdouble * /*params*/)
{
  CGLMgrInst->unimplemented("glTexGendv");
}

void
glTexGenfv(GLenum coord, GLenum pname, const GLfloat *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (pname == GL_TEXTURE_GEN_MODE) {
    if      (coord == GL_S)
      gl->modifyTextureGen().setSMode(uint(params[0]));
    else if (coord == GL_T)
      gl->modifyTextureGen().setTMode(uint(params[0]));
    else if (coord == GL_R)
      gl->modifyTextureGen().setRMode(uint(params[0]));
    else if (coord == GL_Q)
      gl->modifyTextureGen().setQMode(uint(params[0]));
    else
      CGLMgrInst->unimplemented("glTexGenfv");
  }
  else if (pname == GL_OBJECT_PLANE) {
    CPoint4D point(params[0], params[1], params[2], params[3]);

    if      (coord == GL_S)
      gl->modifyTextureGen().setSObjectPlane(point);
    else if (coord == GL_T)
      gl->modifyTextureGen().setTObjectPlane(point);
    else if (coord == GL_R)
      gl->modifyTextureGen().setRObjectPlane(point);
    else if (coord == GL_Q)
      gl->modifyTextureGen().setQObjectPlane(point);
    else
      CGLMgrInst->unimplemented("glTexGenfv");
  }
  else if (pname == GL_EYE_PLANE) {
    CPoint4D point(params[0], params[1], params[2], params[3]);

    if      (coord == GL_S)
      gl->modifyTextureGen().setSEyePlane(point);
    else if (coord == GL_T)
      gl->modifyTextureGen().setTEyePlane(point);
    else if (coord == GL_R)
      gl->modifyTextureGen().setREyePlane(point);
    else if (coord == GL_Q)
      gl->modifyTextureGen().setQEyePlane(point);
    else
      CGLMgrInst->unimplemented("glTexGenfv");
  }
  else
    CGLMgrInst->unimplemented("glTexGenfv");
}

void
glTexGeniv(GLenum /*coord*/, GLenum /*pname*/, const GLint * /*params*/)
{
  CGLMgrInst->unimplemented("glTexGeniv");
}

void
glTexGend(GLenum coord, GLenum pname, GLdouble param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (pname == GL_TEXTURE_GEN_MODE) {
    if      (coord == GL_S)
      gl->modifyTextureGen().setSMode(uint(param));
    else if (coord == GL_T)
      gl->modifyTextureGen().setTMode(uint(param));
    else if (coord == GL_R)
      gl->modifyTextureGen().setRMode(uint(param));
    else if (coord == GL_Q)
      gl->modifyTextureGen().setQMode(uint(param));
    else
      CGLMgrInst->unimplemented("glTexGend");
  }
  else
    CGLMgrInst->unimplemented("glTexGend");
}

void
glTexGenf(GLenum coord, GLenum pname, GLfloat param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (pname == GL_TEXTURE_GEN_MODE) {
    if      (coord == GL_S)
      gl->modifyTextureGen().setSMode(uint(param));
    else if (coord == GL_T)
      gl->modifyTextureGen().setTMode(uint(param));
    else if (coord == GL_R)
      gl->modifyTextureGen().setRMode(uint(param));
    else if (coord == GL_Q)
      gl->modifyTextureGen().setQMode(uint(param));
    else
      CGLMgrInst->unimplemented("glTexGenf");
  }
  else
    CGLMgrInst->unimplemented("glTexGenf");
}

void
glTexGeni(GLenum coord, GLenum pname, GLint param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (pname == GL_TEXTURE_GEN_MODE) {
    if      (coord == GL_S)
      gl->modifyTextureGen().setSMode(param);
    else if (coord == GL_T)
      gl->modifyTextureGen().setTMode(param);
    else if (coord == GL_R)
      gl->modifyTextureGen().setRMode(param);
    else if (coord == GL_Q)
      gl->modifyTextureGen().setQMode(param);
    else
      CGLMgrInst->unimplemented("glTexGeni");
  }
  else
    CGLMgrInst->unimplemented("glTexGeni");
}

void
glTexImage1D(GLenum target, GLint level, GLint /*internalFormat*/,
             GLsizei width, GLint /*border*/, GLenum format,
             GLenum type, const GLvoid *pixels)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (target != GL_TEXTURE_1D) {
    CGLMgrInst->unimplemented("glTexImage1D");
    return;
  }

  uint ind = gl->getTexture1Data().getCurrentInd();

  CGLTextureKey key(ind, level);

  CImagePtr image = gl->modifyTexture1Data().getTexture(key);

  image->setDataSize(width, 1);

  if      (format == GL_RGBA) {
    if (type == GL_UNSIGNED_BYTE) {
      uchar *p = (uchar *) pixels;

      for (int x = 0; x < width; x++, p += 4) {
        CRGBA rgba(CMathGen::mapToReal(p[0]),
                   CMathGen::mapToReal(p[1]),
                   CMathGen::mapToReal(p[2]),
                   CMathGen::mapToReal(p[3]));

        image->setRGBAPixel(x, 0, rgba);
      }
    }
  }
  else if (format == GL_RGB) {
    if (type == GL_UNSIGNED_BYTE) {
      uchar *p = (uchar *) pixels;

      for (int x = 0; x < width; x++, p += 3) {
        CRGBA rgba(CMathGen::mapToReal(p[0]),
                   CMathGen::mapToReal(p[1]),
                   CMathGen::mapToReal(p[2]));

        image->setRGBAPixel(x, 0, rgba);
      }
    }
  }
  else
    CGLMgrInst->unimplemented("glTexImage1D");

  gl->modifyTexture1Data().setTexture(key, image);
}

void
glTexImage2D(GLenum target, GLint level, GLint internalFormat,
             GLsizei width, GLsizei height, GLint /*border*/,
             GLenum format, GLenum type, const GLvoid *pixels)
{
  if      (internalFormat == 1)
    internalFormat = GL_LUMINANCE;
  else if (internalFormat == 2)
    internalFormat = GL_LUMINANCE_ALPHA;
  else if (internalFormat == 3)
    internalFormat = GL_RGB;
  else if (internalFormat == 4)
    internalFormat = GL_RGBA;

  //------

  CGL *gl = CGLMgrInst->getCurrentGL();

  if (target != GL_TEXTURE_2D) {
    CGLMgrInst->unimplemented("glTexImage2D");
    return;
  }

  uint ind = gl->getTexture2Data().getCurrentInd();

  CGLTextureKey key(ind, level);

  CImagePtr image = gl->modifyTexture2Data().getTextureImage(key);

  image->setDataSize(width, height);

  if      (format == GL_RGBA) {
    if (type == GL_UNSIGNED_BYTE) {
      uchar *p = (uchar *) pixels;

      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++, p += 4) {
          CRGBA rgba(CMathGen::mapToReal(p[0]),
                     CMathGen::mapToReal(p[1]),
                     CMathGen::mapToReal(p[2]),
                     CMathGen::mapToReal(p[3]));

          image->setRGBAPixel(x, y, rgba);
        }
      }
    }
  }
  else if (format == GL_RGB) {
    if (type == GL_UNSIGNED_BYTE) {
      uchar *p = (uchar *) pixels;

      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++, p += 3) {
          CRGBA rgba(CMathGen::mapToReal(p[0]),
                     CMathGen::mapToReal(p[1]),
                     CMathGen::mapToReal(p[2]));

          image->setRGBAPixel(x, y, rgba);
        }
      }
    }
  }
  else
    CGLMgrInst->unimplemented("glTexImage2D");

  gl->modifyTexture2Data().setTexture(key, image, internalFormat);
}

void
glTexImage3D(GLenum target, GLint level, GLint /*internalFormat*/,
             GLsizei width, GLsizei height, GLsizei depth,
             GLint /*border*/, GLenum format, GLenum type,
             const GLvoid *pixels)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if (target != GL_TEXTURE_3D) {
    CGLMgrInst->unimplemented("glTexImage3D");
    return;
  }

  uint ind = gl->getTexture3Data().getCurrentInd();

  CGLTextureKey key(ind, level);

  CGLTexture3Data::CImagePtrStack stack =
    gl->modifyTexture3Data().getTexture(key, depth);

  if      (format == GL_RGBA) {
    if (type == GL_UNSIGNED_BYTE) {
      uchar *p = (uchar *) pixels;

      for (int z = 0; z < depth; z++) {
        CImagePtr image = stack[z];

        image->setDataSize(width, height);

        for (int y = 0; y < height; y++) {
          for (int x = 0; x < width; x++, p += 4) {
            CRGBA rgba(CMathGen::mapToReal(p[0]),
                       CMathGen::mapToReal(p[1]),
                       CMathGen::mapToReal(p[2]),
                       CMathGen::mapToReal(p[3]));

            image->setRGBAPixel(x, y, rgba);
          }
        }
      }
    }
  }
  else if (format == GL_RGB) {
    if (type == GL_UNSIGNED_BYTE) {
      uchar *p = (uchar *) pixels;

      for (int z = 0; z < depth; z++) {
        CImagePtr image = stack[z];

        image->setDataSize(width, height);

        for (int y = 0; y < height; y++) {
          for (int x = 0; x < width; x++, p += 3) {
            CRGBA rgba(CMathGen::mapToReal(p[0]),
                       CMathGen::mapToReal(p[1]),
                       CMathGen::mapToReal(p[2]));

            image->setRGBAPixel(x, y, rgba);
          }
        }
      }
    }
  }
  else
    CGLMgrInst->unimplemented("glTexImage3D");

  gl->modifyTexture3Data().setTexture(key, stack);
}

void
glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (target == GL_TEXTURE_1D) {
    switch (pname) {
      case GL_TEXTURE_WRAP_S:
        gl->modifyTexture1Data().setWrapS(uint(param));
        break;
      case GL_TEXTURE_MAG_FILTER:
        gl->modifyTexture1Data().setMagFilter(uint(param));
        break;
      case GL_TEXTURE_MIN_FILTER:
        gl->modifyTexture1Data().setMinFilter(uint(param));
        break;
      default:
        CGLMgrInst->unimplemented("glTexParameterf");
        break;
    }
  }
  else if (target == GL_TEXTURE_2D) {
    switch (pname) {
      case GL_TEXTURE_WRAP_S:
        gl->modifyTexture2Data().setWrapS(uint(param));
        break;
      case GL_TEXTURE_WRAP_T:
        gl->modifyTexture2Data().setWrapT(uint(param));
        break;
      case GL_TEXTURE_MAG_FILTER:
        gl->modifyTexture2Data().setMagFilter(uint(param));
        break;
      case GL_TEXTURE_MIN_FILTER:
        gl->modifyTexture2Data().setMinFilter(uint(param));
        break;
      default:
        CGLMgrInst->unimplemented("glTexParameterf");
        break;
    }
  }
  else if (target == GL_TEXTURE_3D) {
    switch (pname) {
      case GL_TEXTURE_WRAP_S:
        gl->modifyTexture3Data().setWrapS(uint(param));
        break;
      case GL_TEXTURE_WRAP_T:
        gl->modifyTexture3Data().setWrapT(uint(param));
        break;
      case GL_TEXTURE_WRAP_R:
        gl->modifyTexture3Data().setWrapR(uint(param));
        break;
      case GL_TEXTURE_MAG_FILTER:
        gl->modifyTexture3Data().setMagFilter(uint(param));
        break;
      case GL_TEXTURE_MIN_FILTER:
        gl->modifyTexture3Data().setMinFilter(uint(param));
        break;
      default:
        CGLMgrInst->unimplemented("glTexParameterf");
        break;
    }
  }
  else
    CGLMgrInst->unimplemented("glTexParameterf");
}

void
glTexParameterfv(GLenum target, GLenum pname, const GLfloat *params)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (target == GL_TEXTURE_1D) {
    switch (pname) {
      case GL_TEXTURE_BORDER_COLOR:
        gl->modifyTexture1Data().setBorderColor(
         CRGBA(params[0], params[1], params[2], params[3]));
        break;
      default:
        CGLMgrInst->unimplemented("glTexParameterfv");
        break;
    }
  }
  else if (target == GL_TEXTURE_2D) {
    switch (pname) {
      case GL_TEXTURE_BORDER_COLOR:
        gl->modifyTexture2Data().setBorderColor(
         CRGBA(params[0], params[1], params[2], params[3]));
        break;
      default:
        CGLMgrInst->unimplemented("glTexParameterfv");
        break;
    }
  }
  else if (target == GL_TEXTURE_3D) {
    switch (pname) {
      case GL_TEXTURE_BORDER_COLOR:
        gl->modifyTexture3Data().setBorderColor(
         CRGBA(params[0], params[1], params[2], params[3]));
        break;
      default:
        CGLMgrInst->unimplemented("glTexParameterfv");
        break;
    }
  }
  else
    CGLMgrInst->unimplemented("glTexParameterfv");
}

void
glTexParameteri(GLenum target, GLenum pname, GLint param)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  if      (target == GL_TEXTURE_1D) {
    switch (pname) {
      case GL_TEXTURE_WRAP_S:
        gl->modifyTexture1Data().setWrapS(param);
        break;
      case GL_TEXTURE_MAG_FILTER:
        gl->modifyTexture1Data().setMagFilter(param);
        break;
      case GL_TEXTURE_MIN_FILTER:
        gl->modifyTexture1Data().setMinFilter(param);
        break;
      default:
        CGLMgrInst->unimplemented("glTexParameteri");
        break;
    }
  }
  else if (target == GL_TEXTURE_2D) {
    switch (pname) {
      case GL_TEXTURE_WRAP_S:
        gl->modifyTexture2Data().setWrapS(param);
        break;
      case GL_TEXTURE_WRAP_T:
        gl->modifyTexture2Data().setWrapT(param);
        break;
      case GL_TEXTURE_MAG_FILTER:
        gl->modifyTexture2Data().setMagFilter(param);
        break;
      case GL_TEXTURE_MIN_FILTER:
        gl->modifyTexture2Data().setMinFilter(param);
        break;
      default:
        CGLMgrInst->unimplemented("glTexParameteri");
        break;
    }
  }
  else if (target == GL_TEXTURE_3D) {
    switch (pname) {
      case GL_TEXTURE_WRAP_S:
        gl->modifyTexture3Data().setWrapS(param);
        break;
      case GL_TEXTURE_WRAP_T:
        gl->modifyTexture3Data().setWrapT(param);
        break;
      case GL_TEXTURE_WRAP_R:
        gl->modifyTexture3Data().setWrapR(param);
        break;
      case GL_TEXTURE_MAG_FILTER:
        gl->modifyTexture3Data().setMagFilter(param);
        break;
      case GL_TEXTURE_MIN_FILTER:
        gl->modifyTexture3Data().setMinFilter(param);
        break;
      default:
        CGLMgrInst->unimplemented("glTexParameteri");
        break;
    }
  }
  else
    CGLMgrInst->unimplemented("glTexParameteri");
}

void
glTexSubImage2D(GLenum /*target*/, GLint level, GLint xoffset, GLint yoffset,
                GLsizei width, GLsizei height, GLenum format,
                GLenum type, const GLvoid *pixels)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  uint ind = gl->getTexture2Data().getCurrentInd();

  CGLTextureKey key(ind, level);

  if (format == GL_RGBA) {
    CImagePtr image = gl->modifyTexture2Data().getTextureImage(key);

    if (type == GL_UNSIGNED_BYTE) {
      uchar *p = (uchar *) pixels;

      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++, p += 4) {
          CRGBA rgba(CMathGen::mapToReal(p[0]),
                     CMathGen::mapToReal(p[1]),
                     CMathGen::mapToReal(p[2]),
                     CMathGen::mapToReal(p[3]));

          image->setRGBAPixel(x + xoffset, y + yoffset, rgba);
        }
      }
    }
  }
}

void
glTranslated(GLdouble x, GLdouble y, GLdouble z)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->translateMatrix(x, y, z);
}

void
glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  glTranslated(x, y, z);
}

void
glVertex2d(GLdouble x, GLdouble y)
{
  glVertex3d(x, y, 0.0);
}

void
glVertex2dv(const GLdouble *c)
{
  glVertex3d(c[0], c[1], 0.0);
}

void
glVertex2i(GLint x, GLint y)
{
  glVertex3d(x, y, 0.0);
}

void
glVertex2iv(const GLint *c)
{
  glVertex3d(c[0], c[1], 0.0);
}

void
glVertex2f(GLfloat x, GLfloat y)
{
  glVertex3d(x, y, 0.0);
}

void
glVertex2fv(const GLfloat *v)
{
  glVertex3d(v[0], v[1], 0.0);
}

void
glVertex2s(GLshort x, GLshort y)
{
  glVertex3d(x, y, 0.0);
}

void
glVertex2sv(const GLshort *s)
{
  glVertex3d(s[0], s[1], 0.0);
}

void
glVertex3d(GLdouble x, GLdouble y, GLdouble z)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->addBlockPoint(CPoint3D(x, y, z));
}

void
glVertex3dv(const GLdouble *v)
{
  glVertex3d(v[0], v[1], v[2]);
}

void
glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
  glVertex3d(x, y, z);
}

void
glVertex3fv(const GLfloat *v)
{
  glVertex3d(v[0], v[1], v[2]);
}

void
glVertex3i(GLint x, GLint y, GLint z)
{
  glVertex3d(x, y, z);
}

void
glVertex3iv(const GLint *v)
{
  glVertex3d(v[0], v[1], v[2]);
}

void
glVertex3s(GLshort x, GLshort y, GLshort z)
{
  glVertex3d(x, y, z);
}

void
glVertex3sv(const GLshort *s)
{
  glVertex3d(s[0], s[1], s[2]);
}

void
glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->addBlockPoint(CPoint3D(x/w, y/z, z/w));
}

void
glVertex4dv(const GLdouble *s)
{
  glVertex4d(s[0], s[1], s[2], s[3]);
}

void
glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  glVertex4d(x, y, z, w);
}

void
glVertex4fv(const GLfloat *s)
{
  glVertex4d(s[0], s[1], s[2], s[3]);
}

void
glVertex4i(GLint x, GLint y, GLint z, GLint w)
{
  glVertex4d(x, y, z, w);
}

void
glVertex4iv(const GLint *s)
{
  glVertex4d(s[0], s[1], s[2], s[3]);
}

void
glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
  glVertex4d(x, y, z, w);
}

void
glVertex4sv(const GLshort *s)
{
  glVertex4d(s[0], s[1], s[2], s[3]);
}

void
glVertexPointer(GLint size, GLenum type, GLsizei stride,
                const GLvoid *pointer)
{
  gl_vertex_array_size    = size;
  gl_vertex_array_type    = type;
  gl_vertex_array_stride  = stride;
  gl_vertex_array_pointer = (char *) pointer;
}

void
glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setViewport(x, y, x + width - 1.0, y + height - 1.0);
}
