#include <CGLDisplayList.h>
#include <CGL.h>

uint CGLDisplayList::current_id_ = 1;

CGLDisplayList::
CGLDisplayList()
{
  id_ = current_id_++;
}

CGLDisplayList::
CGLDisplayList(uint id) :
 id_(id)
{
  if (id_ > current_id_)
    current_id_ = id_ + 1;
}

CGLDisplayList::
~CGLDisplayList()
{
  clear();
}

bool
CGLDisplayList::
execute(CGL *cgl)
{
  std::vector<CGLDisplayListCmd *>::const_iterator pcmd1 = cmds_.begin();
  std::vector<CGLDisplayListCmd *>::const_iterator pcmd2 = cmds_.end  ();

  for ( ; pcmd1 != pcmd2; ++pcmd1)
    (*pcmd1)->execute(cgl);

  return true;
}

void
CGLDisplayList::
clear()
{
  std::vector<CGLDisplayListCmd *>::const_iterator pcmd1 = cmds_.begin();
  std::vector<CGLDisplayListCmd *>::const_iterator pcmd2 = cmds_.end  ();

  for ( ; pcmd1 != pcmd2; ++pcmd1)
    delete *pcmd1;

  cmds_.clear();
}

void
CGLDisplayList::
beginBlock(int mode)
{
  CGLDisplayListStartBlock *cmd =
    new CGLDisplayListStartBlock(mode);

  cmds_.push_back(cmd);
}

void
CGLDisplayList::
setBackground(const CRGBA &bg)
{
  CGLDisplayListSetBackground *cmd =
    new CGLDisplayListSetBackground(bg);

  cmds_.push_back(cmd);
}

void
CGLDisplayList::
setForeground(const CRGBA &fg)
{
  CGLDisplayListSetForeground *cmd =
    new CGLDisplayListSetForeground(fg);

  cmds_.push_back(cmd);
}

void
CGLDisplayList::
setNormal(const CVector3D &normal)
{
  CGLDisplayListSetNormal *cmd =
    new CGLDisplayListSetNormal(normal);

  cmds_.push_back(cmd);
}

void
CGLDisplayList::
setTexCoord(const CPoint3D &point)
{
  CGLDisplayListSetTexCoord *cmd =
    new CGLDisplayListSetTexCoord(point);

  cmds_.push_back(cmd);
}

void
CGLDisplayList::
addBlockPoint(const CPoint3D &point)
{
  CGLDisplayListAddBlockPoint *cmd =
    new CGLDisplayListAddBlockPoint(point);

  cmds_.push_back(cmd);
}

void
CGLDisplayList::
multMatrix(const CMatrix3DH &matrix)
{
  CGLDisplayListMultMatrix *cmd =
    new CGLDisplayListMultMatrix(matrix);

  cmds_.push_back(cmd);
}

void
CGLDisplayList::
bitmap(uint width, uint height, double xorig, double yorig,
       double xmove, double ymove, const uchar *bitmap)
{
  CGLDisplayListBitmap *cmd =
    new CGLDisplayListBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);

  cmds_.push_back(cmd);
}

void
CGLDisplayList::
setMaterial(uint face, uint pname, double param)
{
  CGLDisplayListSetMaterial *cmd =
    new CGLDisplayListSetMaterial(face, pname, param);

  cmds_.push_back(cmd);
}

void
CGLDisplayList::
setMaterialV(uint face, uint pname, double *params, uint num_params)
{
  CGLDisplayListSetMaterialV *cmd =
    new CGLDisplayListSetMaterialV(face, pname, params, num_params);

  cmds_.push_back(cmd);
}

void
CGLDisplayList::
endBlock()
{
  CGLDisplayListEndBlock *cmd =
    new CGLDisplayListEndBlock();

  cmds_.push_back(cmd);
}

//----------------

bool
CGLDisplayListStartBlock::
execute(CGL *cgl)
{
  cgl->beginBlock(mode_);

  return true;
}

bool
CGLDisplayListSetBackground::
execute(CGL *cgl)
{
  cgl->setBackground(bg_);

  return true;
}

bool
CGLDisplayListSetForeground::
execute(CGL *cgl)
{
  cgl->setForeground(fg_);

  return true;
}

bool
CGLDisplayListSetNormal::
execute(CGL *cgl)
{
  cgl->setNormal(normal_);

  return true;
}

bool
CGLDisplayListSetTexCoord::
execute(CGL *cgl)
{
  cgl->setTexCoord(point_);

  return true;
}

bool
CGLDisplayListAddBlockPoint::
execute(CGL *cgl)
{
  cgl->addBlockPoint(point_);

  return true;
}

bool
CGLDisplayListMultMatrix::
execute(CGL *cgl)
{
  cgl->multMatrix(&matrix_);

  return true;
}

bool
CGLDisplayListBitmap::
execute(CGL *cgl)
{
  cgl->drawBitmapI(width_, height_, xorig_, yorig_, xmove_, ymove_, bitmap_);

  return true;
}

bool
CGLDisplayListSetMaterial::
execute(CGL *cgl)
{
  cgl->setMaterial(face_, pname_, param_);

  return true;
}

bool
CGLDisplayListSetMaterialV::
execute(CGL *cgl)
{
  cgl->setMaterialV(face_, pname_, params_, num_params_);

  return true;
}

bool
CGLDisplayListEndBlock::
execute(CGL *cgl)
{
  cgl->endBlock();

  return true;
}
