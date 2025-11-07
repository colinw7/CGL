#ifndef CGLLabel_H
#define CGLLabel_H

#include <CGLWidget.h>
#include <boost/signals2/signal.hpp>
#include <boost/bind.hpp>

class CGLLabel : public CGLWidget {
 protected:
  std::string label_;
  CImagePtr   image_;

 public:
  CGLLabel(CGLWidget *parent, const char *name);

  virtual ~CGLLabel();

  const std::string &getLabel() const { return label_; }

  void setLabel(const std::string &label);

  CImagePtr getImage() const { return image_; }

  void setImage(CImagePtr image);

 protected:
  void draw() override;
};

#endif
