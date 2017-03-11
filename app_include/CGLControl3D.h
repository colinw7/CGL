#ifndef CGL_CONTROL_3D_H
#define CGL_CONTROL_3D_H

#include <CGLControl.h>
#include <CEvent.h>
#include <CPoint3D.h>
#include <GL/glut.h>

class CGLControl3D : public CGLControl {
 public:
  typedef void (*SelectionFunc)(void);
  typedef void (*PickFunc)(GLint name);

 private:
  double left_;
  double right_;
  double bottom_;
  double top_;

  double size_;
  double center_x_, center_y_;

  double znear_;
  double zfar_;

  int  mouse_x_;
  int  mouse_y_;
  bool mouse_left_;
  bool mouse_middle_;
  bool mouse_right_;

  double drag_pos_x_;
  double drag_pos_y_;
  double drag_pos_z_;

  double matrix_ [16];
  double imatrix_[16];

  /* Configurable center point for zooming and rotation */
  CPoint3D ref_point_;

  SelectionFunc selectionFunc_;
  PickFunc      pickFunc_;

  bool disable_redraw_;

 public:
  CGLControl3D();

  void setSize(double size=2.0);
  void setCenter(double x=0.0, double y=0.0);

  void setDisableRedraw(bool flag) { disable_redraw_ = flag; }

  void reshape(int w, int h);
  void display();

  void mousePress  (const CMouseEvent &event);
  void mouseMotion (const CMouseEvent &event);
  void mouseRelease(const CMouseEvent &event);

  void setSelectionFunc(SelectionFunc selectionFunc);

  void setPickFunc(PickFunc pickFunc);

 private:
  void mouse(CButtonAction action, const CMouseEvent &event);

  double vlen(double x, double y, double z);

  void pos(double *px, double *py, double *pz, int x, int y, int *viewport);

  void getMatrix();

  void pick(GLdouble x, GLdouble y, GLdouble delX, GLdouble delY);

  void setFlags();
};

#endif
