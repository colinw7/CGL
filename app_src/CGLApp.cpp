#include <CGLApp.h>
#include <CGLWindow.h>
#include <CEvent.h>
#include <COSTimer.h>
#include <GL/glut.h>

CGLApp *CGLApp::instance_;

CGLApp *
CGLApp::
getInstance()
{
  if (! instance_)
    instance_ = new CGLApp;

  return instance_;
}

CGLApp::
CGLApp() :
 msecs_       (100),
 mouse_button_(CBUTTON_NONE)
{
  instance_ = this;

  CWindowMgrInst->setFactory(new CGLWindowFactory);
}

CGLApp::
~CGLApp()
{
  instance_ = NULL;
}

void
CGLApp::
init(int argc, char **argv)
{
  init(&argc, argv);
}

void
CGLApp::
init(int *argc, char **argv)
{
  glutInit(argc, argv);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
}

CWindow *
CGLApp::
createWindow(int x, int y, uint width, uint height)
{
  CGLWindow *window = CGLAppInst->newWindow();

  window->init(x, y, width, height);

  window->setup();

  return window;
}

void
CGLApp::
mainLoop()
{
  glutMainLoop();
}

void
CGLApp::
timerLoop(uint msecs)
{
  msecs_ = msecs;

  glutTimerFunc(msecs_, CGLApp::timerCB, 0);

  glutMainLoop();
}

CGLWindow *
CGLApp::
newWindow(CGLWindow *parent)
{
  return new CGLWindow(parent);
}

void
CGLApp::
addWindow(CGLWindow *window)
{
  glutReshapeFunc     (CGLApp::reshapeCB);
  glutDisplayFunc     (CGLApp::displayCB);
  glutKeyboardFunc    (CGLApp::keyboardDownCB);
  glutKeyboardUpFunc  (CGLApp::keyboardUpCB);
  glutSpecialFunc     (CGLApp::specialKeyDownCB);
  glutSpecialUpFunc   (CGLApp::specialKeyUpCB);
  glutMotionFunc      (CGLApp::motionCB);
  glutMouseFunc       (CGLApp::mouseCB);
  glutWindowStatusFunc(CGLApp::statusCB);
  glutEntryFunc       (CGLApp::enterCB);
  glutIdleFunc        (CGLApp::idleCB);

  windows_[window->getId()] = window;
}

CGLWindow *
CGLApp::
lookupWindow(int id) const
{
  WindowMap::const_iterator p = windows_.find(id);

  if (p != windows_.end())
    return (*p).second;

  return NULL;
}

void
CGLApp::
timerCB(int value)
{
  WindowMap::const_iterator p1 = CGLAppInst->windows_.begin();
  WindowMap::const_iterator p2 = CGLAppInst->windows_.end  ();

  for ( ; p1 != p2; ++p1) {
    (*p1).second->setCurrent();

    (*p1).second->timerEvent();
  }

  glutTimerFunc(CGLAppInst->msecs_, CGLApp::timerCB, value);
}

void
CGLApp::
reshapeCB(int w, int h)
{
  int id = glutGetWindow();

  CGLWindow *window = CGLAppInst->lookupWindow(id);

  if (window != NULL)
    window->reshapeCB(w, h);
}

void
CGLApp::
displayCB()
{
  int id = glutGetWindow();

  CGLWindow *window = CGLAppInst->lookupWindow(id);

  if (window != NULL)
    window->displayCB();
}

void
CGLApp::
mouseCB(int button, int state, int x, int y)
{
  CGLApp *app = CGLAppInst;

  switch (button) {
    case GLUT_LEFT_BUTTON  : app->mouse_button_ = CBUTTON_LEFT  ; break;
    case GLUT_MIDDLE_BUTTON: app->mouse_button_ = CBUTTON_MIDDLE; break;
    case GLUT_RIGHT_BUTTON : app->mouse_button_ = CBUTTON_RIGHT ; break;
  }

  int modifier = CMODIFIER_NONE;

  int modifiers = glutGetModifiers();

  if (modifiers == GLUT_ACTIVE_SHIFT) modifier |= CMODIFIER_SHIFT;
  if (modifiers == GLUT_ACTIVE_CTRL ) modifier |= CMODIFIER_CONTROL;
  if (modifiers == GLUT_ACTIVE_ALT  ) modifier |= CMODIFIER_ALT;

  CMouseEvent event(CIPoint2D(x, y), app->mouse_button_, 1, (CEventModifier) modifier);

  int id = glutGetWindow();

  CGLWindow *window = CGLAppInst->lookupWindow(id);

  if (window != NULL) {
    if      (state == GLUT_DOWN)
      window->buttonPressCB(event);
    else if (state == GLUT_UP)
      window->buttonReleaseCB(event);
  }
}

