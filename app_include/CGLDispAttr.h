#ifndef CGL_DISP_ATTR_H
#define CGL_DISP_ATTR_H

#include <CEvent.h>
#include <GL/glut.h>

class CGLDispAttr {
 private:
  bool enabled_;
  bool depth_test_;
  bool cull_face_;
  bool lighting_;
  bool outline_;
  bool front_face_;
  bool smooth_shade_;
  bool disable_redraw_;

 public:
  CGLDispAttr();

  void setEnabled(bool enabled);

  void keyPress(const CKeyEvent &event);

 private:
  void setFlags();
};

#endif
