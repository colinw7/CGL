#include <CPoint4D.h>

class CGLTextureGen {
 private:
  bool senabled_;
  bool tenabled_;
  bool renabled_;
  bool qenabled_;

  uint smode_;
  uint tmode_;
  uint rmode_;
  uint qmode_;

  CPoint4D sobject_plane_;
  CPoint4D tobject_plane_;
  CPoint4D robject_plane_;
  CPoint4D qobject_plane_;

  CPoint4D seye_plane_;
  CPoint4D teye_plane_;
  CPoint4D reye_plane_;
  CPoint4D qeye_plane_;

 public:
  CGLTextureGen() :
   senabled_(false), tenabled_(false), renabled_(false), qenabled_(false),
   smode_(GL_OBJECT_LINEAR), tmode_(GL_OBJECT_LINEAR),
   rmode_(GL_OBJECT_LINEAR), qmode_(GL_OBJECT_LINEAR) {
  }

  ACCESSOR(SEnabled, bool, senabled)
  ACCESSOR(TEnabled, bool, tenabled)
  ACCESSOR(REnabled, bool, renabled)
  ACCESSOR(QEnabled, bool, qenabled)

  ACCESSOR(SMode, uint, smode)
  ACCESSOR(TMode, uint, tmode)
  ACCESSOR(RMode, uint, rmode)
  ACCESSOR(QMode, uint, qmode)

  ACCESSOR(SObjectPlane, CPoint4D, sobject_plane)
  ACCESSOR(TObjectPlane, CPoint4D, tobject_plane)
  ACCESSOR(RObjectPlane, CPoint4D, robject_plane)
  ACCESSOR(QObjectPlane, CPoint4D, qobject_plane)

  ACCESSOR(SEyePlane, CPoint4D, seye_plane)
  ACCESSOR(TEyePlane, CPoint4D, teye_plane)
  ACCESSOR(REyePlane, CPoint4D, reye_plane)
  ACCESSOR(QEyePlane, CPoint4D, qeye_plane)
};
