#include <CGL_glu.h>
#include <CGL.h>
#include <CGLTesselator.h>
#include <CGLQuadric.h>
#include <CGLNurbs.h>
#include <CMathGen.h>
#include <CMatrix3DH.h>

static void gluQuadricError(GLUquadric *quad, GLenum which);

void
gluBeginPolygon(GLUtesselator *tess)
{
  gluTessBeginPolygon(tess, NULL);
  gluTessBeginContour(tess);
}

void
gluEndPolygon(GLUtesselator *tess)
{
  gluTessEndContour(tess);
  gluTessEndPolygon(tess);
}

void
gluBeginSurface(GLUnurbs * /*nurb*/)
{
  CGLMgrInst->unimplemented("gluBeginSurface");
}

void
gluBeginTrim(GLUnurbs * /*nurb*/)
{
  CGLMgrInst->unimplemented("gluBeginTrim");
}

GLint
gluBuild2DMipmaps(GLenum target, GLint internalFormat, GLsizei width,
                  GLsizei height, GLenum format, GLenum type, const void *data)
{
  if (target != GL_TEXTURE_2D) {
    CGLMgrInst->unimplemented("gluBuild2DMipmaps");
    return -1;
  }

  uint size = std::max(width, height);

  int depth = 1;

  while (size > 1) {
    size >>= 1;

    ++depth;
  }

  int level = 1;

  CGL *gl = CGLMgrInst->getCurrentGL();

  uint ind = gl->getTexture2Data().getCurrentInd();

  glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);

  for (level = 1; level < depth; ++level) {
    CGLTextureKey key(ind, level - 1);

    CImagePtr image = gl->modifyTexture2Data().getTextureImage(key);

    int w = std::max(image->getSize().width  >> 1, 1);
    int h = std::max(image->getSize().height >> 1, 1);

    CImagePtr image1 = image->resize(CISize2D(w, h));

    key = CGLTextureKey(ind, level);

    gl->modifyTexture2Data().setTexture(key, image1, internalFormat);
  }

  return 0;
}