void
CGLApp::
motionCB(int x, int y)
{
  CGLApp *app = CGLAppInst;

  int modifier = CMODIFIER_NONE;

  CMouseEvent event(CIPoint2D(x, y), app->mouse_button_, 1,
                    (CEventModifier) modifier);

  int id = glutGetWindow();

  CGLWindow *window = CGLAppInst->lookupWindow(id);

  if (window != NULL)
    window->buttonMotionCB(event);
}

void
CGLApp::
keyboardDownCB(uchar key, int x, int y)
{
  keyboardCB(key, x, y, GLUT_KEY_DOWN);
}

void
CGLApp::
keyboardUpCB(uchar key, int x, int y)
{
  keyboardCB(key, x, y, GLUT_KEY_UP);
}

void
CGLApp::
keyboardCB(uchar key, int x, int y, int state)
{
  CKeyType key1 = (CKeyType) key;

  int modifier = CMODIFIER_NONE;

  int modifiers = glutGetModifiers();

  if (modifiers == GLUT_ACTIVE_SHIFT) modifier |= CMODIFIER_SHIFT;
  if (modifiers == GLUT_ACTIVE_CTRL ) modifier |= CMODIFIER_CONTROL;
  if (modifiers == GLUT_ACTIVE_ALT  ) modifier |= CMODIFIER_ALT;

  CKeyEvent event(CIPoint2D(x, y), key1,
                  CEvent::keyTypeName(key1),
                  (CEventModifier) modifier);

  int id = glutGetWindow();

  CGLWindow *window = CGLAppInst->lookupWindow(id);

  if (window != NULL) {
    if (state == GLUT_KEY_DOWN)
      window->keyPressCB(event);
    else
      window->keyReleaseCB(event);
  }
}

void
CGLApp::
specialKeyDownCB(int key, int x, int y)
{
  specialKeyCB(key, x, y, GLUT_KEY_DOWN);
}

void
CGLApp::
specialKeyUpCB(int key, int x, int y)
{
  specialKeyCB(key, x, y, GLUT_KEY_UP);
}

