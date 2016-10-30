#ifndef CMENU_ITEM_H
#define CMENU_ITEM_H

#include <CMenuTypes.h>

class CMenuItem {
 public:
  CMenuItem(CMenu *menu);

  virtual ~CMenuItem();

  CMenu *getMenu() const { return menu_; }

  void setX(int x) { x_ = x; }
  void setY(int y) { y_ = y; }

  int getX() const { return x_; }
  int getY() const { return y_; }

  void setWidth (int width ) { width_  = width ; }
  void setHeight(int height) { height_ = height; }

  virtual int getWidth () const { return width_ ; }
  virtual int getHeight() const { return height_; }

  virtual bool isSelectable() const = 0;

  void setHighlighted(bool flag) { highlighted_ = flag; }

  bool getHighlighted() const { return highlighted_; }

  void setSelected(bool flag) { selected_ = flag; }

  bool getSelected() const { return selected_; }

  void setFont(CFontPtr font) { font_  = font ; }

  CFontPtr getFont() const { return font_ ; }

  void setBackground(const CRGB &bg) { bg_ = bg; }
  void setForeground(const CRGB &fg) { fg_ = fg; }

  const CRGB &getBackground() const { return bg_; }
  const CRGB &getForeground() const { return fg_; }

  int getBorder() const { return border_; }

  void setBorder(int border) { border_ = border; }

  virtual void enter() { }
  virtual void leave() { }

  bool isInside(int x, int y) const;

  virtual void draw() { }

  virtual void execute() { }

 protected:
  CMenu    *menu_;
  int       x_, y_;
  int       width_, height_;
  int       border_;
  bool      highlighted_;
  bool      selected_;
  CFontPtr  font_;
  CRGB      bg_;
  CRGB      fg_;
};

#endif