void
gluCylinder(GLUquadric *quad, GLdouble base, GLdouble top,
            GLdouble height, GLint slices, GLint stacks)
{
  enum { CACHE_SIZE = 240 };

  GLint   i, j;
  GLfloat sinCache[CACHE_SIZE];
  GLfloat cosCache[CACHE_SIZE];
  GLfloat sinCache2[CACHE_SIZE];
  GLfloat cosCache2[CACHE_SIZE];
  GLfloat sinCache3[CACHE_SIZE];
  GLfloat cosCache3[CACHE_SIZE];
  GLfloat angle;
  GLfloat zLow, zHigh;
  GLfloat sintemp, costemp;
  GLfloat length;
  GLfloat delta;
  GLfloat zNormal;
  GLfloat xyNormalRatio;
  GLfloat radiusLow, radiusHigh;
  int     needCache2, needCache3;

  if (slices >= CACHE_SIZE)
    slices = CACHE_SIZE-1;

  if (slices < 2 || stacks < 1 || base < 0.0 || top < 0.0 || height < 0.0) {
    gluQuadricError(quad, GLU_INVALID_VALUE);
    return;
  }

  /* Compute length (needed for normal calculations) */
  delta  = base - top;
  length = ::sqrt(delta*delta + height*height);

  if (length == 0.0) {
    gluQuadricError(quad, GLU_INVALID_VALUE);
    return;
  }

  /* Cache is the vertex locations cache */
  /* Cache2 is the various normals at the vertices themselves */
  /* Cache3 is the various normals for the faces */
  needCache2 = needCache3 = 0;

  if (quad->getNormals() == GLU_SMOOTH)
    needCache2 = 1;

  if (quad->getNormals() == GLU_FLAT) {
    if (quad->getDrawStyle() != GLU_POINT)
      needCache3 = 1;

    if (quad->getDrawStyle() == GLU_LINE)
      needCache2 = 1;
  }

  zNormal       = delta  / length;
  xyNormalRatio = height / length;

  for (i = 0; i < slices; i++) {
    angle = 2 * M_PI * i / slices;

    if (needCache2) {
      if (quad->getOrientation() == GLU_OUTSIDE) {
        sinCache2[i] = xyNormalRatio * ::sin(angle);
        cosCache2[i] = xyNormalRatio * ::cos(angle);
      }
      else {
        sinCache2[i] = -xyNormalRatio * ::sin(angle);
        cosCache2[i] = -xyNormalRatio * ::cos(angle);
      }
    }

    sinCache[i] = ::sin(angle);
    cosCache[i] = ::cos(angle);
  }

  if (needCache3) {
    for (i = 0; i < slices; i++) {
      angle = 2 * M_PI * (i-0.5) / slices;

      if (quad->getOrientation() == GLU_OUTSIDE) {
        sinCache3[i] = xyNormalRatio * ::sin(angle);
        cosCache3[i] = xyNormalRatio * ::cos(angle);
      }
      else {
        sinCache3[i] = -xyNormalRatio * ::sin(angle);
        cosCache3[i] = -xyNormalRatio * ::cos(angle);
      }
    }
  }

  sinCache[slices] = sinCache[0];
  cosCache[slices] = cosCache[0];

  if (needCache2) {
    sinCache2[slices] = sinCache2[0];
    cosCache2[slices] = cosCache2[0];
  }

  if (needCache3) {
    sinCache3[slices] = sinCache3[0];
    cosCache3[slices] = cosCache3[0];
  }

  switch (quad->getDrawStyle()) {
    case GLU_FILL:
      /* Note:
      ** An argument could be made for using a TRIANGLE_FAN for the end
      ** of the cylinder of either radii is 0.0 (a cone).  However, a
      ** TRIANGLE_FAN would not work in smooth shading mode (the common
      ** case) because the normal for the apex is different for every
      ** triangle (and TRIANGLE_FAN doesn't let me respecify that normal).
      ** Now, my choice is GL_TRIANGLES, or leave the GL_QUAD_STRIP and
      ** just let the GL trivially reject one of the two triangles of the
      ** QUAD.  GL_QUAD_STRIP is probably faster, so I will leave this code
      ** alone.
      */
      for (j = 0; j < stacks; j++) {
        zLow  =  j      * height / stacks;
        zHigh = (j + 1) * height / stacks;

        radiusLow  = base - delta * ((float)  j      / stacks);
        radiusHigh = base - delta * ((float) (j + 1) / stacks);

        glBegin(GL_QUAD_STRIP);

        for (i = 0; i <= slices; i++) {
          switch (quad->getNormals()) {
            case GLU_FLAT:
              glNormal3f(sinCache3[i], cosCache3[i], zNormal);
              break;
            case GLU_SMOOTH:
              glNormal3f(sinCache2[i], cosCache2[i], zNormal);
              break;
            case GLU_NONE:
            default:
              break;
          }

          if (quad->getOrientation() == GLU_OUTSIDE) {
            if (quad->getTexture())
              glTexCoord2f(1 - (float) i / slices, (float) j / stacks);

            glVertex3f(radiusLow * sinCache[i],
                       radiusLow * cosCache[i], zLow);

            if (quad->getTexture())
              glTexCoord2f(1 - (float) i / slices, (float) (j+1) / stacks);

            glVertex3f(radiusHigh * sinCache[i],
                       radiusHigh * cosCache[i], zHigh);
          }
          else {
            if (quad->getTexture())
              glTexCoord2f(1 - (float) i / slices, (float) (j+1) / stacks);

            glVertex3f(radiusHigh * sinCache[i],
                       radiusHigh * cosCache[i], zHigh);

            if (quad->getTexture())
              glTexCoord2f(1 - (float) i / slices, (float) j / stacks);

            glVertex3f(radiusLow * sinCache[i],
                       radiusLow * cosCache[i], zLow);
          }
        }

        glEnd();
      }

      break;
    case GLU_POINT:
      glBegin(GL_POINTS);

      for (i = 0; i < slices; i++) {
        switch (quad->getNormals()) {
          case GLU_FLAT:
          case GLU_SMOOTH:
            glNormal3f(sinCache2[i], cosCache2[i], zNormal);
            break;
          case GLU_NONE:
          default:
            break;
        }

        sintemp = sinCache[i];
        costemp = cosCache[i];

        for (j = 0; j <= stacks; j++) {
          zLow = j * height / stacks;

          radiusLow = base - delta * ((float) j / stacks);

          if (quad->getTexture())
            glTexCoord2f(1 - (float) i / slices, (float) j / stacks);

          glVertex3f(radiusLow * sintemp, radiusLow * costemp, zLow);
        }
      }

      glEnd();

      break;
    case GLU_LINE:
      for (j = 1; j < stacks; j++) {
        zLow = j * height / stacks;

        radiusLow = base - delta * ((float) j / stacks);

        glBegin(GL_LINE_STRIP);

        for (i = 0; i <= slices; i++) {
          switch (quad->getNormals()) {
            case GLU_FLAT:
              glNormal3f(sinCache3[i], cosCache3[i], zNormal);
              break;
            case GLU_SMOOTH:
              glNormal3f(sinCache2[i], cosCache2[i], zNormal);
              break;
            case GLU_NONE:
            default:
              break;
          }

          if (quad->getTexture())
            glTexCoord2f(1 - (float) i / slices, (float) j / stacks);

          glVertex3f(radiusLow * sinCache[i],
                    radiusLow * cosCache[i], zLow);
        }

        glEnd();
      }

      /* Intentionally fall through here... */
    case GLU_SILHOUETTE:
      for (j = 0; j <= stacks; j += stacks) {
        zLow = j * height / stacks;

        radiusLow = base - delta * ((float) j / stacks);

        glBegin(GL_LINE_STRIP);

        for (i = 0; i <= slices; i++) {
          switch (quad->getNormals()) {
                case GLU_FLAT:
                  glNormal3f(sinCache3[i], cosCache3[i], zNormal);
                  break;
                case GLU_SMOOTH:
                  glNormal3f(sinCache2[i], cosCache2[i], zNormal);
                  break;
                case GLU_NONE:
                default:
                  break;
              }
              if (quad->getTexture()) {
                  glTexCoord2f(1 - (float) i / slices,
                          (float) j / stacks);
              }
              glVertex3f(radiusLow * sinCache[i], radiusLow * cosCache[i],
                      zLow);
          }
          glEnd();
      }

      for (i = 0; i < slices; i++) {
        switch (quad->getNormals()) {
          case GLU_FLAT:
          case GLU_SMOOTH:
            glNormal3f(sinCache2[i], cosCache2[i], 0.0);
            break;
          case GLU_NONE:
          default:
            break;
        }

        sintemp = sinCache[i];
        costemp = cosCache[i];

        glBegin(GL_LINE_STRIP);

        for (j = 0; j <= stacks; j++) {
          zLow = j * height / stacks;

          radiusLow = base - delta * ((float) j / stacks);

          if (quad->getTexture())
            glTexCoord2f(1 - (float) i / slices, (float) j / stacks);

          glVertex3f(radiusLow * sintemp, radiusLow * costemp, zLow);
        }

        glEnd();
      }
      break;
    default:
      break;
  }
}

