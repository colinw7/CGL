#ifndef CGL_WINDOW_H
#define CGL_WINDOW_H

#include <CEvent.h>
#include <CAutoPtr.h>
#include <CWindow.h>
#include <CGLRenderer3D.h>
#include <CGLApp.h>
#include <CGLLight.h>
#include <CGLDispAttr.h>
#include <CGLControl2D.h>
#include <CGLControl3D.h>

class CGeomObject3D;
class CGLWidget;

class CGLWindowFactory : public CWindowFactory {
 public:
  CWindow *createWindow(int x, int y, uint width, uint height);
  CWindow *createWindow(CWindow *parent, int x, int y,
                        uint width, uint height);
};

class CGLWindow : public CWindow {
 public:
  typedef std::list<CGLWidget *> WidgetList;

 public:
  CGLWindow(CGLWindow *parent=0);

  virtual ~CGLWindow();

  void init(int x, int y, uint width, uint height, const std::string &title = "window");

  int getId() const { return id_; }

  void setId(int id);

  void setTitle(const std::string &title);

  CGLWindow *addWindow(int x, int y, int width, int height);

  CGLRenderer3D *getRenderer() const;

  //------

  // CWindow interface implementation
  CGLPixelRenderer *getPixelRenderer() const;

  void getPosition(int *x, int *y) const;
  void getSize(uint *w, uint *h) const;

  void getScreenSize(uint *w, uint *h) const;

  void destroy();

  void move  (int x, int y);
  void resize(uint w, uint h);

  void map();
  void unmap();

  bool isMapped();

  void iconize  ();
  void deiconize();

  void maximize  ();
  void demaximize();

  void lower();
  void raise();

  void setWindowTitle(const std::string &title);
  void setIconTitle  (const std::string &title);

  void getWindowTitle(std::string &title) const;
  void getIconTitle(std::string &title) const;

  void expose();

  void setEventAdapter(CEventAdapter *adapter);

  bool setSelectText(const std::string &text);

  bool setProperty(const std::string &name, const std::string &value);

  //------

  CEventAdapter *getEventAdapter() const { return event_adapter_; }

  void enableDispAttr();

  CGLDispAttr *getDispAttr();

  void addControl(uint dim=3, CGLControl::Orientation orientation= CGLControl::ORIENTATION_NONE);

  CGLControl *getControl();

  void addWidget(CGLWidget *widget);

  CGLLight *createLight();

  int getPickName() const { return pick_name_; }

  void reshape(int w, int h);
  void setSize(int w, int h);

  int getWidth () const { return size_.getWidth (); }
  int getHeight() const { return size_.getHeight(); }

  virtual void setup() { }

  virtual bool timerEvent() { return false; }
  virtual bool idleEvent () { return false; }

  void redisplay();

  void setCurrent();

  void initDisplay();
  void updateDisplay();

  void reshapeCB      (int w, int h);
  void displayCB      ();
  void buttonPressCB  (const CMouseEvent &event);
  void buttonReleaseCB(const CMouseEvent &event);
  void buttonMotionCB (const CMouseEvent &event);
  void keyReleaseCB   (const CKeyEvent &event);
  void keyPressCB     (const CKeyEvent &event);
  void enterCB        ();
  void leaveCB        ();
  void idleCB         ();
  void hideCB         ();
  void showCB         ();

  virtual bool exposeEvent();

  virtual void drawScene();

  virtual void initOverlay();
  virtual void termOverlay();
  virtual void drawOverlay();

  int pickScene  (const CMouseEvent &e);
  int pickOverlay(const CMouseEvent &e);

  void setShapeFromImage(CImagePtr image);

  void addObject(CGeomObject3D &object,
                 const CMatrix3D &matrix = CMatrix3D(CMATRIX_TYPE_IDENTITY));

  void drawWidgets();

 protected:
  CGLWindow               *parent_;
  int                      id_;
  CIPoint2D                pos_;
  CISize2D                 size_;
  CAutoPtr<CGLRenderer3D>  renderer_;
  CAutoPtr<CEventAdapter>  event_adapter_;
  CAutoPtr<CGLDispAttr>    disp_attr_;
  CAutoPtr<CGLControl>     control_;
  WidgetList               widgetList_;
  bool                     in_select_;
  int                      pick_name_;
};

#endif
