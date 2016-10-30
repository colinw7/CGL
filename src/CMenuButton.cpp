#include <CMenu.h>
#include <CMenuButton.h>
#include <CGUIPixelRenderer.h>
#include <CPixelRenderer.h>

CMenuButton::
CMenuButton(CMenu *menu, const std::string &str) :
 CMenuItem(menu), str_(str)
{
}

CMenuButton::
~CMenuButton()
{
}

int
CMenuButton::
getWidth() const
{
  CPixelRenderer *renderer = menu_->getRenderer();

  int text_width = renderer->getStringWidth(str_);

  return text_width + 4*border_;
}

int
CMenuButton::
getHeight() const
{
  CPixelRenderer *renderer = menu_->getRenderer();

  int text_height = renderer->getCharHeight();

  return text_height + 4*border_;
}

void
CMenuButton::
draw()
{
  CPixelRenderer *renderer = menu_->getRenderer();

  CGUIPixelRenderer gui_renderer(renderer);

  gui_renderer.drawButton(x_, y_, width_, height_, border_, highlighted_);

  int text_width  = renderer->getStringWidth(str_);
  int text_height = renderer->getCharHeight();

  int xo = (width_  - text_width )/2;
  int yo = (height_ - text_height)/2;

  renderer->drawString(CIPoint2D(x_ + xo, y_ + yo), str_);
}
