#ifndef CGL_MATERIAL_H
#define CGL_MATERIAL_H

struct CGLMaterial {
  CMaterial material;

  int ambient_ind;
  int diffuse_ind;
  int specular_ind;

  void init() {
    material.setAmbient  (CRGBA(0.2, 0.2, 0.2));
    material.setDiffuse  (CRGBA(0.8, 0.8, 0.8));
    material.setSpecular (CRGBA(0.0, 0.0, 0.0));
    material.setEmission (CRGBA(0.0, 0.0, 0.0));
    material.setShininess(0.0);

    ambient_ind  = 0;
    diffuse_ind  = 1;
    specular_ind = 1;
  }
};

#endif
