#include <CGLDispAttr.h>
#include <GL/glut.h>

CGLDispAttr::
CGLDispAttr() :
 enabled_       (false),
 depth_test_    (true),
 cull_face_     (false),
 lighting_      (false),
 outline_       (false),
 front_face_    (false),
 smooth_shade_  (false),
 disable_redraw_(false)
{
}

void
CGLDispAttr::
setEnabled(bool enabled)
{
  enabled_ = enabled;

  setFlags();
}

void
CGLDispAttr::
keyPress(const CKeyEvent &event)
{
  if (! enabled_) return;

  if      (event.isType(CKEY_TYPE_F1) || event.isType(CKEY_TYPE_KP_F1)) {
    depth_test_ = ! depth_test_;

    std::cerr << "Depth Test: " << (depth_test_ ? "Yes" : "No") << std::endl;
  }
  else if (event.isType(CKEY_TYPE_F2) || event.isType(CKEY_TYPE_KP_F2)) {
    cull_face_ = ! cull_face_;

    std::cerr << "Cull Face: " << (cull_face_ ? "Yes" : "No") << std::endl;
  }
  else if (event.isType(CKEY_TYPE_F3) || event.isType(CKEY_TYPE_KP_F3)) {
    lighting_ = ! lighting_;

    std::cerr << "Lighting: " << (lighting_ ? "Yes" : "No") << std::endl;
  }
  else if (event.isType(CKEY_TYPE_F4) || event.isType(CKEY_TYPE_KP_F4)) {
    outline_ = ! outline_;

    std::cerr << "Outline: " << (outline_ ? "Yes" : "No") << std::endl;
  }
  else if (event.isType(CKEY_TYPE_F5)) {
    front_face_ = ! front_face_;

    std::cerr << "Front Face: " << (front_face_ ? "CW" : "CCW") << std::endl;
  }
  else if (event.isType(CKEY_TYPE_F6)) {
    smooth_shade_ = ! smooth_shade_;

    std::cerr << "Shade: " << (smooth_shade_ ? "Smooth" : "Flat") << std::endl;
  }
  else if (event.isType(CKEY_TYPE_h) || event.isType(CKEY_TYPE_H)) {
    std::cerr << "F1: Depth Test: " << (depth_test_   ? "Yes"    : "No"  ) << std::endl;
    std::cerr << "F2: Cull Face : " << (cull_face_    ? "Yes"    : "No"  ) << std::endl;
    std::cerr << "F3: Lighting  : " << (lighting_     ? "Yes"    : "No"  ) << std::endl;
    std::cerr << "F4: Outline   : " << (outline_      ? "Yes"    : "No"  ) << std::endl;
    std::cerr << "F5: Front Face: " << (front_face_   ? "CW"     : "CCW" ) << std::endl;
    std::cerr << "F6: Shade     : " << (smooth_shade_ ? "Smooth" : "Flat") << std::endl;

    return;
  }

  setFlags();

  if (! disable_redraw_)
    glutPostRedisplay();
}

void
CGLDispAttr::
setFlags()
{
  depth_test_ ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
  cull_face_  ? glEnable(GL_CULL_FACE)  : glDisable(GL_CULL_FACE);
  lighting_   ? glEnable(GL_LIGHTING)   : glDisable(GL_LIGHTING);

  outline_ ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) :
             glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  front_face_ ? glFrontFace(GL_CW) : glFrontFace(GL_CCW);

  smooth_shade_ ? glShadeModel(GL_SMOOTH) : glShadeModel(GL_FLAT);
}
