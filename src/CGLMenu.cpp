#include <CGLMenu.h>

uint             CGLMenu::current_id_;
CGLMenu::MenuMap CGLMenu::menu_map_;

void
CGLMenu::
exec() const
{
  CMenuItem *item = getCurrentItem();

  CGLMenuButton *button = dynamic_cast<CGLMenuButton *>(item);

  if (item)
    (*callback_)(button->getValue());
}

void
CGLMenu::
addButton(CGLMenuButton *button)
{
  buttons_.push_back(button);
}
