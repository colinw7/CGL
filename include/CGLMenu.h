#ifndef CGL_MENU_H
#define CGL_MENU_H

#include <CXMenu.h>
#include <CMenuButton.h>
#include <map>
#include <vector>

class CGLMenuButton;

class CGLMenu : public CXMenu {
 private:
  typedef void (*MenuCallback)(int);

  typedef std::map<uint,CGLMenu*>     MenuMap;
  typedef std::vector<CGLMenuButton*> ButtonList;

  static uint    current_id_;
  static MenuMap menu_map_;

  uint         id_;
  MenuCallback callback_;
  ButtonList   buttons_;

 public:
  CGLMenu(MenuCallback callback) :
   id_(current_id_ + 1), callback_(callback) {
    menu_map_[++current_id_] = this;
  }

 ~CGLMenu() {
    menu_map_[id_] = NULL;
  }

  void exec() const;

  uint getId() const { return id_; }

  static CGLMenu *lookupMenu(uint id) {
    if (menu_map_.find(id) != menu_map_.end())
      return menu_map_[id];
    else
      return NULL;
  }

  void addButton(CGLMenuButton *button);

  uint getNumItems() const { return buttons_.size(); }
};

class CGLMenuButton : public CMenuButton {
 private:
  CGLMenu *menu_;
  int      value_;

 public:
  CGLMenuButton(CGLMenu *menu, const char *label, int value) :
   CMenuButton(menu, label), menu_(menu), value_(value) {
    menu_->addButton(this);
  }

  int getValue() const { return value_; }
};

#endif
