#ifndef CMENU_SEPARATOR_H
#define CMENU_SEPARATOR_H

#include <CMenuTypes.h>
#include <CMenuItem.h>

class CMenuSeparator : public CMenuItem {
 public:
  CMenuSeparator(CMenu *menu);
 ~CMenuSeparator();

  int getWidth () const;
  int getHeight() const;

  bool isSelectable() const { return false; }

  void draw();
};

#endif
