#include <CGLNurbs.h>
#include <CGL_glu.h>

GLUnurbs::
GLUnurbs()
{
}

GLUnurbs::
~GLUnurbs()
{
}

void
GLUnurbs::
setCallback(uint type, CallBackFunc func)
{
  callbacks_[type] = func;
}
