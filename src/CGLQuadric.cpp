#include <CGLQuadric.h>
#include <CGL_glu.h>

GLUquadric::
GLUquadric() :
 draw_style_(GLU_FILL), normals_(GLU_SMOOTH),
 orientation_(GLU_OUTSIDE), texture_(false)
{
}

GLUquadric::
~GLUquadric()
{
}

void
GLUquadric::
setCallback(uint type, CallBackFunc func)
{
  callbacks_[type] = func;
}