void
CGLApp::
specialKeyCB(int key, int x, int y, int state)
{
  CKeyType key1 = CKEY_TYPE_NUL;

  switch (key) {
    case GLUT_KEY_F1       : key1 = CKEY_TYPE_F1       ; break;
    case GLUT_KEY_F2       : key1 = CKEY_TYPE_F2       ; break;
    case GLUT_KEY_F3       : key1 = CKEY_TYPE_F3       ; break;
    case GLUT_KEY_F4       : key1 = CKEY_TYPE_F4       ; break;
    case GLUT_KEY_F5       : key1 = CKEY_TYPE_F5       ; break;
    case GLUT_KEY_F6       : key1 = CKEY_TYPE_F6       ; break;
    case GLUT_KEY_F7       : key1 = CKEY_TYPE_F7       ; break;
    case GLUT_KEY_F8       : key1 = CKEY_TYPE_F8       ; break;
    case GLUT_KEY_F9       : key1 = CKEY_TYPE_F9       ; break;
    case GLUT_KEY_F10      : key1 = CKEY_TYPE_F10      ; break;
    case GLUT_KEY_F11      : key1 = CKEY_TYPE_F11      ; break;
    case GLUT_KEY_F12      : key1 = CKEY_TYPE_F12      ; break;
    case GLUT_KEY_LEFT     : key1 = CKEY_TYPE_Left     ; break;
    case GLUT_KEY_UP       : key1 = CKEY_TYPE_Up       ; break;
    case GLUT_KEY_RIGHT    : key1 = CKEY_TYPE_Right    ; break;
    case GLUT_KEY_DOWN     : key1 = CKEY_TYPE_Down     ; break;
    case GLUT_KEY_PAGE_UP  : key1 = CKEY_TYPE_Page_Up  ; break;
    case GLUT_KEY_PAGE_DOWN: key1 = CKEY_TYPE_Page_Down; break;
    case GLUT_KEY_HOME     : key1 = CKEY_TYPE_Home     ; break;
    case GLUT_KEY_END      : key1 = CKEY_TYPE_End      ; break;
    case GLUT_KEY_INSERT   : key1 = CKEY_TYPE_Insert   ; break;
  };

  int modifier = CMODIFIER_NONE;

  int modifiers = glutGetModifiers();

  if (modifiers == GLUT_ACTIVE_SHIFT) modifier |= CMODIFIER_SHIFT;
  if (modifiers == GLUT_ACTIVE_CTRL ) modifier |= CMODIFIER_CONTROL;
  if (modifiers == GLUT_ACTIVE_ALT  ) modifier |= CMODIFIER_ALT;

  CKeyEvent event(CIPoint2D(x, y), key1,
                  CEvent::keyTypeName(key1),
                  (CEventModifier) modifier);

  int id = glutGetWindow();

  CGLWindow *window = CGLAppInst->lookupWindow(id);

  if (window != NULL) {
    if (state == GLUT_KEY_DOWN)
      window->keyPressCB(event);
    else
      window->keyReleaseCB(event);
  }
}

void
CGLApp::
statusCB(int state)
{
  int id = glutGetWindow();

  CGLWindow *window = CGLAppInst->lookupWindow(id);

  if (window != NULL) {
    if (state == GLUT_HIDDEN || state == GLUT_FULLY_COVERED)
      window->hideCB();
    else
      window->showCB();
  }
}

void
CGLApp::
enterCB(int state)
{
  int id = glutGetWindow();

  CGLWindow *window = CGLAppInst->lookupWindow(id);

  if (window != NULL) {
    if (state == GLUT_ENTERED)
      window->enterCB();
    else
      window->leaveCB();
  }
}

void
CGLApp::
idleCB()
{
  int id = glutGetWindow();

  CGLWindow *window = CGLAppInst->lookupWindow(id);

  if (window != NULL)
    window->idleCB();

  COSTimer::micro_sleep(CGLAppInst->msecs_);
}

#if 0
void
CGLApp::
shadowProjection(const CPoint3D &l, const CPoint3D &e, const CVector3D &n)
{
  double mat[16];

  double nl = n.getX()*l.getX() + n.getY()*l.getY() + n.getZ()*l.getZ();
  double en = e.getX()*n.getX() + e.getY()*n.getY() + e.getZ()*n.getZ();

  double c = en - nl;

  // Create the matrix. OpenGL uses column by column ordering

  mat[ 0] =  l.getX()*n.getX() + c;
  mat[ 4] =  l.getX()*n.getY();
  mat[ 8] =  l.getX()*n.getZ();
  mat[12] = -l.getX()*en;

  mat[ 1] =  l.getY()*n.getX();
  mat[ 5] =  l.getY()*n.getY() + c;
  mat[ 9] =  l.getY()*n.getZ();
  mat[13] = -l.getY()*en;

  mat[ 2] =  l.getZ()*n.getX();
  mat[ 6] =  l.getZ()*n.getY();
  mat[10] =  l.getZ()*n.getZ() + c;
  mat[14] = -l.getZ()*en;

  mat[ 3] = n.getX();
  mat[ 7] = n.getY();
  mat[11] = n.getZ();
  mat[15] = -nl;

  glMultMatrixd(mat);
}
#endif

