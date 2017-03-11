#ifndef CGL_CONTROL_2D_H
#define CGL_CONTROL_2D_H

#include <CGLControl.h>
#include <CEvent.h>
#include <GL/glut.h>

class CGLControl2D : public CGLControl {
 private:
  double left_;
  double right_;
  double bottom_;
  double top_;

  double znear_;
  double zfar_;

  double scale_;
  double angle_;
  double dx_, dy_;

  bool mouse_left_;
  bool mouse_middle_;
  bool mouse_right_;

  int    mouse_x_;
  int    mouse_y_;
  double window_x_;
  double window_y_;

 public:
  CGLControl2D(Orientation orientation);

 private:
  void reshape(int w, int h);
  void display();

  void mousePress  (const CMouseEvent &event);
  void mouseMotion (const CMouseEvent &event);
  void mouseRelease(const CMouseEvent &event);

  void pixelToWindow(int px, int py, double *wx, double *wy);
};

#endif
