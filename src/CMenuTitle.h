#ifndef CMENU_TITLE_H
#define CMENU_TITLE_H

#include <CMenuTypes.h>
#include <CMenuItem.h>

class CMenuTitle : public CMenuItem {
 public:
  CMenuTitle(CMenu *menu, const std::string &str);
 ~CMenuTitle();

  int getWidth () const;
  int getHeight() const;

  bool isSelectable() const { return false; }

  virtual void draw();

 private:
  std::string str_;
};

#endif
