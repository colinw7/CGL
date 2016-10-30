#include <CMenuItem.h>
#include <CMenu.h>
#include <CFuncs.h>

CMenuItem::
CMenuItem(CMenu *menu) :
 menu_(menu), x_(0), y_(0), width_(0), height_(0),
 border_(2), highlighted_(false)
{
  menu_->addItem(this);
}

CMenuItem::
~CMenuItem()
{
}

bool
CMenuItem::
isInside(int x, int y) const
{
  if (x >= x_ && x <= x_ + width_ &&
      y >= y_ && y <= y_ + height_)
    return true;

  return false;
}
