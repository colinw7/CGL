#ifndef CGLDISPLAY_LIST_H
#define CGLDISPLAY_LIST_H

#include <CRGBA.h>
#include <CVector3D.h>
#include <CMatrix3DH.h>
#include <vector>

class CGL;

class CGLDisplayListCmd {
 public:
  virtual ~CGLDisplayListCmd() { }

  virtual bool execute(CGL *cgl) = 0;
};

class CGLDisplayListStartBlock : public CGLDisplayListCmd {
 private:
  int mode_;

 public:
  CGLDisplayListStartBlock(int mode) :
   mode_(mode) {
  }

  bool execute(CGL *cgl);
};

class CGLDisplayListSetBackground : public CGLDisplayListCmd {
 private:
  CRGBA bg_;

 public:
  CGLDisplayListSetBackground(const CRGBA &bg) :
   bg_(bg) {
  }

  bool execute(CGL *cgl);
};

class CGLDisplayListSetForeground : public CGLDisplayListCmd {
 private:
  CRGBA fg_;

 public:
  CGLDisplayListSetForeground(const CRGBA &fg) :
   fg_(fg) {
  }

  bool execute(CGL *cgl);
};

class CGLDisplayListSetNormal : public CGLDisplayListCmd {
 private:
  CVector3D normal_;

 public:
  CGLDisplayListSetNormal(const CVector3D &normal) :
   normal_(normal) {
  }

  bool execute(CGL *cgl);
};

class CGLDisplayListSetTexCoord : public CGLDisplayListCmd {
 private:
  CPoint3D point_;

 public:
  CGLDisplayListSetTexCoord(const CPoint3D &point) :
   point_(point) {
  }

  bool execute(CGL *cgl);
};

class CGLDisplayListAddBlockPoint : public CGLDisplayListCmd {
 private:
  CPoint3D point_;

 public:
  CGLDisplayListAddBlockPoint(const CPoint3D &point) :
   point_(point) {
  }

  bool execute(CGL *cgl);
};

class CGLDisplayListMultMatrix : public CGLDisplayListCmd {
 private:
  CMatrix3DH matrix_;

 public:
  CGLDisplayListMultMatrix(const CMatrix3DH &matrix) :
   matrix_(matrix) {
  }

  bool execute(CGL *cgl);
};

class CGLDisplayListSetMaterial : public CGLDisplayListCmd {
 private:
  uint   face_;
  uint   pname_;
  double param_;

 public:
  CGLDisplayListSetMaterial(uint face, uint pname, double param) :
   face_(face), pname_(pname), param_(param) {
  }

  bool execute(CGL *cgl);
};

class CGLDisplayListSetMaterialV : public CGLDisplayListCmd {
 private:
  uint    face_;
  uint    pname_;
  double *params_;
  uint    num_params_;

 public:
  CGLDisplayListSetMaterialV(uint face, uint pname,
                             double *params, uint num_params) :
   face_(face), pname_(pname), params_(NULL), num_params_(num_params) {
    params_ = new double [num_params_];

    memcpy(params_, params, num_params_*sizeof(double));
  }

 ~CGLDisplayListSetMaterialV() {
    delete [] params_;
  }

  bool execute(CGL *cgl);
};

class CGLDisplayListBitmap : public CGLDisplayListCmd {
 private:
  typedef unsigned char uchar;

  uint    width_, height_;
  double  xorig_, yorig_;
  double  xmove_, ymove_;
  uchar  *bitmap_;

 public:
  CGLDisplayListBitmap(uint width, uint height, double xorig, double yorig,
                       double xmove, double ymove, const uchar *bitmap) :
   width_(width), height_(height), xorig_(xorig), yorig_(yorig),
   xmove_(xmove), ymove_(ymove), bitmap_(NULL) {
    if (bitmap != NULL) {
      uint width1 = (width + 7) >> 3;

      uint size = width1*height;

      bitmap_ = new uchar [size];

      memcpy(bitmap_, bitmap, size);
    }
  }

 ~CGLDisplayListBitmap() {
    delete [] bitmap_;
  }

  bool execute(CGL *cgl);
};

class CGLDisplayListEndBlock : public CGLDisplayListCmd {
 public:
  bool execute(CGL *cgl);
};

class CGLDisplayList {
 private:
  typedef unsigned char uchar;

  typedef std::vector<CGLDisplayListCmd *> CmdList;

  static uint current_id_;

  uint    id_;
  bool    execute_;
  CmdList cmds_;

 public:
  CGLDisplayList();
  CGLDisplayList(uint id);
 ~CGLDisplayList();

  uint getId() const { return id_; }

  bool getExecute() const { return execute_; }
  void setExecute(bool execute) { execute_ = execute; }

  bool execute(CGL *cgl);

  void clear();

  void beginBlock(int mode);

  void setBackground(const CRGBA &bg);
  void setForeground(const CRGBA &fg);

  void setNormal(const CVector3D &normal);

  void setTexCoord(const CPoint3D &point);

  void addBlockPoint(const CPoint3D &point);

  void multMatrix(const CMatrix3DH &matrix);

  void bitmap(uint width, uint height, double xorig, double yorig,
              double xmove, double ymove, const uchar *bitmap);

  void setMaterial(uint face, uint pname, double param);

  void setMaterialV(uint face, uint pname, double *params, uint num_params);

  void endBlock();
};

#endif
