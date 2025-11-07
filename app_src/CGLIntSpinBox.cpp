#include <CGLIntSpinBox.h>
#include <CImageLib.h>
#include <CRGBName.h>
#include <CStrUtil.h>

CGLIntSpinBox::
CGLIntSpinBox(CGLWidget *parent, const char *name) :
 CGLWidget(parent, name), label_(name), value_(0), border_(4)
{
  region1_ = addRegion(CIBBox2D(), boost::bind(&CGLIntSpinBox::upPress  , this));
  region2_ = addRegion(CIBBox2D(), boost::bind(&CGLIntSpinBox::downPress, this));
}

CGLIntSpinBox::
~CGLIntSpinBox()
{
}

void
CGLIntSpinBox::
setLabel(const std::string &label)
{
  label_ = label;
}

void
CGLIntSpinBox::
setImage(CImagePtr image)
{
  image_ = image;
}

void
CGLIntSpinBox::
setValue(int value)
{
  value_ = value;
}

void
CGLIntSpinBox::
setMinValue(int minValue)
{
  minValue_ = minValue;
}

void
CGLIntSpinBox::
setMaxValue(int maxValue)
{
  maxValue_ = maxValue;
}

void
CGLIntSpinBox::
draw()
{
  auto *renderer = getRenderer();

  int x1 = int(getX());
  int y1 = int(getY());
  int x2 = x1 + int(getWidth ()) - 1;
  int y2 = y1 + int(getHeight()) - 1;

  if (image_) {
    renderer->setOverlayLayer(2);

    renderer->drawOverlayImage(x1, y1, image_);
  }

  int x = border_;

  if (label_.size()) {
    renderer->setForeground(getForeground());

    renderer->setFont(getFont());

    CImagePtr image = renderer->imageString(label_);

    int ty = (y2 - y1 - int(image->getHeight()))/2;

    renderer->setOverlayLayer(3);

    renderer->drawOverlayImage(x1 + x, y1 + ty, image);

    x += int(image->getWidth()) + border_;
  }

  int bsize = ((y2 - y1) - 3*border_)/2;

  int b1x1 = x2 - border_ - bsize; int b1x2 = b1x1 + bsize;
  int b1y1 = y1 + border_        ; int b1y2 = b1y1 + bsize;

  CRGBA fg1 = CRGBName::toRGBA("#4187fb");
  CRGBA fg2 = CRGBName::toRGBA("#99e1e8");

  renderer->setOverlayLayer(3);

  renderer->fillOverlayTriangle(
    CGLPoint2D(CPoint2D( b1x1          , b1y2), fg2),
    CGLPoint2D(CPoint2D((b1x1 + b1x2)/2, b1y1), fg2),
    CGLPoint2D(CPoint2D( b1x2          , b1y2), fg1));

  setRegionBBox(region1_, CIBBox2D(b1x1, b1y1, b1x2, b1y2));

  int b2x1 = b1x1               ; int b2x2 = b1x2;
  int b2y1 = b1y2 + border_     ; int b2y2 = b2y1 + bsize;

  renderer->setForeground(CRGBA(0.8,0.8,0.8));

  renderer->setOverlayLayer(3);

  renderer->fillOverlayTriangle(
    CGLPoint2D(CPoint2D( b2x1          , b2y1), fg2),
    CGLPoint2D(CPoint2D((b2x1 + b2x2)/2, b2y2), fg1),
    CGLPoint2D(CPoint2D( b2x2          , b2y1), fg1));

  setRegionBBox(region2_, CIBBox2D(b2x1, b2y1, b2x2, b2y2));

  renderer->setForeground(getForeground());

  renderer->setFont(getFont());

  CImagePtr image = renderer->imageString(CStrUtil::toString(value_));

  x = int(getWidth()) - int(2*border_) - bsize - int(image->getWidth());

  int ty = (y2 - y1 - int(image->getHeight()))/2;

  renderer->setOverlayLayer(3);

  renderer->drawOverlayImage(x1 + x, y1 + ty, image);
}

void
CGLIntSpinBox::
upPress()
{
  if (! maxValue_ || value_ < maxValue_.value()) {
    ++value_;

    valueChangedSignal_();

    getRenderer()->flush();
  }
}

void
CGLIntSpinBox::
downPress()
{
  if (! minValue_ || value_ > minValue_.value()) {
    --value_;

    valueChangedSignal_();

    getRenderer()->flush();
  }
}