void
gluDeleteQuadric(GLUquadric* quad)
{
  delete quad;
}

void
gluDeleteTess(GLUtesselator *tess)
{
  delete tess;
}

void
gluDisk(GLUquadric *quad, GLdouble inner, GLdouble outer,
        GLint slices, GLint loops)
{
  gluPartialDisk(quad, inner, outer, slices, loops, 0.0, 360.0);
}

void
gluEndSurface(GLUnurbs * /*nurb*/)
{
  CGLMgrInst->unimplemented("gluEndSurface");
}

void
gluEndTrim(GLUnurbs * /*nurb*/)
{
  CGLMgrInst->unimplemented("gluEndTrim");
}

const GLubyte *
gluErrorString(GLenum error)
{
  switch (error) {
    case GLU_INVALID_ENUM:
      return (const GLubyte *) "Invalid Enum";
    case GLU_INVALID_VALUE:
      return (const GLubyte *) "Invalid Value";
    case GLU_OUT_OF_MEMORY:
      return (const GLubyte *) "Out of Memory";
    case GL_STACK_UNDERFLOW:
      return (const GLubyte *) "Stack Underflow";
    default:
      return (const GLubyte *) "Unknown Error";
  }
}

const GLubyte *
gluGetString(GLenum /*name*/)
{
  CGLMgrInst->unimplemented("gluGetString");

  return (const GLubyte *) "";
}

void
gluLookAt(GLdouble eyeX   , GLdouble eyeY   , GLdouble eyeZ   ,
          GLdouble centerX, GLdouble centerY, GLdouble centerZ,
          GLdouble upX    , GLdouble upY    , GLdouble upZ    )
{
  CVector3D f(centerX - eyeX, centerY - eyeY, centerZ - eyeZ);

  CVector3D up(upX, upY, upZ);

  f .normalize();
  up.normalize();

  CVector3D s = f.crossProduct(up);
  CVector3D u = s.crossProduct(f);

  f = -f;

  GLdouble m[16];

  memset(m, 0, sizeof(m));

  s.getXYZ(&m[ 0], &m[ 1], &m[ 2]);
  u.getXYZ(&m[ 4], &m[ 5], &m[ 6]);
  f.getXYZ(&m[ 8], &m[ 9], &m[10]);

  m[15] = 1;

  glMultMatrixd(m);

  glTranslated(-eyeX, -eyeY, -eyeZ);
}

GLUnurbs *
gluNewNurbsRenderer()
{
  return new GLUnurbs;
}

GLUquadric *
gluNewQuadric()
{
  return new GLUquadric;
}

GLUtesselator *
gluNewTess()
{
  return new GLUtesselator;
}

void
gluNextContour(GLUtesselator *tess, GLenum /*type*/)
{
  gluTessEndContour(tess);

  gluTessBeginContour(tess);
}

void
gluNurbsCallback(GLUnurbs *nurb, GLenum which, void (*func)())
{
  nurb->setCallback(which, func);
}

void
gluNurbsCurve(GLUnurbs * /*nurb*/, GLint /*knotCount*/, GLfloat * /*knots*/, GLint /*stride*/,
              GLfloat * /*control*/, GLint /*order*/, GLenum /*type*/)
{
  CGLMgrInst->unimplemented("gluNurbsCurve");
}

void
gluNurbsProperty(GLUnurbs * /*nurb*/, GLenum /*property*/, GLfloat /*value*/)
{
  CGLMgrInst->unimplemented("gluNurbsProperty");
}

void
gluNurbsSurface(GLUnurbs * /*nurb*/, GLint /*sKnotCount*/, GLfloat * /*sKnots*/,
                GLint /*tKnotCount*/, GLfloat * /*tKnots*/, GLint /*sStride*/,
                GLint /*tStride*/, GLfloat * /*control*/, GLint /*sOrder*/,
                GLint /*tOrder*/, GLenum /*type*/)
{
  CGLMgrInst->unimplemented("gluNurbsSurface");
}

void
gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
  glOrtho(left, right, bottom, top, -1, 1);
}

