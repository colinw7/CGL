#include <CGLWind.h>
#include <CGL.h>
#include <CGLMenu.h>
#include <CGL_glut.h>
#include <CTimer.h>
#include <CXWindow.h>
#include <CXMachine.h>
#include <CXLibPixelRenderer.h>

class CGLWindEventAdapter : public CXEventAdapter {
 private:
  CGLWind *window_;

 public:
  CGLWindEventAdapter(CGLWind *window) :
   window_(window) {
  }

  bool exposeEvent() override;
  bool resizeEvent() override;

  bool buttonPressEvent  (const CMouseEvent &bevent) override;
  bool buttonReleaseEvent(const CMouseEvent &bevent) override;
  bool buttonMotionEvent (const CMouseEvent &bevent) override;

  bool keyPressEvent  (const CKeyEvent &kevent) override;
  bool keyReleaseEvent(const CKeyEvent &kevent) override;

  bool visibilityEvent(bool visible) override;
};

//----------

class CGLWindTimer : public CTimer {
 private:
  typedef void (*TimerProc)(int);

  TimerProc proc_;
  int       value_;

 public:
  CGLWindTimer(uint msecs, TimerProc proc, int value) :
   CTimer(msecs), proc_(proc), value_(value) {
  }

  void timeOut() override {
    (*proc_)(value_);
  }
};

//----------

void
CGLWindMgr::
addWindow(CGLWind *window)
{
  windows_.push_back(window);
}

void
CGLWindMgr::
removeWindow(CGLWind *window)
{
  windows_.remove(window);
}

void
CGLWindMgr::
idle()
{
  WindowList::iterator p1 = windows_.begin();
  WindowList::iterator p2 = windows_.end  ();

  for ( ; p1 != p2; ++p1)
    (*p1)->idle();
}

//----------

CGLWind::
CGLWind(int x, int y, int w, int h) :
 parent_(nullptr), id_(0), mouse_button_(-1), timer_(nullptr)
{
  cgl_ = new CGL(this);

  window_ = new CXWindow(x, y, uint(w), uint(h));

  window_->selectAllEvents();
  window_->selectPointerMotion();

  renderer_ = new CXLibPixelRenderer(window_);

  event_handler_ = new CGLWindEventAdapter(this);

  window_->setEventAdapter(event_handler_);

  window_->map();

  updateSize();

  CGLWindMgrInst->addWindow(this);
}

CGLWind::
CGLWind(CGLWind *parent, int x, int y, int w, int h) :
 parent_(parent), id_(0), mouse_button_(-1), timer_(nullptr)
{
  cgl_ = new CGL(this);

  window_ = new CXWindow(parent->window_, x, y, uint(w), uint(h));

  window_->selectAllEvents();
  window_->selectPointerMotion();

  renderer_ = new CXLibPixelRenderer(window_);

  event_handler_ = new CGLWindEventAdapter(this);

  window_->setEventAdapter(event_handler_);

  window_->map();

  updateSize();

  CGLWindMgrInst->addWindow(this);
}

CGLWind::
~CGLWind()
{
  delete window_;
  delete renderer_;
  delete event_handler_;

  CGLWindMgrInst->removeWindow(this);
}

