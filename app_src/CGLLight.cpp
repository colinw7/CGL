#include <CGLLight.h>
#include <CGLApp.h>
#include <CPoint3D.h>
#include <GL/glut.h>

struct CPoint3DToFV {
  float fvalues[4];

  CPoint3DToFV(const CPoint3D &point) {
    fvalues[0] = float(point.x);
    fvalues[1] = float(point.y);
    fvalues[2] = float(point.z);
    fvalues[3] = 0.0f;
  }
};

//-----------

CGLLightMgr *
CGLLightMgr::
getInstance()
{
  static CGLLightMgr *instance;

  if (! instance)
    instance = new CGLLightMgr;

  return instance;
}

CGLLightMgr::
CGLLightMgr() :
 id_(GL_LIGHT0)
{
}

CGLLight *
CGLLightMgr::
createLight()
{
  CGLLight *light = new CGLLight(id_++);

  light_map_[light->getId()] = light;

  return light;
}

//-----------

CGLLight::
CGLLight(int id, const CPoint3D &pos) :
 id_(id), pos_(pos), enabled_(false)
{
  setup();
}

void
CGLLight::
setup()
{
  if (enabled_)
    glEnable(uint(id_));
  else
    glDisable(uint(id_));

  glLightfv(uint(id_), GL_AMBIENT , CRGBAToFV(getAmbient ()).fvalues);
  glLightfv(uint(id_), GL_DIFFUSE , CRGBAToFV(getDiffuse ()).fvalues);
  glLightfv(uint(id_), GL_SPECULAR, CRGBAToFV(getSpecular()).fvalues);

  CPoint3DToFV pv(getPosition());

  float *fvalues = pv.fvalues;

  fvalues[3] = (getDirectional() ? 0.0 : 1.0);

  glLightfv(uint(id_), GL_POSITION, fvalues);
}

void
CGLLight::
setEnabled(bool enabled)
{
  enabled_ = enabled;

  if (enabled_)
    glEnable(uint(id_));
  else
    glDisable(uint(id_));

  setup();
}

void
CGLLight::
setAmbient(const CRGBA &rgba)
{
  CGeomLight3DData::setAmbient(rgba);

  glLightfv(uint(id_), GL_AMBIENT, CRGBAToFV(getAmbient()).fvalues);
}

void
CGLLight::
setDiffuse(const CRGBA &rgba)
{
  CGeomLight3DData::setDiffuse(rgba);

  glLightfv(uint(id_), GL_DIFFUSE, CRGBAToFV(getDiffuse()).fvalues);
}

void
CGLLight::
setSpecular(const CRGBA &rgba)
{
  CGeomLight3DData::setSpecular(rgba);

  glLightfv(uint(id_), GL_SPECULAR, CRGBAToFV(getSpecular()).fvalues);
}

void
CGLLight::
setPosition(const CPoint3D &pos)
{
  pos_ = pos;

  CPoint3DToFV pv(getPosition());

  float *fvalues = pv.fvalues;

  fvalues[3] = (getDirectional() ? 0.0 : 1.0);

  glLightfv(uint(id_), GL_POSITION, fvalues);
}
