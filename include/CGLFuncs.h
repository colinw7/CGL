#ifndef CGL_FUNCS_H
#define CGL_FUNCS_H

typedef void (*CGLExposeFunc    )(void);
typedef void (*CGLResizeFunc    )(int width, int height);
typedef void (*CGLMouseFunc     )(int button, int state, int x, int y);
typedef void (*CGLMotionFunc    )(int x, int y);
typedef void (*CGLIdleFunc      )(void);
typedef void (*CGLKeyboardFunc  )(unsigned char key, int x, int y);
typedef void (*CGLVisibilityFunc)(int state);
typedef void (*CGLSpecialFunc   )(int key, int x, int y);

struct CGLFuncs {
  CGLExposeFunc     expose;
  CGLResizeFunc     resize;
  CGLMouseFunc      mouse;
  CGLMotionFunc     motion;
  CGLIdleFunc       idle;
  CGLKeyboardFunc   keyboard;
  CGLVisibilityFunc visibility;
  CGLSpecialFunc    special;

  CGLFuncs() :
   expose    (nullptr),
   resize    (nullptr),
   mouse     (nullptr),
   motion    (nullptr),
   idle      (nullptr),
   keyboard  (nullptr),
   visibility(nullptr),
   special   (nullptr) {
  }
};

#endif
