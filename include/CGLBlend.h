struct CGLBlend {
  bool             enabled;
  CRGBACombineMode src_type;
  CRGBACombineMode dst_type;
  CRGBA            color;
  CRGBACombineFunc func;

  CGLBlend() :
   enabled (false),
   src_type(CRGBA_COMBINE_ONE),
   dst_type(CRGBA_COMBINE_ZERO),
   color   (0,0,0,0),
   func    (CRGBA_COMBINE_ADD) {
  }
};
