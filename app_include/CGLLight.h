#ifndef CGL_LIGHT_H
#define CGL_LIGHT_H

#define CGLLightMgrInst CGLLightMgr::getInstance()

#include <CGeomLight3D.h>
#include <CRGBA.h>
#include <map>

class CGLLight;

class CGLLightMgr {
 private:
  typedef std::map<int,CGLLight*> LightMap;

  int      id_;
  LightMap light_map_;

 public:
  static CGLLightMgr *getInstance();

  CGLLight *createLight();

 private:
  CGLLightMgr();
 ~CGLLightMgr() { }
};

class CGLLight : public CGeomLight3DData {
 private:
  int      id_;
  CPoint3D pos_;
  bool     enabled_;

 private:
  friend class CGLLightMgr;

  CGLLight(int id, const CPoint3D &pos=CPoint3D(0,0,0));

 public:
  ACCESSOR(Id, int, id)

  void setup();

  bool getEnabled() const { return enabled_; }

  void setEnabled(bool enabled);

  const CPoint3D &getPosition() const { return pos_; }

  void setPosition(const CPoint3D &pos);

  void setAmbient (const CRGBA &rgba);
  void setDiffuse (const CRGBA &rgba);
  void setSpecular(const CRGBA &rgba);
};

#endif
