#include <CGLControl2D.h>
#include <GL/glut.h>
#include <cmath>

CGLControl2D::
CGLControl2D(Orientation orientation) :
 CGLControl(orientation)
{
  left_   = -1.0;
  right_  =  1.0;
  bottom_ = -1.0;
  top_    =  1.0;
  znear_  =  1.5;
  zfar_   = 20.0;

  scale_ = 1.0;
  angle_ = 0.0;

  dx_ = 0.0;
  dy_ = 0.0;
}

void
CGLControl2D::
reshape(int w, int h)
{
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();

  double aspect = double(w)/double(h);

  //glFrustum(left_*aspect, right_*aspect, bottom_, top_, znear_, zfar_);
  glOrtho(left_*aspect, right_*aspect, bottom_, top_, znear_, zfar_);

  glMatrixMode(GL_MODELVIEW);
}

void
CGLControl2D::
display()
{
  glLoadIdentity();

  if      (orientation_ == ORIENTATION_XY)
    gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  else if (orientation_ == ORIENTATION_XZ)
    gluLookAt(0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
  else
    gluLookAt(2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

  glScalef(scale_, scale_, scale_);

  if      (orientation_ == ORIENTATION_XY)
    glRotatef(angle_, 0, 0, 1);
  else if (orientation_ == ORIENTATION_XZ)
    glRotatef(angle_, 0, 1, 0);
  else
    glRotatef(angle_, 1, 0, 0);

  if      (orientation_ == ORIENTATION_XY)
    glTranslatef(dx_, dy_, 0);
  else if (orientation_ == ORIENTATION_XZ)
    glTranslatef(dy_, 0, dx_);
  else
    glTranslatef(0, dx_, dy_);
}

void
CGLControl2D::
mousePress(const CMouseEvent &event)
{
  mouse_x_ = event.getX();
  mouse_y_ = event.getY();

  pixelToWindow(mouse_x_, mouse_y_, &window_x_, &window_y_);

  switch (event.getButton()) {
   case CBUTTON_LEFT  : mouse_left_   = true ; break;
   case CBUTTON_MIDDLE: mouse_middle_ = true ; break;
   case CBUTTON_RIGHT : mouse_right_  = true ; break;
   default            :                        break;
 }
}

void
CGLControl2D::
mouseRelease(const CMouseEvent &event)
{
  switch (event.getButton()) {
   case CBUTTON_LEFT  : mouse_left_   = false; break;
   case CBUTTON_MIDDLE: mouse_middle_ = false; break;
   case CBUTTON_RIGHT : mouse_right_  = false; break;
   default            :                        break;
 }
}

void
CGLControl2D::
mouseMotion(const CMouseEvent &event)
{
  int mouse_x = event.getX();
  int mouse_y = event.getY();

  int dx = mouse_x - mouse_x_;
  int dy = mouse_y - mouse_y_;

  if (dx == 0 && dy == 0)
    return;

  double window_x, window_y;

  pixelToWindow(mouse_x, mouse_y, &window_x, &window_y);

  bool changed = false;

  // zoom
  if      (mouse_left_) {
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);

    double dangle = dx/(double)(viewport[2] + 1)*180.0;

    angle_ += dangle;

    changed = true;
  }
  else if (mouse_middle_) {
    double s = exp(double(dy)*0.01);

    scale_ *= s;

    changed = true;
  }
  // translate
  else if (mouse_right_) {
    dx_ += (window_x - window_x_);
    dy_ += (window_y - window_y_);

    changed = true;
  }

  mouse_x_ = mouse_x;
  mouse_y_ = mouse_y;

  window_x_ = window_x;
  window_y_ = window_y;

  if (changed) {
    glutPostRedisplay();

    glFlush();
  }
}

void
CGLControl2D::
pixelToWindow(int x, int y, double *wx, double *wy)
{
  /*
    Use the ortho projection and viewport information
    to map from mouse co-ordinates back into world
    co-ordinates
  */

  GLint viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);

  *wx = double(x - viewport[0])/double(viewport[2]);
  *wy = double(y - viewport[1])/double(viewport[3]);

  *wx = left_ + (*wx)*(right_  - left_);
  *wy = top_  + (*wy)*(bottom_ - top_ );
}
