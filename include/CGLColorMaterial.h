class CGLColorMaterial {
 private:
  bool enabled_;
  uint face_;
  uint mode_;

 public:
  CGLColorMaterial() :
   enabled_(false), face_(GL_FRONT_AND_BACK),
   mode_(GL_AMBIENT_AND_DIFFUSE) {
  }

  ACCESSOR(Enabled, bool, enabled)
  ACCESSOR(Face   , uint, face   )
  ACCESSOR(Mode   , uint, mode   )
};
