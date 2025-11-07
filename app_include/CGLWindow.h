#ifndef CGL_WINDOW_H
#define CGL_WINDOW_H

#include <CGLRenderer3D.h>
#include <CGLApp.h>
#include <CGLLight.h>
#include <CGLDispAttr.h>
#include <CGLControl2D.h>
#include <CGLControl3D.h>
#include <CWindow.h>
#include <CEvent.h>
#include <memory>

class CGeomObject3D;
class CGLWidget;

class CGLWindowFactory : public CWindowFactory {
 public:
  CWindow *createWindow(int x, int y, uint width, uint height) override;
  CWindow *createWindow(CWindow *parent, int x, int y,
                        uint width, uint height) override;
};

class CGLWindow : public CWindow {
 public:
  typedef std::list<CGLWidget *> WidgetList;

 public:
  CGLWindow(CGLWindow *parent=nullptr);

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

  void getPosition(int *x, int *y) const override;
  void getSize(uint *w, uint *h) const override;

  void getScreenSize(uint *w, uint *h) const override;

  void destroy() override;

  void move  (int x, int y) override;
  void resize(uint w, uint h) override;

  void map() override;
  void unmap() override;

  bool isMapped() override;

  void iconize  () override;
  void deiconize() override;

  void maximize  () override;
  void demaximize() override;

  void lower() override;
  void raise() override;

  void setWindowTitle(const std::string &title) override;
  void setIconTitle  (const std::string &title) override;

  void getWindowTitle(std::string &title) const override;
  void getIconTitle(std::string &title) const override;

  void expose() override;

  void setEventAdapter(CEventAdapter *adapter) override;

  bool setSelectText(const std::string &text) override;

  bool setProperty(const std::string &name, const std::string &value) override;

  //------

  CEventAdapter *getEventAdapter() const { return event_adapter_.get(); }

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

  bool exposeEvent() override;

  virtual void drawScene();

  virtual void initOverlay();
  virtual void termOverlay();
  virtual void drawOverlay();

  int pickScene  (const CMouseEvent &e);
  int pickOverlay(const CMouseEvent &e);

  void setShapeFromImage(CImagePtr image);

  void addObject(CGeomObject3D &object,
                 const CMatrix3D &matrix = CMatrix3D(CMatrix3D::Type::IDENTITY));

  void drawWidgets();

 protected:
  using RendererP     = std::unique_ptr<CGLRenderer3D>;
  using EventAdapterP = std::unique_ptr<CEventAdapter>;
  using DispAttrP     = std::unique_ptr<CGLDispAttr>;
  using ControlP      = std::unique_ptr<CGLControl>;

  CGLWindow*    parent_     { nullptr };
  int           id_         { 0 };
  CIPoint2D     pos_        { 0, 0 };
  CISize2D      size_       { 100, 100 };
  RendererP     renderer_;
  EventAdapterP event_adapter_;
  DispAttrP     disp_attr_;
  ControlP      control_;
  WidgetList    widgetList_;
  bool          in_select_  { false };
  int           pick_name_  { 0 };
};

#endif
