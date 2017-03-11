#ifndef CGL_MATERIAL_H
#define CGL_MATERIAL_H

#include <CMaterial.h>

#define CGLMaterialMgrInst CGLMaterialMgr::getInstance()

class CGLMaterialMgr {
 protected:
  CGLMaterialMgr() { }

 public:
  static CGLMaterialMgr *getInstance() {
    static CGLMaterialMgr *instance;

    if (! instance)
      instance = new CGLMaterialMgr;

    return instance;
  }
};

class CGLMaterial : public CMaterial {
 private:
  std::string name_;

 public:
  CGLMaterial(const std::string &name="") :
   name_(name) {
  }

  void setup();
};

#endif