bool
CGLWind::
setCursor(int cursor)
{
  cursor_ = cursor;

  switch (cursor_) {
    case GLUT_CURSOR_RIGHT_ARROW:
      window_->setCursor(CURSOR_TYPE_ARROW);
      break;
    case GLUT_CURSOR_LEFT_ARROW:
      window_->setCursor(CURSOR_TYPE_TOP_LEFT_ARROW);
      break;
    case GLUT_CURSOR_INFO:
      window_->setCursor(CURSOR_TYPE_QUESTION_ARROW);
      break;
    case GLUT_CURSOR_DESTROY:
      window_->setCursor(CURSOR_TYPE_SKULL);
      break;
    case GLUT_CURSOR_HELP:
      window_->setCursor(CURSOR_TYPE_QUESTION_ARROW);
      break;
    case GLUT_CURSOR_CYCLE:
      window_->setCursor(CURSOR_TYPE_CYCLE);
      break;
    case GLUT_CURSOR_SPRAY:
      window_->setCursor(CURSOR_TYPE_SPRAY);
      break;
    case GLUT_CURSOR_WAIT:
      window_->setCursor(CURSOR_TYPE_WATCH);
      break;
    case GLUT_CURSOR_TEXT:
      window_->setCursor(CURSOR_TYPE_TEXT);
      break;
    case GLUT_CURSOR_CROSSHAIR:
      window_->setCursor(CURSOR_TYPE_CROSS_HAIR);
      break;
    case GLUT_CURSOR_UP_DOWN:
      window_->setCursor(CURSOR_TYPE_SB_V_DOUBLE_ARROW);
      break;
    case GLUT_CURSOR_LEFT_RIGHT:
      window_->setCursor(CURSOR_TYPE_SB_H_DOUBLE_ARROW);
      break;
    case GLUT_CURSOR_TOP_SIDE:
      window_->setCursor(CURSOR_TYPE_TOP_SIDE);
      break;
    case GLUT_CURSOR_BOTTOM_SIDE:
      window_->setCursor(CURSOR_TYPE_BOTTOM_SIDE);
      break;
    case GLUT_CURSOR_LEFT_SIDE:
      window_->setCursor(CURSOR_TYPE_LEFT_SIDE);
      break;
    case GLUT_CURSOR_RIGHT_SIDE:
      window_->setCursor(CURSOR_TYPE_RIGHT_SIDE);
      break;
    case GLUT_CURSOR_TOP_LEFT_CORNER:
      window_->setCursor(CURSOR_TYPE_TOP_LEFT_CORNER);
      break;
    case GLUT_CURSOR_TOP_RIGHT_CORNER:
      window_->setCursor(CURSOR_TYPE_TOP_RIGHT_CORNER);
      break;
    case GLUT_CURSOR_BOTTOM_RIGHT_CORNER:
      window_->setCursor(CURSOR_TYPE_BOTTOM_RIGHT_CORNER);
      break;
    case GLUT_CURSOR_BOTTOM_LEFT_CORNER:
      window_->setCursor(CURSOR_TYPE_BOTTOM_LEFT_CORNER);
      break;
    case GLUT_CURSOR_FULL_CROSSHAIR:
      window_->setCursor(CURSOR_TYPE_CROSS_HAIR);
      break;
    case GLUT_CURSOR_NONE:
      window_->unsetCursor();
      break;
    case GLUT_CURSOR_INHERIT:
      window_->unsetCursor();
      break;
    default:
      return false;
  }

  return true;
}

void
CGLWind::
setDisplayFunc(void (*func)())
{
  funcs_.expose = func;
}

void
CGLWind::
setResizeFunc(void (*func)(int width, int height))
{
  funcs_.resize = func;
}

void
CGLWind::
setMouseFunc(void (*func)(int button, int state, int x, int y))
{
  funcs_.mouse = func;
}

void
CGLWind::
setMotionFunc(void (*func)(int x, int y))
{
  funcs_.motion = func;
}

void
CGLWind::
setIdleFunc(void (*func)())
{
  funcs_.idle = func;
}

void
CGLWind::
setKeyboardFunc(void (*func)(uchar key, int x, int y))
{
  funcs_.keyboard = func;
}

void
CGLWind::
setVisibilityFunc(void (*func)(int state))
{
  funcs_.visibility = func;
}

void
CGLWind::
setSpecialFunc(void (*func)(int key, int x, int y))
{
  funcs_.special = func;
}

void
CGLWind::
addTimerFunc(uint msecs, void (*func)(int), int value)
{
  new CGLWindTimer(msecs, func, value);
}

