#ifndef CGLIntSpinBox_H
#define CGLIntSpinBox_H

#include <CGLWidget.h>
#include <COptVal.h>

class CGLIntSpinBox : public CGLWidget {
 protected:
  typedef boost::signal<void ()> ValueChangedSignalType;

  std::string            label_;
  CImagePtr              image_;
  int                    value_;
  int                    border_;
  COptValT<int>          minValue_, maxValue_;
  uint                   region1_, region2_;
  ValueChangedSignalType valueChangedSignal_;

 public:
  CGLIntSpinBox(CGLWidget *parent, const char *name);

  virtual ~CGLIntSpinBox();

  const std::string &getLabel() const { return label_; }

  void setLabel(const std::string &label);

  CImagePtr getImage() const { return image_; }

  int getValue() const { return value_; }

  void setValue(int value);

  void setMinValue(int minValue);
  void setMaxValue(int maxValue);

  void setImage(CImagePtr image);

  void connectValueChanged(ValueChangedSignalType::slot_function_type slot) {
    valueChangedSignal_.connect(slot);
  }

 protected:
  void draw();

  void upPress();
  void downPress();
};

#endif
