#ifndef CXMENU_H
#define CXMENU_H

#include <CMenu.h>

class CXWindow;
class CXLibPixelRenderer;

class CXMenu : public CMenu {
 private:
  CXWindow           *xwindow_;
  CXLibPixelRenderer *renderer_;
  bool                grab_;

 public:
  CXMenu();

  virtual ~CXMenu();

  void show(int x=0, int y=0);
  void hide();

  void motion(int x, int y);

  CXWindow *getXWindow();

  CWindow *getWindow();
};

#endif
