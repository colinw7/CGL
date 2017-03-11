#ifndef CGL_APP_H
#define CGL_APP_H

#include <CEvent.h>
#include <CRGBA.h>
#include <CMatrix3DH.h>
#include <CPlane3D.h>
#include <CPoint3D.h>

#define CGLAppInst CGLApp::getInstance()

#define GLERROR { \
  GLenum code = glGetError(); \
  while (code != GL_NO_ERROR) { \
    printf("%s\n",(char *) gluErrorString(code)); \
    code = glGetError(); \
  } \
}

struct CRGBAToFV {
  float fvalues[4];

  CRGBAToFV(const CRGBA &rgba) {
    fvalues[0] = rgba.getRed  ();
    fvalues[1] = rgba.getGreen();
    fvalues[2] = rgba.getBlue ();
    fvalues[3] = rgba.getAlpha();
  }
};

struct DoubleToFV {
  float fvalues[1];

  DoubleToFV(double value) {
    fvalues[0] = value;
  }
};

class CWindow;
class CGLWindow;

class CGLApp {
 protected:
  static CGLApp *instance_;

  typedef std::map<int,CGLWindow*> WindowMap;

  WindowMap    windows_;
  uint         msecs_;
  CMouseButton mouse_button_;

 public:
  static CGLApp *getInstance();

  CGLApp();

  virtual ~CGLApp();

  void init(int argc, char **argv);
  void init(int *argc, char **argv);

  CWindow *createWindow(int x, int y, uint width, uint height);

  void mainLoop();

  void timerLoop(uint msecs);

  virtual CGLWindow *newWindow(CGLWindow *parent = 0);

  void addWindow(CGLWindow *window);

  CGLWindow *lookupWindow(int id) const;

  void shadowProjection(const CPoint3D &l, const CPoint3D &e,
                        const CVector3D &n);

  bool isExtensionSupported(const char *extension) const;

  void displayErrors() const;

  void toMatrix(const double values[16], CMatrix3DH &matrix);
  void fromMatrix(const CMatrix3DH &matrix, double values[16]);

 private:
  static void timerCB(int value);

  static void reshapeCB(int w, int h);
  static void displayCB();

  static void mouseCB(int button, int state, int x, int y);
  static void motionCB(int x, int y);
  static void keyboardDownCB(unsigned char key, int x, int y);
  static void keyboardUpCB(unsigned char key, int x, int y);
  static void keyboardCB(unsigned char key, int x, int y, int state);
  static void specialKeyDownCB(int key, int x, int y);
  static void specialKeyUpCB(int key, int x, int y);
  static void specialKeyCB(int key, int x, int y, int state);
  static void statusCB(int state);
  static void enterCB(int state);
  static void idleCB();
};

#endif
