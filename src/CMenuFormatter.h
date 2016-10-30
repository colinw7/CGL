#ifndef CMENU_FORMATTER_H
#define CMENU_FORMATTER_H

#include <CMenuTypes.h>

class CMenuFormatter {
 public:
  CMenuFormatter(CMenu *menu);
 ~CMenuFormatter();

  void format();

  int getWidth () const { return width_ ; }
  int getHeight() const { return height_; }

 protected:
  void formatItem(CMenuItem *item);
  void positionItem(CMenuItem *item);

 protected:
  CMenu *menu_;
  int    num_rows_;
  int    x_, y_;
  int    width_, height_;
  int   *heights_;
};

#endif