void
CGLWind::
callDisplayFunc()
{
  uint id = CGLMgrInst->setCurrentWindowId(id_);

  if (funcs_.expose)
    (*funcs_.expose)();

  CGLMgrInst->setCurrentWindowId(id);
}

void
CGLWind::
callResizeFunc(int width, int height)
{
  uint id = CGLMgrInst->setCurrentWindowId(id_);

  if (funcs_.resize)
    (*funcs_.resize)(width, height);

  CGLMgrInst->setCurrentWindowId(id);
}

void
CGLWind::
callMouseFunc(int button, int state, int x, int y)
{
  uint id = CGLMgrInst->setCurrentWindowId(id_);

  if (menu_map_.find(button) != menu_map_.end()) {
    CGLMenu *menu = CGLMenu::lookupMenu(menu_map_[button]);

    if (! menu)
      return;

    if (state == GLUT_DOWN) {
      mouse_button_ = button;

      int rx, ry;

      window_->translateToRootCoords(x, y, &rx, &ry);

      menu->show(rx, ry);
    }
    else {
      menu->exec();

      menu->hide();

     mouse_button_ = -1;
    }
  }
  else {
    if (funcs_.mouse)
      (*funcs_.mouse)(button, state, x, y);
  }

  CGLMgrInst->setCurrentWindowId(id);
}

void
CGLWind::
callMotionFunc(int x, int y)
{
  uint id = CGLMgrInst->setCurrentWindowId(id_);

  if (menu_map_.find(mouse_button_) != menu_map_.end()) {
    CGLMenu *menu = CGLMenu::lookupMenu(menu_map_[mouse_button_]);

    if (! menu)
      return;

    int rx, ry;

    window_->translateToRootCoords(x, y, &rx, &ry);

    menu->motion(rx, ry);
  }
  else {
    if (funcs_.motion)
      (*funcs_.motion)(x, y);
  }

  CGLMgrInst->setCurrentWindowId(id);
}

void
CGLWind::
callIdleFunc()
{
  uint id = CGLMgrInst->setCurrentWindowId(id_);

  if (funcs_.idle)
    (*funcs_.idle)();

  CGLMgrInst->setCurrentWindowId(id);
}

void
CGLWind::
callKeyboardFunc(uchar key, int x, int y)
{
  uint id = CGLMgrInst->setCurrentWindowId(id_);

  if (funcs_.keyboard)
    (*funcs_.keyboard)(key, x, y);

  CGLMgrInst->setCurrentWindowId(id);
}

void
CGLWind::
callVisibilityFunc(int state)
{
  uint id = CGLMgrInst->setCurrentWindowId(id_);

  if (funcs_.visibility)
    (*funcs_.visibility)(state);

  CGLMgrInst->setCurrentWindowId(id);
}

void
CGLWind::
callSpecialFunc(int key, int x, int y)
{
  uint id = CGLMgrInst->setCurrentWindowId(id_);

  if (funcs_.special)
    (*funcs_.special)(key, x, y);

  CGLMgrInst->setCurrentWindowId(id);
}

void
CGLWind::
setPosition(const CIPoint2D &point)
{
  window_->setPosition(point.x, point.y);
}

CIPoint2D
CGLWind::
getPosition() const
{
  int x, y;

  window_->getPosition(&x, &y);

  return CIPoint2D(x, y);
}

void
CGLWind::
setSize(const CISize2D &size)
{
  window_->resize(uint(size.width), uint(size.height));
}

CISize2D
CGLWind::
getSize() const
{
  uint w, h;

  window_->getSize(&w, &h);

  return CISize2D(int(w), int(h));
}

void
CGLWind::
show()
{
  window_->map();
}

void
CGLWind::
hide()
{
  window_->unmap();
}

void
CGLWind::
iconize()
{
  window_->iconize();
}

void
CGLWind::
raise()
{
  window_->raise();
}

