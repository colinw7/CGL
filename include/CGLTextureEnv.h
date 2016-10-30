class CGLTextureEnv {
 private:
  uint  mode_;
  CRGBA color_;

 public:
  CGLTextureEnv() :
   mode_(GL_REPLACE) {
  }

  ACCESSOR(Mode , uint , mode )
  ACCESSOR(Color, CRGBA, color)
};
