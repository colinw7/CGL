#include <CGL_glx.h>

Display *
glXGetCurrentDisplay()
{
  return CXMachineInst->getDisplay();
}

GLXDrawable
glXGetCurrentDrawable()
{
  return 0;
}