void
CGLWind::
lower()
{
  window_->lower();
}

void
CGLWind::
setTitle(const std::string &title)
{
  window_->setWindowTitle(title);
}

void
CGLWind::
setIconTitle(const std::string &title)
{
  window_->setIconTitle(title);
}

void
CGLWind::
updateSize()
{
  static bool inside;

  if (inside) return;

  inside = true;

  uint id = CGLMgrInst->setCurrentWindowId(id_);

  renderer_->updateSize(int(window_->getWidth()), int(window_->getHeight()));

  int w = int(renderer_->getWidth ());
  int h = int(renderer_->getHeight());

  cgl_->resize(w, h);

  cgl_->setViewport(0.0, 0.0, w - 1.0, h - 1.0);

  callResizeFunc(w, h);

  expose();

  CGLMgrInst->setCurrentWindowId(id);

  inside = false;
}

void
CGLWind::
expose()
{
  uint id = CGLMgrInst->setCurrentWindowId(id_);

  renderer_->startDoubleBuffer();

  callDisplayFunc();

  cgl_->getColorBuffer().render(renderer_);

  renderer_->endDoubleBuffer();

  CGLMgrInst->setCurrentWindowId(id);
}

void
CGLWind::
redraw()
{
  renderer_->copyDoubleBuffer();
}

CXLibPixelRenderer *
CGLWind::
getXRenderer()
{
  return renderer_;
}

void
CGLWind::
flushEvents()
{
  window_->flushEvents();

//updateSize();

  renderer_->startDoubleBuffer();

  cgl_->getColorBuffer().render(renderer_);

  renderer_->endDoubleBuffer();
}

void
CGLWind::
idle()
{
  if (redisplay_) {
    expose();

    //updateSize();

    redisplay_ = false;
  }

  uint id = CGLMgrInst->setCurrentWindowId(id_);

  callIdleFunc();

  CGLMgrInst->setCurrentWindowId(id);
}

void
CGLWind::
setMenu(int button, uint menu_id)
{
  if (button != -1)
    menu_map_[button] = menu_id;
  else {
    for (int mbutton = 0; mbutton <= 5; ++mbutton) {
      MenuMap::iterator p = menu_map_.find(mbutton);

      if (p != menu_map_.end())
        menu_map_.erase(p);
    }
  }
}

//---------

bool
CGLWindEventAdapter::
exposeEvent()
{
  window_->redraw();
  //window_->updateSize();

  return true;
}

bool
CGLWindEventAdapter::
resizeEvent()
{
  window_->updateSize();

  return true;
}

bool
CGLWindEventAdapter::
buttonPressEvent(const CMouseEvent &bevent)
{
  int x = bevent.getX();
  int y = bevent.getY();

  if      (bevent.getButton() == CBUTTON_LEFT)
    window_->callMouseFunc(GLUT_LEFT_BUTTON, GLUT_DOWN, x, y);
  else if (bevent.getButton() == CBUTTON_MIDDLE)
    window_->callMouseFunc(GLUT_MIDDLE_BUTTON, GLUT_DOWN, x, y);
  else if (bevent.getButton() == CBUTTON_RIGHT)
    window_->callMouseFunc(GLUT_RIGHT_BUTTON, GLUT_DOWN, x, y);

  return true;
}

bool
CGLWindEventAdapter::
buttonReleaseEvent(const CMouseEvent &bevent)
{
  int x = bevent.getX();
  int y = bevent.getY();

  if      (bevent.getButton() == CBUTTON_LEFT)
    window_->callMouseFunc(GLUT_LEFT_BUTTON, GLUT_UP, x, y);
  else if (bevent.getButton() == CBUTTON_MIDDLE)
    window_->callMouseFunc(GLUT_MIDDLE_BUTTON, GLUT_UP, x, y);
  else if (bevent.getButton() == CBUTTON_RIGHT)
    window_->callMouseFunc(GLUT_RIGHT_BUTTON, GLUT_UP, x, y);

  return true;
}

