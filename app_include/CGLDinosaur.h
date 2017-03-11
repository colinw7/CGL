#ifndef CGL_DINOSAUR_H
#define CGL_DINOSAUR_H

#include <GL/gl.h>
#include <sys/types.h>

class CGLDinosaur {
 private:
  uint offset_;

 public:
  CGLDinosaur();

  uint createList(uint offset);

  void draw();

 private:
  void extrudeSolidFromPolygon(GLfloat data[][2], unsigned int dataSize,
                               GLdouble thickness, GLuint side, GLuint edge,
                               GLuint whole);
};

#endif
