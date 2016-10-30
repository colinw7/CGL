#ifndef CMENU_BUTTON_H
#define CMENU_BUTTON_H

#include <CMenuTypes.h>
#include <CMenuItem.h>
#include <string>

class CMenuButton : public CMenuItem {
 public:
  CMenuButton(CMenu *menu, const std::string &str);

  virtual ~CMenuButton();

  int getWidth () const;
  int getHeight() const;

  bool isSelectable() const { return true; }

  virtual void execute() { }

  void draw();

 private:
  std::string str_;
};

#endif