void
gluPartialDisk(GLUquadric *quad, GLdouble inner, GLdouble outer,
               GLint slices, GLint loops, GLdouble start, GLdouble sweep)
{
  enum { CACHE_SIZE = 240 };

  GLint   i, j;
  GLfloat sinCache[CACHE_SIZE];
  GLfloat cosCache[CACHE_SIZE];
  GLfloat angle;
  GLfloat sintemp, costemp;
  GLfloat delta;
  GLfloat radiusLow, radiusHigh;
  GLfloat texLow = 0.0, texHigh = 0.0;
  GLfloat angleOffset;
  GLint   slices2;
  GLint   finish;

  if (slices >= CACHE_SIZE)
    slices = CACHE_SIZE - 1;

  if (slices < 2 || loops < 1 || outer <= 0.0 || inner < 0.0 || inner > outer) {
    gluQuadricError(quad, GLU_INVALID_VALUE);
    return;
  }

  if (sweep < -360.0) sweep = 360.0;
  if (sweep >  360.0) sweep = 360.0;

  if (sweep < 0) {
    start += sweep;
    sweep = -sweep;
  }

  if (sweep == 360.0)
    slices2 = slices;
  else
    slices2 = slices + 1;

  /* Compute length (needed for normal calculations) */
  delta = outer - inner;

  /* Cache is the vertex locations cache */

  angleOffset = start / 180.0 * M_PI;

  for (i = 0; i <= slices; i++) {
    angle = angleOffset + ((M_PI * sweep) / 180.0) * i / slices;

    sinCache[i] = ::sin(angle);
    cosCache[i] = ::cos(angle);
  }

  if (sweep == 360.0) {
    sinCache[slices] = sinCache[0];
    cosCache[slices] = cosCache[0];
  }

  switch (quad->getNormals()) {
    case GLU_FLAT:
    case GLU_SMOOTH:
      if (quad->getOrientation() == GLU_OUTSIDE)
        glNormal3f(0.0, 0.0, 1.0);
      else
        glNormal3f(0.0, 0.0, -1.0);

      break;
    default:
    case GLU_NONE:
      break;
  }

  switch (quad->getDrawStyle()) {
    case GLU_FILL:
      if (inner == 0.0) {
        finish = loops - 1;

        /* Triangle strip for inner polygons */
        glBegin(GL_TRIANGLE_FAN);

        if (quad->getTexture())
          glTexCoord2f(0.5, 0.5);

        glVertex3f(0.0, 0.0, 0.0);

        radiusLow = outer - delta * ((float) (loops-1) / loops);

        if (quad->getTexture())
          texLow = radiusLow / outer / 2;

        if (quad->getOrientation() == GLU_OUTSIDE) {
          for (i = slices; i >= 0; i--) {
            if (quad->getTexture()) {
              glTexCoord2f(texLow * sinCache[i] + 0.5,

              texLow * cosCache[i] + 0.5);
            }

            glVertex3f(radiusLow * sinCache[i], radiusLow * cosCache[i], 0.0);
          }
        }
        else {
          for (i = 0; i <= slices; i++) {
            if (quad->getTexture()) {
              glTexCoord2f(texLow * sinCache[i] + 0.5,

              texLow * cosCache[i] + 0.5);
            }

            glVertex3f(radiusLow * sinCache[i], radiusLow * cosCache[i], 0.0);
          }
        }

        glEnd();
      }
      else
        finish = loops;

      for (j = 0; j < finish; j++) {
        radiusLow  = outer - delta * ((float)  j      / loops);
        radiusHigh = outer - delta * ((float) (j + 1) / loops);

        if (quad->getTexture()) {
          texLow  = radiusLow  / outer / 2;
          texHigh = radiusHigh / outer / 2;
        }

        glBegin(GL_QUAD_STRIP);

        for (i = 0; i <= slices; i++) {
          if (quad->getOrientation() == GLU_OUTSIDE) {
            if (quad->getTexture()) {
              glTexCoord2f(texLow * sinCache[i] + 0.5,

              texLow * cosCache[i] + 0.5);
            }

            glVertex3f(radiusLow * sinCache[i], radiusLow * cosCache[i], 0.0);

            if (quad->getTexture())
              glTexCoord2f(texHigh * sinCache[i] + 0.5,
                           texHigh * cosCache[i] + 0.5);

            glVertex3f(radiusHigh * sinCache[i],
                       radiusHigh * cosCache[i], 0.0);
          }
          else {
            if (quad->getTexture())
              glTexCoord2f(texHigh * sinCache[i] + 0.5,
                           texHigh * cosCache[i] + 0.5);

            glVertex3f(radiusHigh * sinCache[i],
                       radiusHigh * cosCache[i], 0.0);

            if (quad->getTexture())
              glTexCoord2f(texLow * sinCache[i] + 0.5,
                           texLow * cosCache[i] + 0.5);

            glVertex3f(radiusLow * sinCache[i],
                       radiusLow * cosCache[i], 0.0);
          }
        }

        glEnd();
      }

      break;
    case GLU_POINT:
      glBegin(GL_POINTS);

      for (i = 0; i < slices2; i++) {
        sintemp = sinCache[i];
        costemp = cosCache[i];

        for (j = 0; j <= loops; j++) {
          radiusLow = outer - delta * ((float) j / loops);

          if (quad->getTexture()) {
            texLow = radiusLow / outer / 2;

            glTexCoord2f(texLow * sinCache[i] + 0.5,
                         texLow * cosCache[i] + 0.5);
          }

          glVertex3f(radiusLow * sintemp, radiusLow * costemp, 0.0);
        }
      }

      glEnd();

      break;
    case GLU_LINE:
      if (inner == outer) {
        glBegin(GL_LINE_STRIP);

        for (i = 0; i <= slices; i++) {
          if (quad->getTexture())
            glTexCoord2f(sinCache[i] / 2 + 0.5, cosCache[i] / 2 + 0.5);

          glVertex3f(inner * sinCache[i], inner * cosCache[i], 0.0);
        }

        glEnd();

        break;
      }

      for (j = 0; j <= loops; j++) {
        radiusLow = outer - delta * ((float) j / loops);

        if (quad->getTexture())
          texLow = radiusLow / outer / 2;

        glBegin(GL_LINE_STRIP);

        for (i = 0; i <= slices; i++) {
          if (quad->getTexture())
            glTexCoord2f(texLow * sinCache[i] + 0.5,
                         texLow * cosCache[i] + 0.5);

          glVertex3f(radiusLow * sinCache[i],
                     radiusLow * cosCache[i], 0.0);
        }

        glEnd();
      }

      for (i = 0; i < slices2; i++) {
        sintemp = sinCache[i];
        costemp = cosCache[i];

        glBegin(GL_LINE_STRIP);

        for (j = 0; j <= loops; j++) {
          radiusLow = outer - delta * ((float) j / loops);

          if (quad->getTexture())
            texLow = radiusLow / outer / 2;

          if (quad->getTexture())
            glTexCoord2f(texLow * sinCache[i] + 0.5,
                         texLow * cosCache[i] + 0.5);

          glVertex3f(radiusLow * sintemp, radiusLow * costemp, 0.0);
        }

        glEnd();
      }

      break;
    case GLU_SILHOUETTE:
      if (sweep < 360.0) {
        for (i = 0; i <= slices; i+= slices) {
          sintemp = sinCache[i];
          costemp = cosCache[i];

          glBegin(GL_LINE_STRIP);

          for (j = 0; j <= loops; j++) {
            radiusLow = outer - delta * ((float) j / loops);

            if (quad->getTexture()) {
              texLow = radiusLow / outer / 2;

              glTexCoord2f(texLow * sinCache[i] + 0.5,
                           texLow * cosCache[i] + 0.5);
            }

            glVertex3f(radiusLow * sintemp, radiusLow * costemp, 0.0);
          }

          glEnd();
        }
      }

      for (j = 0; j <= loops; j += loops) {
        radiusLow = outer - delta * ((float) j / loops);

        if (quad->getTexture())
          texLow = radiusLow / outer / 2;

        glBegin(GL_LINE_STRIP);

        for (i = 0; i <= slices; i++) {
          if (quad->getTexture())
            glTexCoord2f(texLow * sinCache[i] + 0.5,
                         texLow * cosCache[i] + 0.5);

          glVertex3f(radiusLow * sinCache[i],
                     radiusLow * cosCache[i], 0.0);
        }

        glEnd();

        if (inner == outer)
          break;
      }

      break;
    default:
      break;
  }
}

