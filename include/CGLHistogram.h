class CGLHistogram {
 private:
  uint   type_;
  CRGBA *values_;
  uint   width_;
  bool   sink_;

 public:
  CGLHistogram() :
   type_(GL_RGBA), values_(NULL), width_(0), sink_(false) {
  }

  CGLHistogram(const CGLHistogram &histogram) :
   type_(GL_RGBA), values_(NULL), width_(0), sink_(false) {
    setType (histogram.type_ );
    setWidth(histogram.width_);
    setSink (histogram.sink_ );

    for (uint i = 0; i < width_; ++i)
      values_[i] = histogram.values_[i];
  }

 ~CGLHistogram() {
    delete [] values_;
  }

  const CGLHistogram &operator=(const CGLHistogram &histogram) {
    setType (histogram.type_ );
    setWidth(histogram.width_);
    setSink (histogram.sink_ );

    for (uint i = 0; i < width_; ++i)
      values_[i] = histogram.values_[i];

    return *this;
  }

  ACCESSOR(Type , uint, type )
  ACCESSOR(Sink , bool, sink )

  ACCESSOR_IMPL_GET(Width, uint, width)

  void setWidth(uint width) {
    if (width != width_) {
      delete [] values_;

      width_  = width;
      values_ = new CRGBA [width_];
    }
  }

  const CRGBA &getValue(uint i) const {
    return values_[i];
  }

  void setValue(uint i, const CRGBA &value) {
    values_[i] = value;
  }

  void print(std::ostream &os) const {
    for (uint i = 0; i < width_; ++i)
      os << i << ": " << values_[i] << std::endl;
  }

  friend std::ostream &operator<<(std::ostream &os, const CGLHistogram &histogram) {
    histogram.print(os);

    return os;
  }
};
