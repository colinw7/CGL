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
  CGLExposeFunc    expose;
  CGLResizeFunc     resize;
  CGLMouseFunc      mouse;
  CGLMotionFunc     motion;
  CGLIdleFunc       idle;
  CGLKeyboardFunc   keyboard;
  CGLVisibilityFunc visibility;
  CGLSpecialFunc    special;

  CGLFuncs() :
   expose    (0),
   resize    (0),
   mouse     (0),
   motion    (0),
   idle      (0),
   keyboard  (0),
   visibility(0),
   special   (0) {
  }
};

#endif