void
gluPerspective(GLdouble fov, GLdouble aspect, GLdouble near, GLdouble far)
{
  CGL *gl = CGLMgrInst->getCurrentGL();

  CMatrix3DH matrix;

  matrix.buildPerspective(fov, aspect, near, far);

  gl->multMatrix(&matrix);
}

void
gluPickMatrix(GLdouble x, GLdouble y, GLdouble delX, GLdouble delY,
              GLint *viewport)
{
  if (delX <= 0 || delY <= 0)
    return;

  /* Translate and scale the picked region to the entire window */
  glTranslatef((viewport[2] - 2*(x - viewport[0]))/delX,
               (viewport[3] - 2*(y - viewport[1]))/delY, 0);

  glScalef(viewport[2] / delX, viewport[3] / delY, 1.0);
}

void
gluPwlCurve(GLUnurbs * /*nurb*/, GLint /*count*/, GLfloat * /*data*/,
            GLint /*stride*/, GLenum /*type*/)
{
  CGLMgrInst->unimplemented("gluPwlCurve");
}

void
gluQuadricCallback(GLUquadric *quad, GLenum which, GLvoid (*func)())
{
  quad->setCallback(which, func);
}

static void
gluQuadricError(GLUquadric * /*quad*/, GLenum /*which*/)
{
  CGLMgrInst->unimplemented("gluQuadricError");
}

void
gluQuadricOrientation(GLUquadric *quad, GLenum orientation)
{
  quad->setOrientation(orientation);
}

void
gluQuadricTexture(GLUquadric *quad, GLboolean texture)
{
  quad->setTexture(texture);
}

void
gluQuadricDrawStyle(GLUquadric *quad, GLenum drawStyle)
{
  quad->setDrawStyle(drawStyle);
}

void
gluQuadricNormals(GLUquadric *quad, uint normals)
{
  quad->setNormals(normals);
}

GLint
gluScaleImage(GLenum /*format*/, GLsizei /*wIn*/, GLsizei /*hIn*/, GLenum /*typeIn*/,
              const void * /*dataIn*/, GLsizei /*wOut*/, GLsizei /*hOut*/,
              GLenum /*typeOut*/, GLvoid* /*dataOut*/)
{
  CGLMgrInst->unimplemented("gluScaleImage");

  return 0;
}

