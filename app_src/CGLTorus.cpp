#include <CGLTorus.h>
#include <CGLApp.h>
#include <CMatrix3DH.h>
#include <GL/glut.h>

CGLTorus::
CGLTorus(double majorRadius, double minorRadius, int numMajor, int numMinor) :
 majorRadius_(majorRadius),
 minorRadius_(minorRadius),
 numMajor_   (numMajor   ),
 numMinor_   (numMinor   ),
 color_      (1, 1, 1    )
{
}

void
CGLTorus::
setColor(const CRGBA &color)
{
  color_ = color;
}

void
CGLTorus::
draw()
{
  double majorStep = 2.0*M_PI/numMajor_;
  double minorStep = 2.0*M_PI/numMinor_;

  // Draw torus as a series of triangle strips
  for (int i = 0; i < numMajor_; ++i) {
    double a0 = i*majorStep;
    double a1 = a0 + majorStep;

    double x0 = cos(a0), y0 = sin(a0);
    double x1 = cos(a1), y1 = sin(a1);

    glBegin(GL_TRIANGLE_STRIP);

    for (int j = 0; j <= numMinor_; ++j) {
      double b = j*minorStep;

      double c = cos(b), s = sin(b);

      double r = minorRadius_*c + majorRadius_;
      double z = minorRadius_*s;

      // First point
      CVector3D normal1(x0*c, y0*c, z/minorRadius_);

      normal1.normalize();

      CPoint3D point1(x0*r, y0*r, z);

      processPoint(point1, normal1);

      // Second point
      CVector3D normal2(x1*c, y1*c, z/minorRadius_);

      normal2.normalize();

      CPoint3D point2(x1*r, y1*r, z);

      processPoint(point2, normal2);
    }

    glEnd();
  }
}

void
CGLTorus::
processPoint(const CPoint3D &point, const CVector3D &normal)
{
  glColor3d(color_.getRed(), color_.getGreen(), color_.getBlue());

  glNormal3d(normal.getX(), normal.getY(), normal.getZ());

  glVertex3d(point.x, point.y, point.z);
}

//------

static unsigned char
toonShade[4][3] = {
 { 0,  32, 0 },
 { 0,  64, 0 },
 { 0, 128, 0 },
 { 0, 192, 0 }
};

CGLToonTorus::
CGLToonTorus(double majorRadius, double minorRadius, int numMajor, int numMinor,
             const CVector3D &lightDir) :
 CGLTorus(majorRadius, minorRadius, numMajor, numMinor), lightDir_(lightDir)
{
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 4, 0, GL_RGB,
               GL_UNSIGNED_BYTE, toonShade);

  glEnable(GL_TEXTURE_1D);
}

void
CGLToonTorus::
draw()
{
  // Get the modelview matrix
  double modelMatrix[16];

  glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);

  // Instead of transforming every normal and then dotting it with
  // the light vector, we will transform the light into object
  // space by multiplying it by the inverse of the modelview matrix
  CMatrix3DH mat;

  CGLAppInst->toMatrix(modelMatrix, mat);

  CMatrix3DH imat = mat.inverse();

  newLight_ = imat.multiplyVector(lightDir_);

  newLight_ -= CVector3D(imat.getValue(3), imat.getValue(7), imat.getValue(11));

  newLight_.normalize();

  //------

  CGLTorus::draw();
}

void
CGLToonTorus::
processPoint(const CPoint3D &point, const CVector3D &normal)
{
  // Texture coordinate is set by intensity of light
  double dot = newLight_.dotProduct(normal);

  glTexCoord1d(dot);

  glVertex3d(point.x, point.y, point.z);
}
