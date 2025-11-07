#ifndef CGL_MENU_H
#define CGL_MENU_H

#include <string>
#include <map>
#include <sys/types.h>

class CGLWindow;
class CGLMenuItem;

class CGLMenu {
 public:
  CGLMenu(CGLWindow *window);

  virtual ~CGLMenu() { }

  CGLWindow *getWindow() const { return window_; }

  uint getId() const { return id_; }

  void attach();

  void addMenuItem(CGLMenuItem *item);

  void addSubMenu(CGLMenu *menu, const std::string &text);

  CGLMenuItem *getMenuItem(uint id) const {
    MenuItemMap::const_iterator p = menu_item_map_.find(id);

    if (p == menu_item_map_.end())
      return NULL;

    return (*p).second;
  }

  virtual void exec(CGLMenuItem *) { }

 protected:
  static void menuCB(int item_id);

 protected:
  typedef std::map<uint,CGLMenuItem *> MenuItemMap;

  CGLWindow   *window_;
  uint         id_;
  uint         item_id_;
  MenuItemMap  menu_item_map_;
};

//---

class CGLMenuItem {
 public:
  CGLMenuItem(const std::string &text) :
   id_(0), text_(text) {
  }

  virtual ~CGLMenuItem() { }

  const std::string &getText() const { return text_; }

  uint getId() const { return id_; }

  void setId(uint id) { id_ = id; }

  virtual bool exec() { return false; }

 protected:
  uint        id_;
  std::string text_;
};

//---

#define CGLMenuMgrInst CGLMenuMgr::getInstance()

class CGLMenuMgr {
 public:
  static CGLMenuMgr *getInstance() {
    static CGLMenuMgr *instance;

    if (! instance)
      instance = new CGLMenuMgr;

    return instance;
  }

  void addMenu(uint id, CGLMenu *menu) {
    menu_map_[id] = menu;
  }

  CGLMenu *getMenu(uint id) const {
    MenuMap::const_iterator p = menu_map_.find(id);

    if (p == menu_map_.end())
      return NULL;

    return (*p).second;
  }

 protected:
  CGLMenuMgr() { }
 ~CGLMenuMgr() { }

 private:
  // not to be implemented
  CGLMenuMgr(const CGLMenuMgr &mgr);
  CGLMenuMgr &operator=(const CGLMenuMgr &mgr);

 protected:
  typedef std::map<uint,CGLMenu *> MenuMap;

  MenuMap menu_map_;
};

#endif
