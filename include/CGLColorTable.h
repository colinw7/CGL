class CGLColorTable {
 private:
  uint   type_;
  CRGBA *values_;
  uint   width_;

 public:
  CGLColorTable() :
   type_(GL_RGBA), values_(NULL), width_(0) {
  }

  CGLColorTable(const CGLColorTable &color_table) :
   type_(GL_RGBA), values_(NULL), width_(0) {
    setType(color_table.type_);
    setSize(color_table.width_);

    memcpy(values_, color_table.values_, width_*sizeof(CRGBA));
  }

 ~CGLColorTable() {
    delete [] values_;
  }

  const CGLColorTable &operator=(const CGLColorTable &color_table) {
    setType(color_table.type_);
    setSize(color_table.width_);

    memcpy(values_, color_table.values_, width_*sizeof(CRGBA));

    return *this;
  }

  void setType(uint type) {
    type_ = type;
  }

  void setSize(uint width) {
    if (width != width_) {
      delete [] values_;

      width_  = width;
      values_ = new CRGBA [width_ + 1];
    }
  }

  void setValue(uint i, const CRGBA &value) {
    values_[i] = value;
  }

  CRGBA remap(const CRGBA &rgba) const {
    double r, g, b, a;

    rgba.getRGBA(&r, &g, &b, &a);

    if (type_ == GL_RGB || type_ == GL_RGBA) {
      r = values_[int(r*(width_ - 1))].getRed  ();
      g = values_[int(g*(width_ - 1))].getGreen();
      b = values_[int(b*(width_ - 1))].getBlue ();
    }

    if (type_ == GL_RGBA)
      a = values_[int(a*(width_ - 1))].getAlpha();

    return CRGBA(r, g, b, a);
  }
};
