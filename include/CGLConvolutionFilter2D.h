class CGLConvolutionFilter2D {
 private:
  uint   type_;
  CRGBA *values_;
  uint   width_, height_;

 public:
  CGLConvolutionFilter2D() :
   type_(GL_RGBA), values_(NULL), width_(0), height_(0) {
  }

  CGLConvolutionFilter2D(const CGLConvolutionFilter2D &filter) :
   type_(GL_RGBA), values_(NULL), width_(0), height_(0) {
    setType(filter.type_);
    setSize(filter.width_, filter.height_);

    memcpy(values_, filter.values_, width_*height_*sizeof(CRGBA));
  }

 ~CGLConvolutionFilter2D() {
    delete [] values_;
  }

  const CGLConvolutionFilter2D &
  operator=(const CGLConvolutionFilter2D &filter) {
    setType(filter.type_);
    setSize(filter.width_, filter.height_);

    memcpy(values_, filter.values_, width_*height_*sizeof(CRGBA));

    return *this;
  }

  uint getWidth () const { return width_ ; }
  uint getHeight() const { return height_; }

  void setType(uint type) {
    type_ = type;
  }

  void setSize(uint width, uint height) {
    if (width != width_ || height != height_) {
      delete [] values_;

      width_  = width;
      height_ = height;
      values_ = new CRGBA [width_*height_ + 1];
    }
  }

  void setValue(uint x, uint y, const CRGBA &value) {
    values_[y*width_ + x] = value;
  }

  const CRGBA &getValue(uint x, uint y) const {
    return values_[y*width_ + x];
  }

  CRGBA remap(const CRGBA &rgba) const {
    return rgba;
  }
};
