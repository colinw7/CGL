#ifndef CGL_CONTROL_H
#define CGL_CONTROL_H

#include <CEvent.h>
#include <GL/glut.h>

class CGLControl {
 public:
  enum Orientation {
    ORIENTATION_NONE,
    ORIENTATION_XY,
    ORIENTATION_XZ,
    ORIENTATION_YZ
  };

 protected:
  bool        enabled_;
  Orientation orientation_;

 public:
  CGLControl(Orientation orientation=ORIENTATION_NONE) :
   enabled_(true), orientation_(orientation) {
  }

  virtual ~CGLControl() { }

  void setEnabled(bool flag) { enabled_ = flag; }

  bool getEnabled() const { return enabled_; }

  virtual void reshape(int w, int h) = 0;
  virtual void display() = 0;

  virtual void mousePress  (const CMouseEvent &event) = 0;
  virtual void mouseMotion (const CMouseEvent &event) = 0;
  virtual void mouseRelease(const CMouseEvent &event) = 0;
};

#endif
