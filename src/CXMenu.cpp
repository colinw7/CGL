#include <CXLib.h>
#include <CXLibPixelRenderer.h>
#include <CXMenu.h>

class CXMenuEventAdapter : public CXEventAdapter {
 private:
  static uint event_mask_;

  CXMenu *menu_;

  public:
   CXMenuEventAdapter(CXMenu *menu) :
    CXEventAdapter(0), menu_(menu) {
   }

   bool exposeEvent() { menu_->expose(); return true; }
};

CXMenu::
CXMenu()
{
  grab_ = false;

  xwindow_ = new CXWindow(CWINDOW_TYPE_OVERRIDE);

  renderer_ = new CXLibPixelRenderer(xwindow_);

  xwindow_->setEventAdapter(new CXMenuEventAdapter(this));

  setRenderer(renderer_);
}

CXMenu::
~CXMenu()
{
  delete xwindow_;
  delete renderer_;
}

void
CXMenu::
show(int x, int y)
{
  CMenu::show(x, y);

  xwindow_->flushEvents();

  if (grab_)
    xwindow_->grabPointer();
}

void
CXMenu::
hide()
{
  xwindow_->flushEvents();

  if (grab_)
    xwindow_->ungrabPointer();

  CMenu::hide();
}

void
CXMenu::
motion(int x, int y)
{
  int wx, wy;

  xwindow_->translateFromRootCoords(x, y, &wx, &wy);

  CMenu::motion(wx, wy);
}

CXWindow *
CXMenu::
getXWindow()
{
  return xwindow_;
}

CWindow *
CXMenu::
getWindow()
{
  return xwindow_;
}
