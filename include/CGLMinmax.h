class CGLMinmax {
 private:
  uint  type_;
  CRGBA minv_, maxv_;
  bool  sink_;

 public:
  CGLMinmax() :
   type_(GL_RGBA), sink_(false) {
  }

  CGLMinmax(const CGLMinmax &minmax) :
   type_(minmax.type_), minv_(minmax.minv_),
   maxv_(minmax.maxv_), sink_(minmax.sink_) {
  }

 ~CGLMinmax() {
  }

  const CGLMinmax &operator=(const CGLMinmax &minmax) {
    type_ = minmax.type_;
    minv_ = minmax.minv_;
    maxv_ = minmax.maxv_;
    sink_ = minmax.sink_;

    return *this;
  }

  ACCESSOR(Type , uint , type)
  ACCESSOR(Min  , CRGBA, minv)
  ACCESSOR(Max  , CRGBA, maxv)
  ACCESSOR(Sink , bool , sink)
};
