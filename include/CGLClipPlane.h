class CGLClipPlane {
 private:
  bool      enabled_;
  CNPlane3D plane_;

 public:
  CGLClipPlane() {
   init();
  }

  void init() {
    enabled_ = false;
  }

  ACCESSOR(Enabled, bool     , enabled)
  ACCESSOR(Plane  , CNPlane3D, plane  )
};
