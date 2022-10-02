#ifndef CGL_WINDOW_H
#define CGL_WINDOW_H

#include <CGLFuncs.h>
#include <CIPoint2D.h>
#include <CISize2D.h>
#include <map>
#include <list>
#include <sys/types.h>
#include <accessor.h>

class CGL;
class CGLWind;
class CGLWindEventAdapter;
class CGLWindTimer;
class CXLibPixelRenderer;
class CXWindow;

#define CGLWindMgrInst CGLWindMgr::getInstance()

class CGLWindMgr {
 private:
  typedef std::list<CGLWind *> WindowList;

 public:
  static CGLWindMgr *getInstance() {
    static CGLWindMgr *instance;

    if (! instance)
      instance = new CGLWindMgr;

    return instance;
  }

  void addWindow(CGLWind *window);
  void removeWindow(CGLWind *window);

  void idle();

 private:
  CGLWindMgr() { }

 private:
  WindowList windows_;
};

class CGLWind {
 public:
  CGLWind(int x, int y, int w, int h);

  CGLWind(CGLWind *parent, int x, int y, int w, int h);

 ~CGLWind();

  CGL *getGL() const { return cgl_; }

  CGLWind *getParent() const { return parent_; }

  CGLWindTimer *timer() const { return timer_; }

  ACCESSOR(Id, uint, id)

  ACCESSOR_IMPL_GET(Cursor, int, cursor)

  bool setCursor(int cursor);

  ACCESSOR(Redisplay, bool, redisplay)

  void setDisplayFunc(void (*func)(void));
  void setResizeFunc(void (*func)(int width, int height));
  void setMouseFunc(void (*func)(int button, int state, int x, int y));
  void setMotionFunc(void (*func)(int x, int y));
  void setIdleFunc(void (*func)(void));
  void setKeyboardFunc(void (*func)(unsigned char key, int x, int y));
  void setVisibilityFunc(void (*func)(int state));
  void setSpecialFunc(void (*func)(int key, int x, int y));

  void addTimerFunc(uint msecs ,void (*func)(int), int value);

  void callDisplayFunc();
  void callResizeFunc(int width, int height);
  void callMouseFunc(int button, int state, int x, int y);
  void callMotionFunc(int x, int y);
  void callIdleFunc();
  void callKeyboardFunc(unsigned char key, int x, int y);
  void callVisibilityFunc(int state);
  void callSpecialFunc(int key, int x, int y);

  void      setPosition(const CIPoint2D &point);
  CIPoint2D getPosition() const;

  void     setSize(const CISize2D &size);
  CISize2D getSize() const;

  void show();
  void hide();
  void iconize();

  void raise();
  void lower();

  void setTitle(const std::string &title);
  void setIconTitle(const std::string &title);

  void updateSize();

  void expose();

  void redraw();

  CXLibPixelRenderer *getXRenderer();

  void flushEvents();

  void idle();

  void setMenu(int button, uint menu_id);

 private:
  typedef std::map<int,uint> MenuMap;

  CGL                 *cgl_;
  CGLWind             *parent_;
  CXWindow            *window_;
  CXLibPixelRenderer  *renderer_;
  uint                 id_;
  CGLFuncs             funcs_;
  CGLWindEventAdapter *event_handler_;
  MenuMap              menu_map_;
  int                  mouse_button_;
  CGLWindTimer        *timer_;
  int                  cursor_;
  bool                 redisplay_;
};

#endif
