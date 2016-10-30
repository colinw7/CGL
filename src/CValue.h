#ifndef CVALUE_H
#define CVALUE_H

class CValueBase {
};

template<typename T>
class CValueT : public CValueBase {
 private:
  T value_;

 public:
  CValueT() { }

  explicit CValueT(const T &value) :
   value_(value) {
  }

  CValueT(const CValueT &rhs) :
    value_(rhs.value_) {
  }

  const CValueT &operator=(const CValueT &rhs) {
    value_ = rhs.value_;

    return *this;
  }

  const CValueT &operator=(const T &value) {
    value_ = value;

    return *this;
  }

  const T &getValue() const {
    return value_;
  }

  T &getValue() {
    return value_;
  }

  void getValue(T &value) const {
    value = value_;
  }

  void setValue(const T &value) {
    value_ = value;
  }
};

#endif
