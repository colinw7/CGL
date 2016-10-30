#include <CMenuSeparator.h>
#include <CMenu.h>
#include <CPixelRenderer.h>

CMenuSeparator::
CMenuSeparator(CMenu *menu) :
 CMenuItem(menu)
{
}

CMenuSeparator::
~CMenuSeparator()
{
}

int
CMenuSeparator::
getWidth() const
{
  return 1;
}

int
CMenuSeparator::
getHeight() const
{
  return border_;
}

void
CMenuSeparator::
draw()
{
  CPixelRenderer *renderer = menu_->getRenderer();

  int y = y_ + height_/2;

  renderer->drawLine(CIPoint2D(0, y), CIPoint2D(width_ - 1, y));
}
