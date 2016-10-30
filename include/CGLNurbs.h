#ifndef CGL_NURBS_H
#define CGL_NURBS_H

#include <map>
#include <sys/types.h>

class GLUnurbs {
 public:
  typedef void (*CallBackFunc)();

 private:
  std::map<uint,CallBackFunc> callbacks_;

 public:
  GLUnurbs();
 ~GLUnurbs();

  void setCallback(uint type, CallBackFunc func);
};

#endif
