class CGLCullFace {
 private:
  bool enabled_;
  bool front_;
  bool back_;

 public:
  CGLCullFace() :
   enabled_(false), front_(false), back_(true) {
  }

  ACCESSOR(Enabled, bool, enabled)
  ACCESSOR(Front  , bool, front  )
  ACCESSOR(Back   , bool, back   )
};