void
gluSphere(GLUquadric *quad, GLdouble radius, GLint slices, GLint stacks)
{
  enum { CACHE_SIZE = 240 };

  typedef std::vector<GLfloat> GLfloatArray;

  GLfloatArray sinCache1a; sinCache1a.resize(CACHE_SIZE);
  GLfloatArray cosCache1a; cosCache1a.resize(CACHE_SIZE);
  GLfloatArray sinCache2a; sinCache2a.resize(CACHE_SIZE);
  GLfloatArray cosCache2a; cosCache2a.resize(CACHE_SIZE);
  GLfloatArray sinCache3a; sinCache3a.resize(CACHE_SIZE);
  GLfloatArray cosCache3a; cosCache3a.resize(CACHE_SIZE);
  GLfloatArray sinCache1b; sinCache1b.resize(CACHE_SIZE);
  GLfloatArray cosCache1b; cosCache1b.resize(CACHE_SIZE);
  GLfloatArray sinCache2b; sinCache2b.resize(CACHE_SIZE);
  GLfloatArray cosCache2b; cosCache2b.resize(CACHE_SIZE);
  GLfloatArray sinCache3b; sinCache3b.resize(CACHE_SIZE);
  GLfloatArray cosCache3b; cosCache3b.resize(CACHE_SIZE);

  GLint     i, j;
  GLfloat   angle;
  GLfloat   zLow, zHigh;
  GLfloat   sintemp1, sintemp2, sintemp3 = 0.0, sintemp4 = 0.0;
  GLfloat   costemp1, costemp2 = 0.0, costemp3 = 0.0, costemp4 = 0.0;
  GLboolean needCache2, needCache3;
  GLint     start, finish;

  if (slices >= CACHE_SIZE) slices = CACHE_SIZE-1;
  if (stacks >= CACHE_SIZE) stacks = CACHE_SIZE-1;

  if (slices < 2 || stacks < 1 || radius < 0.0) {
    gluQuadricError(quad, GLU_INVALID_VALUE);
    return;
  }

  /* Cache is the vertex locations cache */
  /* Cache2 is the various normals at the vertices themselves */
  /* Cache3 is the various normals for the faces */
  needCache2 = needCache3 = GL_FALSE;

  if (quad->getNormals() == GLU_SMOOTH)
    needCache2 = GL_TRUE;

  if (quad->getNormals() == GLU_FLAT) {
    if (quad->getDrawStyle() != GLU_POINT)
      needCache3 = GL_TRUE;

    if (quad->getDrawStyle() == GLU_LINE)
      needCache2 = GL_TRUE;
  }

  for (i = 0; i < slices; i++) {
    angle = 2 * M_PI * i / slices;

    sinCache1a[i] = ::sin(angle);
    cosCache1a[i] = ::cos(angle);

    if (needCache2) {
      sinCache2a[i] = sinCache1a[i];
      cosCache2a[i] = cosCache1a[i];
    }
  }

  for (j = 0; j <= stacks; j++) {
    angle = M_PI * j / stacks;

    if (needCache2) {
      if (quad->getOrientation() == GLU_OUTSIDE) {
        sinCache2b[j] = ::sin(angle);
        cosCache2b[j] = ::cos(angle);
      }
      else {
        sinCache2b[j] = -::sin(angle);
        cosCache2b[j] = -::cos(angle);
      }
    }

    sinCache1b[j] = radius * ::sin(angle);
    cosCache1b[j] = radius * ::cos(angle);
  }

  /* Make sure it comes to a point */
  sinCache1b[0     ] = 0;
  sinCache1b[stacks] = 0;

  if (needCache3) {
    for (i = 0; i < slices; i++) {
      angle = 2 * M_PI * (i-0.5) / slices;

      sinCache3a[i] = ::sin(angle);
      cosCache3a[i] = ::cos(angle);
    }

    for (j = 0; j <= stacks; j++) {
      angle = M_PI * (j - 0.5) / stacks;

      if (quad->getOrientation() == GLU_OUTSIDE) {
        sinCache3b[j] = ::sin(angle);
        cosCache3b[j] = ::cos(angle);
      }
      else {
        sinCache3b[j] = -::sin(angle);
        cosCache3b[j] = -::cos(angle);
      }
    }
  }

  sinCache1a[slices] = sinCache1a[0];
  cosCache1a[slices] = cosCache1a[0];

  if (needCache2) {
    sinCache2a[slices] = sinCache2a[0];
    cosCache2a[slices] = cosCache2a[0];
  }

  if (needCache3) {
    sinCache3a[slices] = sinCache3a[0];
    cosCache3a[slices] = cosCache3a[0];
  }

  switch (quad->getDrawStyle()) {
    case GLU_FILL:
      /* Do ends of sphere as TRIANGLE_FAN's (if not texturing)
      ** We don't do it when texturing because we need to respecify the
      ** texture coordinates of the apex for every adjacent vertex (because
      ** it isn't a constant for that point)
      */
      if (! quad->getTexture()) {
        start  = 1;
        finish = stacks - 1;

        /* Low end first (j == 0 iteration) */
        sintemp2 = sinCache1b[1];
        zHigh    = cosCache1b[1];

        switch (quad->getNormals()) {
          case GLU_FLAT:
            sintemp3 = sinCache3b[1];
            costemp3 = cosCache3b[1];
            break;
          case GLU_SMOOTH:
            sintemp3 = sinCache2b[1];
            costemp3 = cosCache2b[1];

            glNormal3f(sinCache2a[0] * sinCache2b[0],
                       cosCache2a[0] * sinCache2b[0],
                       cosCache2b[0]);
            break;
          default:
            break;
        }

        glBegin(GL_TRIANGLE_FAN);

        glVertex3f(0.0, 0.0, radius);

        if (quad->getOrientation() == GLU_OUTSIDE) {
          for (i = slices; i >= 0; i--) {
            switch (quad->getNormals()) {
              case GLU_SMOOTH:
                glNormal3f(sinCache2a[i] * sintemp3,
                           cosCache2a[i] * sintemp3,
                           costemp3);
                break;
              case GLU_FLAT:
                if (i != slices)
                  glNormal3f(sinCache3a[i+1] * sintemp3,
                             cosCache3a[i+1] * sintemp3,
                             costemp3);

                break;
              case GLU_NONE:
              default:
                break;
            }

            glVertex3f(sintemp2 * sinCache1a[i],
                       sintemp2 * cosCache1a[i], zHigh);
          }
        }
        else {
          for (i = 0; i <= slices; i++) {
            switch (quad->getNormals()) {
              case GLU_SMOOTH:
                glNormal3f(sinCache2a[i] * sintemp3,
                           cosCache2a[i] * sintemp3,
                           costemp3);
                break;
              case GLU_FLAT:
                glNormal3f(sinCache3a[i] * sintemp3,
                           cosCache3a[i] * sintemp3,
                           costemp3);
                break;
              case GLU_NONE:
              default:
                break;
            }

            glVertex3f(sintemp2 * sinCache1a[i],
                       sintemp2 * cosCache1a[i], zHigh);
          }
        }

        glEnd();

        /* High end next (j == stacks-1 iteration) */
        sintemp2 = sinCache1b[stacks-1];
        zHigh    = cosCache1b[stacks-1];

        switch (quad->getNormals()) {
          case GLU_FLAT:
            sintemp3 = sinCache3b[stacks];
            costemp3 = cosCache3b[stacks];
            break;
          case GLU_SMOOTH:
            sintemp3 = sinCache2b[stacks-1];
            costemp3 = cosCache2b[stacks-1];

            glNormal3f(sinCache2a[stacks] * sinCache2b[stacks],
                       cosCache2a[stacks] * sinCache2b[stacks],
                       cosCache2b[stacks]);
            break;
          default:
            break;
        }

        glBegin(GL_TRIANGLE_FAN);

        glVertex3f(0.0, 0.0, -radius);

        if (quad->getOrientation() == GLU_OUTSIDE) {
          for (i = 0; i <= slices; i++) {
            switch (quad->getNormals()) {
              case GLU_SMOOTH:
                glNormal3f(sinCache2a[i] * sintemp3,
                           cosCache2a[i] * sintemp3,
                           costemp3);
                break;
              case GLU_FLAT:
                glNormal3f(sinCache3a[i] * sintemp3,
                           cosCache3a[i] * sintemp3,
                           costemp3);
                break;
              case GLU_NONE:
              default:
                break;
            }

            glVertex3f(sintemp2 * sinCache1a[i],
                       sintemp2 * cosCache1a[i], zHigh);
          }
        }
        else {
          for (i = slices; i >= 0; i--) {
            switch (quad->getNormals()) {
              case GLU_SMOOTH:
                glNormal3f(sinCache2a[i] * sintemp3,
                           cosCache2a[i] * sintemp3,
                           costemp3);
                break;
              case GLU_FLAT:
                if (i != slices)
                  glNormal3f(sinCache3a[i+1] * sintemp3,
                             cosCache3a[i+1] * sintemp3,
                             costemp3);

                break;
              case GLU_NONE:
              default:
                break;
            }

            glVertex3f(sintemp2 * sinCache1a[i],
                       sintemp2 * cosCache1a[i], zHigh);
          }
        }

        glEnd();
      }
      else {
        start = 0;
        finish = stacks;
      }

      for (j = start; j < finish; j++) {
        zLow  = cosCache1b[j];
        zHigh = cosCache1b[j+1];

        sintemp1 = sinCache1b[j];
        sintemp2 = sinCache1b[j+1];

        switch (quad->getNormals()) {
          case GLU_FLAT:
            sintemp4 = sinCache3b[j+1];
            costemp4 = cosCache3b[j+1];
            break;
          case GLU_SMOOTH:
            if (quad->getOrientation() == GLU_OUTSIDE) {
              sintemp3 = sinCache2b[j+1];
              costemp3 = cosCache2b[j+1];
              sintemp4 = sinCache2b[j];
              costemp4 = cosCache2b[j];
            }
            else {
              sintemp3 = sinCache2b[j];
              costemp3 = cosCache2b[j];
              sintemp4 = sinCache2b[j+1];
              costemp4 = cosCache2b[j+1];
            }
            break;
          default:
            break;
        }

        glBegin(GL_QUAD_STRIP);

        for (i = 0; i <= slices; i++) {
          switch (quad->getNormals()) {
            case GLU_SMOOTH:
              glNormal3f(sinCache2a[i] * sintemp3,
                         cosCache2a[i] * sintemp3,
                         costemp3);
              break;
            case GLU_FLAT:
            case GLU_NONE:
            default:
              break;
          }

          if (quad->getOrientation() == GLU_OUTSIDE) {
            if (quad->getTexture())
              glTexCoord2f(1 - (float) i / slices,
                           1 - (float) (j+1) / stacks);

            glVertex3f(sintemp2 * sinCache1a[i],
                       sintemp2 * cosCache1a[i], zHigh);
          }
          else {
            if (quad->getTexture())
              glTexCoord2f(1 - (float) i / slices,
                           1 - (float) j / stacks);

            glVertex3f(sintemp1 * sinCache1a[i],
                       sintemp1 * cosCache1a[i], zLow);
          }

          switch (quad->getNormals()) {
            case GLU_SMOOTH:
              glNormal3f(sinCache2a[i] * sintemp4,
                         cosCache2a[i] * sintemp4,
                         costemp4);
              break;
            case GLU_FLAT:
              glNormal3f(sinCache3a[i] * sintemp4,
                         cosCache3a[i] * sintemp4,
                         costemp4);
              break;
            case GLU_NONE:
            default:
              break;
          }

          if (quad->getOrientation() == GLU_OUTSIDE) {
            if (quad->getTexture())
              glTexCoord2f(1 - (float) i / slices,
                           1 - (float) j / stacks);

            glVertex3f(sintemp1 * sinCache1a[i],
                       sintemp1 * cosCache1a[i], zLow);
          }
          else {
            if (quad->getTexture())
              glTexCoord2f(1 - (float) i / slices,
                           1 - (float) (j+1) / stacks);

            glVertex3f(sintemp2 * sinCache1a[i],
                       sintemp2 * cosCache1a[i], zHigh);
          }
        }

        glEnd();
      }

      break;
    case GLU_POINT:
      glBegin(GL_POINTS);

      for (j = 0; j <= stacks; j++) {
        sintemp1 = sinCache1b[j];
        costemp1 = cosCache1b[j];

        switch (quad->getNormals()) {
          case GLU_FLAT:
          case GLU_SMOOTH:
            sintemp2 = sinCache2b[j];
            costemp2 = cosCache2b[j];
            break;
          default:
            break;
        }

        for (i = 0; i < slices; i++) {
          switch (quad->getNormals()) {
            case GLU_FLAT:
            case GLU_SMOOTH:
              glNormal3f(sinCache2a[i] * sintemp2,
                         cosCache2a[i] * sintemp2,
                         costemp2);
              break;
            case GLU_NONE:
            default:
              break;
          }

          zLow = j * radius / stacks;

          if (quad->getTexture())
            glTexCoord2f(1 - (float) i / slices,
                         1 - (float) j / stacks);

            glVertex3f(sintemp1 * sinCache1a[i],
                       sintemp1 * cosCache1a[i], costemp1);
          }
      }

      glEnd();

      break;
    case GLU_LINE:
    case GLU_SILHOUETTE:
      for (j = 1; j < stacks; j++) {
        sintemp1 = sinCache1b[j];
        costemp1 = cosCache1b[j];

        switch (quad->getNormals()) {
          case GLU_FLAT:
          case GLU_SMOOTH:
            sintemp2 = sinCache2b[j];
            costemp2 = cosCache2b[j];
            break;
          default:
            break;
        }

        glBegin(GL_LINE_STRIP);

        for (i = 0; i <= slices; i++) {
          switch (quad->getNormals()) {
            case GLU_FLAT:
              glNormal3f(sinCache3a[i] * sintemp2,
                         cosCache3a[i] * sintemp2,
                         costemp2);
              break;
            case GLU_SMOOTH:
              glNormal3f(sinCache2a[i] * sintemp2,
                         cosCache2a[i] * sintemp2,
                         costemp2);
              break;
            case GLU_NONE:
            default:
              break;
          }

          if (quad->getTexture())
            glTexCoord2f(1 - (float) i / slices,
                         1 - (float) j / stacks);

          glVertex3f(sintemp1 * sinCache1a[i],
                     sintemp1 * cosCache1a[i], costemp1);
        }

        glEnd();
      }

      for (i = 0; i < slices; i++) {
        sintemp1 = sinCache1a[i];
        costemp1 = cosCache1a[i];

        switch (quad->getNormals()) {
          case GLU_FLAT:
          case GLU_SMOOTH:
            sintemp2 = sinCache2a[i];
            costemp2 = cosCache2a[i];
            break;
          default:
            break;
        }

        glBegin(GL_LINE_STRIP);

        for (j = 0; j <= stacks; j++) {
          switch (quad->getNormals()) {
            case GLU_FLAT:
              glNormal3f(sintemp2 * sinCache3b[j],
                         costemp2 * sinCache3b[j],
                         cosCache3b[j]);
              break;
            case GLU_SMOOTH:
              glNormal3f(sintemp2 * sinCache2b[j],
                         costemp2 * sinCache2b[j],
                         cosCache2b[j]);
              break;
            case GLU_NONE:
            default:
              break;
          }

          if (quad->getTexture())
            glTexCoord2f(1 - (float) i / slices,
                         1 - (float) j / stacks);

          glVertex3f(sintemp1 * sinCache1b[j],
                     costemp1 * sinCache1b[j], cosCache1b[j]);
        }

        glEnd();
      }

      break;
    default:
      break;
  }
}

