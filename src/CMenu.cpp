#include <CMenu.h>
#include <CMenuFormatter.h>
#include <CMenuTitle.h>
#include <CMenuButton.h>
#include <CMenuSeparator.h>
#include <CGUIPixelRenderer.h>
#include <CPixelRenderer.h>
#include <CFuncs.h>
#include <CWindow.h>

CMenu::
CMenu() :
 current_item_(0)
{
  fmt_ = new CMenuFormatter(this);
}

CMenu::
~CMenu()
{
  for_each(items_.begin(), items_.end(), CDeletePointer());

  delete fmt_;
}

void
CMenu::
addItem(CMenuItem *item)
{
  items_.push_back(item);
}

void
CMenu::
removeItem(CMenuItem *item)
{
  items_.remove(item);
}

void
CMenu::
show(int x, int y)
{
  CWindow *window = CWindowMgrInst->createWindow(x, y, 1000, 1000);

//renderer_->setBackground(CRGB(0.2,0.2,0.7));

  fmt_->format();

  window->resize(fmt_->getWidth(), fmt_->getHeight());

  window->move(x, y);

  window->map();

  expose();
}

void
CMenu::
hide()
{
  CWindow *window = getWindow();

  window->destroy();
}

void
CMenu::
expose()
{
  renderer_->clear();

  int width  = renderer_->getWidth ();
  int height = renderer_->getHeight();

//renderer_->setBackground(CRGB(0.2,0.2,0.7));

  CGUIPixelRenderer gui_renderer(renderer_);

  gui_renderer.drawButton(0, 0, width, height, 2, false);

  for (auto &item : items_)
    item->draw();
}

void
CMenu::
buttonPress(int, int)
{
  if (current_item_ != 0) {
    current_item_->setSelected(true);

    expose();
  }
}

void
CMenu::
buttonMotion(int x, int y)
{
  motion(x, y);
}

void
CMenu::
buttonRelease(int, int)
{
  if (current_item_ != 0) {
    current_item_->setSelected(false);

    expose();

    current_item_->execute();
  }
}

void
CMenu::
motion(int x, int y)
{
  CMenuItem *item = getItem(x, y);

  if (item == current_item_)
    return;

  if (item != 0 && ! item->isSelectable())
    item = 0;

  if (item != current_item_) {
    if (current_item_ != 0) {
      current_item_->setHighlighted(false);
      current_item_->setSelected   (false);

      current_item_->draw();
    }

    current_item_ = item;

    if (current_item_ != 0) {
      current_item_->setHighlighted(true);
      current_item_->setSelected   (false);

      current_item_->draw();
    }
  }
}

CMenuItem *
CMenu::
getItem(int x, int y) const
{
  ItemList::const_iterator pitem1 = items_.begin();
  ItemList::const_iterator pitem2 = items_.end  ();

  for ( ; pitem1 != pitem2; ++pitem1) {
    if ((*pitem1)->isInside(x, y))
      return *pitem1;
  }

  return 0;
}
