#ifndef CGL_TORUS_H
#define CGL_TORUS_H

#include <CVector3D.h>
#include <CRGBA.h>

class CGLTorus {
 private:
  double majorRadius_;
  double minorRadius_;
  int    numMajor_;
  int    numMinor_;
  CRGBA  color_;

 public:
  CGLTorus(double majorRadius, double minorRadius, int numMajor, int numMinor);

  virtual ~CGLTorus() { }

  virtual void setColor(const CRGBA &rgba);

  virtual void draw();

  virtual void processPoint(const CPoint3D &point, const CVector3D &normal);
};

class CGLToonTorus : public CGLTorus {
 private:
  CVector3D lightDir_;
  CVector3D newLight_;

 public:
  CGLToonTorus(double majorRadius, double minorRadius, int numMajor,
               int numMinor, const CVector3D &lightDir);

  void draw() override;

  void processPoint(const CPoint3D &point, const CVector3D &normal) override;
};

#endif
