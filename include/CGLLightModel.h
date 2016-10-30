struct CGLLightModel {
  CRGBA ambient;
  bool  local_viewer;
  bool  two_side;
  int   color_control;

  CGLLightModel() :
    ambient(0.2,0.2,0.2),
    local_viewer(false),
    two_side(false),
    color_control(GL_SINGLE_COLOR) {
  }
};
