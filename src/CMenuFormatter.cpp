#include <CMenuFormatter.h>
#include <CMenu.h>
#include <CMenuItem.h>

CMenuFormatter::
CMenuFormatter(CMenu *menu) :
 menu_(menu)
{
}

CMenuFormatter::
~CMenuFormatter()
{
}

void
CMenuFormatter::
format()
{
  const CMenu::ItemList &items = menu_->getItems();

  int num_items = items.size();

  heights_ = new int [num_items];

  for (int i = 0; i < num_items; ++i)
    heights_[i] = 0;

  x_      = 0;
  y_      = 0;
  width_  = 0;
  height_ = 0;

  num_rows_ = 0;

  for (auto &item : items)
    CMenuFormatter::formatItem(item);

  for (int i = 0; i < num_items; ++i)
    height_ += heights_[i];

  num_rows_ = 0;

  for (auto &item : items)
    CMenuFormatter::positionItem(item);

  delete [] heights_;
}

void
CMenuFormatter::
formatItem(CMenuItem *item)
{
  int width  = item->getWidth ();
  int height = item->getHeight();

  width_ = std::max(width_, width );

  heights_[num_rows_] = std::max(heights_[num_rows_], height);

  ++num_rows_;
}

void
CMenuFormatter::
positionItem(CMenuItem *item)
{
  item->setX     (0);
  item->setY     (y_);
  item->setWidth (width_);
  item->setHeight(heights_[num_rows_]);

  y_ += heights_[num_rows_];

  ++num_rows_;
}
