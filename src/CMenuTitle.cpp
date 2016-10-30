#include <CMenuTitle.h>
#include <CMenu.h>
#include <CPixelRenderer.h>

CMenuTitle::
CMenuTitle(CMenu *menu, const std::string &str) :
 CMenuItem(menu), str_(str)
{
}

CMenuTitle::
~CMenuTitle()
{
}

int
CMenuTitle::
getWidth() const
{
  CPixelRenderer *renderer = menu_->getRenderer();

  int text_width = renderer->getStringWidth(str_);

  return text_width + 2*border_;
}

int
CMenuTitle::
getHeight() const
{
  CPixelRenderer *renderer = menu_->getRenderer();

  int text_height = renderer->getCharHeight();

  return text_height + 2*border_;
}

void
CMenuTitle::
draw()
{
  CPixelRenderer *renderer = menu_->getRenderer();

  int text_width  = renderer->getStringWidth(str_);
  int text_height = renderer->getCharHeight();

  int xo = (width_  - text_width )/2;
  int yo = (height_ - text_height)/2;

  renderer->drawString(CIPoint2D(x_ + xo, y_ + yo), str_);
}
