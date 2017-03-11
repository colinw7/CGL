#include <CGLMaterial.h>
#include <CGLApp.h>
#include <GL/glut.h>

void
CGLMaterial::
setup()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT , CRGBAToFV(getAmbient() ).fvalues);
  glMaterialfv(GL_FRONT, GL_DIFFUSE , CRGBAToFV(getDiffuse() ).fvalues);
  glMaterialfv(GL_FRONT, GL_SPECULAR, CRGBAToFV(getSpecular()).fvalues);

  glMaterialfv(GL_FRONT, GL_SHININESS, DoubleToFV(getShininess()).fvalues);
}
