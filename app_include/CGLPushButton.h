#ifndef CGLPushButton_H
#define CGLPushButton_H

#include <CGLWidget.h>

class CGLPushButton : public CGLWidget {
 protected:
  typedef boost::signal<void ()> PressedSignalType;

  std::string       label_;
  CImagePtr         image_;
  PressedSignalType pressedSignal_;

 public:
  CGLPushButton(CGLWidget *parent, const char *name);

  virtual ~CGLPushButton();

  const std::string &getLabel() const { return label_; }

  void setLabel(const std::string &label);

  CImagePtr getImage() const { return image_; }

  void setImage(CImagePtr image);

  void connectPressed(PressedSignalType::slot_function_type slot) {
    pressedSignal_.connect(slot);
  }

 protected:
  void draw();

  void buttonPress(const CMouseEvent &e);
};

#endif
