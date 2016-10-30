#ifndef CMENU_H
#define CMENU_H

#include <CMenuTypes.h>
#include <list>

class CWindow;
class CPixelRenderer;

class CMenu {
 public:
  typedef std::list<CMenuItem *> ItemList;

 public:
  CMenu();

  virtual ~CMenu();

  const ItemList &getItems() const { return items_; }

  void addItem(CMenuItem *item);

  void removeItem(CMenuItem *item);

  virtual void show(int x = 0, int y = 0);
  virtual void hide();

  void expose();

  virtual void motion(int x, int y);

  void buttonPress  (int x, int y);
  void buttonMotion (int x, int y);
  void buttonRelease(int x, int y);

  CMenuItem *getCurrentItem() const { return current_item_; }

  CMenuItem *getItem(int x, int y) const;

  virtual CWindow *getWindow() = 0;

  void setRenderer(CPixelRenderer *renderer) {
    renderer_ = renderer;
  }

  CPixelRenderer *getRenderer() const {
    return renderer_;
  }

 protected:
  ItemList        items_;
  CMenuItem      *current_item_;
  CMenuFormatter *fmt_;
  CPixelRenderer *renderer_;
};

#endif
