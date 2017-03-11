#include <CGLLabel.h>

CGLLabel::
CGLLabel(CGLWidget *parent, const char *name) :
 CGLWidget(parent, name), label_(name)
{
}

CGLLabel::
~CGLLabel()
{
}

void
CGLLabel::
setLabel(const std::string &label)
{
  label_ = label;
}

void
CGLLabel::
setImage(CImagePtr image)
{
  image_ = image;
}

void
CGLLabel::
draw()
{
  CGLRenderer3D *renderer = getRenderer();

  int x1 = getX();
  int y1 = getY();
  int x2 = x1 + getWidth () - 1;
  int y2 = y1 + getHeight() - 1;

  if (image_.isValid()) {
    renderer->setOverlayLayer(2);

    renderer->drawOverlayImage(x1, y1, image_);
  }

  if (label_.size()) {
    renderer->setForeground(getForeground());

    renderer->setFont(getFont());

    CImagePtr image = renderer->imageString(label_);

    int tx = (x2 - x1 - int(image->getWidth ()))/2;
    int ty = (y2 - y1 - int(image->getHeight()))/2;

    renderer->setOverlayLayer(3);

    renderer->drawOverlayImage(x1 + tx, y1 + ty, image);
  }
}