bool
CGLWindEventAdapter::
buttonMotionEvent(const CMouseEvent &bevent)
{
  int x = bevent.getX();
  int y = bevent.getY();

  window_->callMotionFunc(x, y);

  return true;
}

bool
CGLWindEventAdapter::
keyPressEvent(const CKeyEvent &)
{
  return true;
}

bool
CGLWindEventAdapter::
keyReleaseEvent(const CKeyEvent &kevent)
{
  CKeyType type = kevent.getType();
  int      x    = kevent.getX();
  int      y    = kevent.getY();

  const std::string &key_str = kevent.getText();

  int kval = key_str[0];

  if      (type == CKEY_TYPE_Escape)
    window_->callKeyboardFunc(27, x, y);
  else if (type == CKEY_TYPE_F1)
    window_->callSpecialFunc(GLUT_KEY_F1, x, y);
  else if (type == CKEY_TYPE_F2)
    window_->callSpecialFunc(GLUT_KEY_F2, x, y);
  else if (type == CKEY_TYPE_F3)
    window_->callSpecialFunc(GLUT_KEY_F3, x, y);
  else if (type == CKEY_TYPE_F4)
    window_->callSpecialFunc(GLUT_KEY_F4, x, y);
  else if (type == CKEY_TYPE_F5)
    window_->callSpecialFunc(GLUT_KEY_F5, x, y);
  else if (type == CKEY_TYPE_F6)
    window_->callSpecialFunc(GLUT_KEY_F6, x, y);
  else if (type == CKEY_TYPE_F7)
    window_->callSpecialFunc(GLUT_KEY_F7, x, y);
  else if (type == CKEY_TYPE_F8)
    window_->callSpecialFunc(GLUT_KEY_F8, x, y);
  else if (type == CKEY_TYPE_F9)
    window_->callSpecialFunc(GLUT_KEY_F9, x, y);
  else if (type == CKEY_TYPE_F10)
    window_->callSpecialFunc(GLUT_KEY_F10, x, y);
  else if (type == CKEY_TYPE_F11)
    window_->callSpecialFunc(GLUT_KEY_F11, x, y);
  else if (type == CKEY_TYPE_F12)
    window_->callSpecialFunc(GLUT_KEY_F12, x, y);
  else if (type == CKEY_TYPE_Left)
    window_->callSpecialFunc(GLUT_KEY_LEFT, x, y);
  else if (type == CKEY_TYPE_Up)
    window_->callSpecialFunc(GLUT_KEY_UP, x, y);
  else if (type == CKEY_TYPE_Right)
    window_->callSpecialFunc(GLUT_KEY_RIGHT, x, y);
  else if (type == CKEY_TYPE_Down)
    window_->callSpecialFunc(GLUT_KEY_DOWN, x, y);
  else if (type == CKEY_TYPE_Page_Up)
    window_->callSpecialFunc(GLUT_KEY_PAGE_UP, x, y);
  else if (type == CKEY_TYPE_Page_Down)
    window_->callSpecialFunc(GLUT_KEY_PAGE_DOWN, x, y);
  else if (type == CKEY_TYPE_Home)
    window_->callSpecialFunc(GLUT_KEY_HOME, x, y);
  else if (type == CKEY_TYPE_End)
    window_->callSpecialFunc(GLUT_KEY_END, x, y);
  else if (type == CKEY_TYPE_Insert)
    window_->callSpecialFunc(GLUT_KEY_INSERT, x, y);
  else
    window_->callKeyboardFunc(uchar(kval), x, y);

  return true;
}

bool
CGLWindEventAdapter::
visibilityEvent(bool visible)
{
  window_->callVisibilityFunc(visible ? GLUT_VISIBLE : GLUT_NOT_VISIBLE);

  return true;
}