#if 0
void
CGLApp::
shadowProjection(const CPoint3D &l, const CPoint3D &e, const CVector3D &n)
{
  CPlane3D pl(e, n);

  double proj[16];

  double a = pl.getNormal().getX();
  double b = pl.getNormal().getY();
  double c = pl.getNormal().getZ();
  double d = pl.getConstant();

  double dx = l.getX();
  double dy = l.getY();
  double dz = l.getZ();

  double dot = - (a * dx + b * dy + c * dz);

  // Now build the projection matrix
  proj[ 0] = a * dx + dot;
  proj[ 4] = b * dx;
  proj[ 8] = c * dx;
  proj[12] = d * dx;

  proj[ 1] = a * dy;
  proj[ 5] = b * dy + dot;
  proj[ 9] = c * dy;
  proj[13] = d * dy;

  proj[ 2] = a * dz;
  proj[ 6] = b * dz;
  proj[10] = c * dz + dot;
  proj[14] = d * dz;

  proj[ 3] = 0.0;
  proj[ 7] = 0.0;
  proj[11] = 0.0;
  proj[15] = 0.0 + dot;

  glMultMatrixd(proj);
}
#endif

void
CGLApp::
shadowProjection(const CPoint3D &l, const CPoint3D &e, const CVector3D &n)
{
  CPlane3D pl(e, n);

  /* Find dot product between light position vector and ground plane normal. */

  double dot = pl.getNormal().getX()*l.getX() +
               pl.getNormal().getY()*l.getY() +
               pl.getNormal().getZ()*l.getZ() +
               pl.getConstant();

  double mat[16];

  mat[ 0] = dot - l.getX()*pl.getNormal().getX();
  mat[ 4] = 0.0 - l.getX()*pl.getNormal().getY();
  mat[ 8] = 0.0 - l.getX()*pl.getNormal().getZ();
  mat[12] = 0.0 - l.getX()*pl.getConstant();

  mat[ 1] = 0.0 - l.getY()*pl.getNormal().getX();
  mat[ 5] = dot - l.getY()*pl.getNormal().getY();
  mat[ 9] = 0.0 - l.getY()*pl.getNormal().getZ();
  mat[13] = 0.0 - l.getY()*pl.getConstant();

  mat[ 2] = 0.0 - l.getZ()*pl.getNormal().getX();
  mat[ 6] = 0.0 - l.getZ()*pl.getNormal().getY();
  mat[10] = dot - l.getZ()*pl.getNormal().getZ();
  mat[14] = 0.0 - l.getZ()*pl.getConstant();

  mat[ 3] = 0.0 -          pl.getNormal().getX();
  mat[ 7] = 0.0 -          pl.getNormal().getY();
  mat[11] = 0.0 -          pl.getNormal().getZ();
  mat[15] = dot -          pl.getConstant();

  glMultMatrixd(mat);
}

bool
CGLApp::
isExtensionSupported(const char *extension) const
{
  // make sure extension name is valid
  if (! extension || extension[0] == '\0' || strchr(extension, ' ') != NULL)
    return 0;

  int len = strlen(extension);

  const char *extensions = (const char *) glGetString(GL_EXTENSIONS);

  const char *start = extensions;

  for (;;) {
    const char *where = strstr(start, extension);

    if (! where)
      break;

    const char *terminator = &where[len];

    if (where == start || where[-1] == ' ') {
      if (*terminator == ' ' || *terminator == '\0')
        return 1;
    }

    start = terminator;
  }

  return 0;
}

void
CGLApp::
displayErrors() const
{
  GLenum err = glGetError();

  if (err != GL_NO_ERROR)
    std::cerr << "OpenGL Error: " << gluErrorString(err) << std::endl;
}

void
CGLApp::
toMatrix(const double values[16], CMatrix3DH &matrix)
{
  matrix.setValues(values[0], values[4], values[ 8], values[12],
                   values[1], values[5], values[ 9], values[13],
                   values[2], values[6], values[10], values[14],
                   values[3], values[7], values[11], values[15]);
}

void
CGLApp::
fromMatrix(const CMatrix3DH &matrix, double values[16])
{
  matrix.getValues(&values[0], &values[4], &values[ 8], &values[12],
                   &values[1], &values[5], &values[ 9], &values[13],
                   &values[2], &values[6], &values[10], &values[14],
                   &values[3], &values[7], &values[11], &values[15]);
}