void
gluTessBeginContour(GLUtesselator *tess)
{
  tess->startContour();
}

void
gluTessBeginPolygon(GLUtesselator *tess, GLvoid *data)
{
  tess->startPolygon(data);
}

void
gluTessEndContour(GLUtesselator *tess)
{
  tess->endContour();
}

void
gluTessEndPolygon(GLUtesselator *tess)
{
  tess->endPolygon();
}

void
gluTessNormal(GLUtesselator * /*tess*/, GLdouble /*x*/, GLdouble /*y*/, GLdouble /*z*/)
{
  CGLMgrInst->unimplemented("gluTessNormal");
}

void
gluTessProperty(GLUtesselator *tess, GLenum which, GLdouble data)
{
  if      (which == GLU_TESS_WINDING_RULE)
    tess->setWindingRule(uint(data));
  else if (which == GLU_TESS_BOUNDARY_ONLY)
    tess->setBoundaryOnly(data == GL_TRUE);
  else if (which == GLU_TESS_TOLERANCE)
    tess->setTolerance(data);
}

void
gluTessVertex(GLUtesselator *tess, GLdouble *location, GLvoid *data)
{
  tess->addVertex(CPoint3D(location[0], location[1], location[2]), data);
}

void
gluTessCallback(GLUtesselator *tess, GLenum which, GLvoid (*func)())
{
  tess->setCallback(which, func);
}

GLint
gluUnProject(GLdouble winX, GLdouble winY, GLdouble winZ,
             const GLdouble *model, const GLdouble *proj, const GLint *view,
             GLdouble *objX, GLdouble *objY, GLdouble *objZ)
{
  CMatrix3DH model_matrix(model, 16);
  CMatrix3DH proj_matrix (proj , 16);

  CMatrix3DH pm = model_matrix*proj_matrix;

  CMatrix3DH ipm;

  if (! pm.invert(ipm))
    return GL_FALSE;

  double w;

  ipm.multiplyPoint(2*(winX - view[0])/view[2] - 1,
                    2*(winY - view[1])/view[3] - 1,
                    2* winZ                    - 1,
                    1,
                    objX, objY, objZ, &w);

  *objX /= w;
  *objY /= w;
  *objZ /= w;

  return GL_TRUE;
}
