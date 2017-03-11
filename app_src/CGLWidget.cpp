#include <CGLWidget.h>

CGLWidgetMgr::
CGLWidgetMgr() :
 id_(1000), root_(NULL)
{
}

CGLWidgetMgr::
~CGLWidgetMgr()
{
}

CGLWidgetMgr *
CGLWidgetMgr::
getInstance()
{
  static CGLWidgetMgr *instance;

  if (! instance)
    instance = new CGLWidgetMgr;

  return instance;
}

int
CGLWidgetMgr::
getId() const
{
  return id_;
}

CGLWidget *
CGLWidgetMgr::
getRoot() const
{
  return root_;
}

void
CGLWidgetMgr::
setRoot(CGLWidget *root)
{
  root_ = root;
}

//---------

CGLWidget::
CGLWidget(CGLWindow *window, const char *name) :
 window_(window), parent_(NULL), name_(name)
{
  CGLWidgetMgrInst->setRoot(this);

  initWidget();

  window->addWidget(this);
}

CGLWidget::
CGLWidget(CGLWidget *parent, const char *name) :
 window_(parent->getWindow()), parent_(parent), name_(name)
{
  initWidget();

  parent_->addChild(this);
}

void
CGLWidget::
initWidget()
{
  bbox_ = CIBBox2D(0, 0, 100, 100);

  bg_ = CRGBA(0.8, 0.8, 0.8, 0.4);
  fg_ = CRGBA(0.0, 0.0, 0.0);

  visible_  = true;
  autoFill_ = true;
}

CGLWidget::
~CGLWidget()
{
}

void
CGLWidget::
addChild(CGLWidget *widget)
{
  children_.push_back(widget);
}

uint
CGLWidget::
addRegion(const CIBBox2D &bbox, RegionSignalType::slot_function_type slot)
{
  Region *region = new Region(bbox, slot);

  regionList_.push_back(region);

  return region->id;
}

void
CGLWidget::
setRegionBBox(uint id, const CIBBox2D &bbox)
{
  RegionList::const_iterator p1, p2;

  for (p1 = regionList_.begin(), p2 = regionList_.end(); p1 != p2; ++p1) {
    if ((*p1)->id != id) continue;

    (*p1)->bbox = bbox;
  }
}

void
CGLWidget::
setBackground(const CRGBA &bg)
{
  bg_ = bg;
}

void
CGLWidget::
setForeground(const CRGBA &fg)
{
  fg_ = fg;
}

CFontPtr
CGLWidget::
getFont() const
{
  if (! font_.isValid() && parent_)
    return parent_->getFont();

  return font_;
}

void
CGLWidget::
setFont(CFontPtr font)
{
  font_ = font;
}

void
CGLWidget::
setAutoFill(bool autoFill)
{
  autoFill_ = autoFill;
}

void
CGLWidget::
setVisible(bool visible)
{
  visible_ = visible;
}

void
CGLWidget::
setGeometry(const CIBBox2D &bbox)
{
  bbox_ = bbox;
}

uint
CGLWidget::
getX() const
{
  if (parent_)
    return parent_->getX() + bbox_.getXMin();
  else
    return bbox_.getXMin();
}

uint
CGLWidget::
getY() const
{
  if (parent_)
    return parent_->getY() + bbox_.getYMin();
  else
    return bbox_.getYMin();
}

uint
CGLWidget::
getWidth() const
{
  return bbox_.getWidth();
}

uint
CGLWidget::
getHeight() const
{
  return bbox_.getHeight();
}

CGLRenderer3D *
CGLWidget::
getRenderer() const
{
  return window_->getRenderer();
}

void
CGLWidget::
exposeEvent()
{
  CGLRenderer3D *renderer = window_->getRenderer();

  int w = getWidth ();
  int h = getHeight();

  //int s = 0.95*std::min(w, h);

  //int dx = (w - s)/2;
  //int dy = (h - s)/2;

  renderer->setOverlayPixelRange(0, 0, w - 1, h - 1);

  //renderer->setOverlayDataRange(-dx, s + dy, s + dx, -dy);
  renderer->setOverlayDataRange(0, h - 1, w - 1, 0);

  glLoadName(CGLWidgetMgrInst->getId());

  if (autoFill_) {
    renderer->setForeground(bg_);

    double x1 = getX();
    double y1 = getY();
    double x2 = x1 + getWidth () - 1;
    double y2 = y1 + getHeight() - 1;

    renderer->setOverlayLayer(1);

    renderer->fillOverlayRectangle(x1, y1, x2, y2);
  }

  renderer->setForeground(fg_);

  draw();

  WidgetList::const_iterator p1, p2;

  for (p1 = children_.begin(), p2 = children_.end(); p1 != p2; ++p1) {
    if (! (*p1)->getVisible()) continue;

    (*p1)->draw();
  }
}

void
CGLWidget::
resizeEvent()
{
}

void
CGLWidget::
buttonPressEvent(const CMouseEvent &event)
{
  WidgetList::const_iterator p1, p2;

  for (p1 = children_.begin(), p2 = children_.end(); p1 != p2; ++p1) {
    if (! (*p1)->getVisible()) continue;

    if (! (*p1)->isInside(event.getPosition())) continue;

    if (! (*p1)->regionList_.empty()) {
      RegionList::const_iterator pr1, pr2;

      for (pr1 = (*p1)->regionList_.begin(),
             pr2 = (*p1)->regionList_.end(); pr1 != pr2; ++pr1) {
        if (! (*pr1)->bbox.inside(event.getPosition())) continue;

        (*pr1)->signal();
      }
    }
    else
      (*p1)->buttonPress(event);
  }
}

void
CGLWidget::
buttonMotionEvent(const CMouseEvent &event)
{
  WidgetList::const_iterator p1, p2;

  for (p1 = children_.begin(), p2 = children_.end(); p1 != p2; ++p1) {
    if (! (*p1)->getVisible()) continue;

    (*p1)->buttonMotion(event);
  }
}

void
CGLWidget::
buttonReleaseEvent(const CMouseEvent &event)
{
  WidgetList::const_iterator p1, p2;

  for (p1 = children_.begin(), p2 = children_.end(); p1 != p2; ++p1) {
    if (! (*p1)->getVisible()) continue;

    (*p1)->buttonRelease(event);
  }
}

void
CGLWidget::
keyPressEvent(const CKeyEvent &event)
{
  WidgetList::const_iterator p1, p2;

  for (p1 = children_.begin(), p2 = children_.end(); p1 != p2; ++p1) {
    if (! (*p1)->getVisible()) continue;

    (*p1)->keyPress(event);
  }
}

void
CGLWidget::
keyReleaseEvent(const CKeyEvent &event)
{
  WidgetList::const_iterator p1, p2;

  for (p1 = children_.begin(), p2 = children_.end(); p1 != p2; ++p1) {
    if (! (*p1)->getVisible()) continue;

    (*p1)->keyRelease(event);
  }
}

void
CGLWidget::
draw()
{
}

void
CGLWidget::
buttonPress(const CMouseEvent &)
{
}

void
CGLWidget::
buttonMotion(const CMouseEvent &)
{
}

void
CGLWidget::
buttonRelease(const CMouseEvent &)
{
}

void
CGLWidget::
keyPress(const CKeyEvent &)
{
}

void
CGLWidget::
keyRelease(const CKeyEvent &)
{
}

bool
CGLWidget::
isInside(const CIPoint2D &pos)
{
  CIBBox2D bbox(CIPoint2D(getX(), getY()), CISize2D(getWidth(), getHeight()));

  return bbox.inside(pos);
}
