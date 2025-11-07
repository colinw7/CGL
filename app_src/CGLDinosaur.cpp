#include <CGLDinosaur.h>
#include <GL/glut.h>
#include <cmath>

static GLfloat
body[][2] = {
 { 0,  3  },
 { 1,  1  },
 { 5,  1  },
 { 8,  4  },
 {10,  4  },
 {11,  5  },
 {11, 11.5},
 {13, 12  },
 {13, 13  },
 {10, 13.5},
 {13, 14  },
 {13, 15  },
 {11, 16  },
 { 8, 16  },
 { 7, 15  },
 { 7, 13  },
 { 8, 12  },
 { 7, 11  },
 { 6,  6  },
 { 4,  3  },
 { 3,  2  },
 { 1,  2  },
};

static GLfloat
arm[][2] = {
 {  8  , 10  },
 {  9  ,  9  },
 { 10  ,  9  },
 { 13  ,  8  },
 { 14  ,  9  },
 { 16  ,  9  },
 { 15  ,  9.5},
 { 16  , 10  },
 { 15  , 10  },
 { 15.5, 11  },
 { 14.5, 10  },
 { 14  , 11  },
 { 14  , 10  },
 { 13  ,  9  },
 { 11  , 11  },
 {  9  , 11  },
};

static GLfloat
leg[][2] = {
 {  8,  6   },
 {  8,  4   },
 {  9,  3   },
 {  9,  2   },
 {  8,  1   },
 {  8,  0.5 },
 {  9,  0   },
 { 12,  0   },
 { 10,  1   },
 { 10,  2   },
 { 12,  4   },
 { 11,  6   },
 { 10,  7   },
 {  9,  7   },
};

static GLfloat
eye[][2] = {
  {  8.75f, 15.00f },
  {  9.00f, 14.70f },
  {  9.60f, 14.70f },
  { 10.10f, 15.00f },
  {  9.60f, 15.25f },
  {  9.00f, 15.25f },
};

enum {
  BODY_SIDE,
  BODY_EDGE,
  BODY_WHOLE,
  ARM_SIDE,
  ARM_EDGE,
  ARM_WHOLE,
  LEG_SIDE,
  LEG_EDGE,
  LEG_WHOLE,
  EYE_SIDE,
  EYE_EDGE,
  EYE_WHOLE,
  DINOSAUR
};

GLfloat skinColor[] = { 0.1f, 1.0f, 0.1f, 1.0f };
GLfloat eyeColor [] = { 1.0f, 0.2f, 0.2f, 1.0f };

CGLDinosaur::
CGLDinosaur() :
 offset_(0)
{
}

uint
CGLDinosaur::
createList(uint offset)
{
  offset_ = offset;

  GLfloat bodyWidth = 3.0;

  extrudeSolidFromPolygon(body, sizeof(body), bodyWidth,
                          offset + BODY_SIDE,
                          offset + BODY_EDGE,
                          offset + BODY_WHOLE);
  extrudeSolidFromPolygon(arm, sizeof(arm), bodyWidth / 4.0,
                          offset + ARM_SIDE,
                          offset + ARM_EDGE,
                          offset + ARM_WHOLE);
  extrudeSolidFromPolygon(leg, sizeof(leg), bodyWidth / 2.0,
                          offset + LEG_SIDE,
                          offset + LEG_EDGE,
                          offset + LEG_WHOLE);
  extrudeSolidFromPolygon(eye, sizeof(eye), bodyWidth + 0.2,
                          offset + EYE_SIDE,
                          offset + EYE_EDGE,
                          offset + EYE_WHOLE);

  glNewList(offset + DINOSAUR, GL_COMPILE);

    glMaterialfv(offset + GL_FRONT, GL_DIFFUSE, skinColor);

    glCallList(offset + BODY_WHOLE);

    glPushMatrix();

      glTranslatef(0.0, 0.0, bodyWidth);

      glCallList(offset + ARM_WHOLE);
      glCallList(offset + LEG_WHOLE);

      glTranslatef(0.0, 0.0, float(-bodyWidth - bodyWidth/4.0));

      glCallList(offset + ARM_WHOLE);

      glTranslatef(0.0, 0.0, float(-bodyWidth/4.0));

      glCallList(offset + LEG_WHOLE);

      glTranslatef(0.0, 0.0, float(bodyWidth/2.0 - 0.1));

      glMaterialfv(GL_FRONT, GL_DIFFUSE, eyeColor);

      glCallList(offset + EYE_WHOLE);

    glPopMatrix();

  glEndList();

  return offset + DINOSAUR;
}

void
CGLDinosaur::
extrudeSolidFromPolygon(GLfloat data[][2], uint dataSize,
                        GLdouble thickness, GLuint side, GLuint edge,
                        GLuint whole)
{
  static GLUtriangulatorObj *tobj = NULL;

  uint count = dataSize/(2*sizeof(GLfloat));

  if (tobj == NULL) {
    tobj = gluNewTess();

    using Callback = void (*)();

    gluTessCallback(tobj, GLU_BEGIN , reinterpret_cast<Callback>(glBegin));
    gluTessCallback(tobj, GLU_VERTEX, reinterpret_cast<Callback>(glVertex2fv));
    gluTessCallback(tobj, GLU_END   , reinterpret_cast<Callback>(glEnd));
  }

  glNewList(side, GL_COMPILE);
    GLdouble vertex[3];

    glShadeModel(GL_SMOOTH);

    gluBeginPolygon(tobj);

    for (uint i = 0; i < count; ++i) {
      vertex[0] = data[i][0];
      vertex[1] = data[i][1];
      vertex[2] = 0;

      gluTessVertex(tobj, vertex, &data[i]);
    }

    gluEndPolygon(tobj);
  glEndList();

  glNewList(edge, GL_COMPILE);
    glShadeModel(GL_FLAT);

    glBegin(GL_QUAD_STRIP);

    for (uint i = 0; i <= count; ++i) {
      glVertex3f(data[i % count][0], data[i % count][1], 0.0);
      glVertex3f(data[i % count][0], data[i % count][1], float(thickness));

      double dx = data[(i + 1) % count][1] - data[i % count][1];
      double dy = data[i % count][0] - data[(i + 1) % count][0];

      double len = sqrt(dx * dx + dy * dy);

      glNormal3f(float(dx/len), float(dy/len), 0.0);
    }

    glEnd();
  glEndList();

  glNewList(whole, GL_COMPILE);
    glFrontFace(GL_CW);
    glCallList(edge);
    glNormal3f(0.0, 0.0, -1.0);
    glCallList(side);
    glPushMatrix();
      glTranslatef(0.0, 0.0, float(thickness));
      glFrontFace(GL_CCW);
      glNormal3f(0.0, 0.0, 1.0);
      glCallList(side);
    glPopMatrix();
  glEndList();
}

void
CGLDinosaur::
draw()
{
  glCallList(offset_ + DINOSAUR);
}
