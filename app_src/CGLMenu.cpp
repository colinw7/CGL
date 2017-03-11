#include <CGLMenu.h>
#include <CGLWindow.h>
#include <GL/glut.h>

CGLMenu::
CGLMenu(CGLWindow *window) :
 window_(window)
{
  id_ = glutCreateMenu(menuCB);

  CGLMenuMgrInst->addMenu(id_, this);

  item_id_ = 0;
}

void
CGLMenu::
attach()
{
  window_->setCurrent();

  glutSetMenu(id_);

  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void
CGLMenu::
addMenuItem(CGLMenuItem *item)
{
  glutSetMenu(id_);

  item->setId(item_id_);

  glutAddMenuEntry(item->getText().c_str(), item->getId());

  menu_item_map_[item_id_] = item;

  ++item_id_;
}

void
CGLMenu::
addSubMenu(CGLMenu *menu, const std::string &text)
{
  glutSetMenu(id_);

  glutAddSubMenu(text.c_str(), menu->getId());
}

void
CGLMenu::
menuCB(int item_id)
{
  int id = glutGetMenu();

  CGLMenu *menu = CGLMenuMgrInst->getMenu(id);

  if (menu == NULL)
    return;

  CGLMenuItem *item = menu->getMenuItem(item_id);

  if (item == NULL)
    return;

  if (! item->exec())
    menu->exec(item);
}
