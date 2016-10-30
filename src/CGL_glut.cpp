#include <CGL_glut.h>
#include <CGL.h>
#include <CGLMenu.h>
#include <CGLWind.h>
#include <CStrUtil.h>
#include <CStrParse.h>
#include <CTimer.h>
#include <COSTime.h>
#include <CArgs.h>
#include <CRGBA.h>
#include <CFuncs.h>
#include <CMathMacros.h>
#include <CMathGen.h>
#include <CPoint3D.h>
#include <CXMachine.h>
#include <CXScreen.h>
#include <bool_ops.h>
#include <climits>

#define CGLGlutInst CGLGlut::getInstance()

class CGLGlut {
 private:
  int display_mode_;

 public:
  static CGLGlut *getInstance() {
    static CGLGlut *instance;

    if (! instance)
      instance = new CGLGlut;

    return instance;
  }

 ~CGLGlut() { }

  ACCESSOR(DisplayMode, int, display_mode)

 private:
  CGLGlut() :
   display_mode_(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH) {
  }
};

static GLfloat
CGL_glut_cube_normal[6][3] = {
  {-1.0,  0.0,  0.0},
  { 0.0,  1.0,  0.0},
  { 1.0,  0.0,  0.0},
  { 0.0, -1.0,  0.0},
  { 0.0,  0.0,  1.0},
  { 0.0,  0.0, -1.0}
};

static GLint
CGL_glut_cube_faces[6][4] = {
  {0, 1, 2, 3},
  {3, 2, 6, 7},
  {7, 6, 5, 4},
  {4, 5, 1, 0},
  {5, 6, 2, 1},
  {7, 4, 0, 3}
};

#define GLUT_BITMAP_8_BY_13_NAME \
  "-misc-fixed-medium-r-normal--13-120-75-75-C-80-iso8859-1"
#define GLUT_BITMAP_9_BY_15_NAME \
  "-misc-fixed-medium-r-normal--15-140-75-75-C-90-iso8859-1"
#define GLUT_BITMAP_HELVETICA_10_NAME \
  "-adobe-helvetica-medium-r-normal--10-100-75-75-p-60-iso8859-1"
#define GLUT_BITMAP_HELVETICA_12_NAME \
  "-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1"
#define GLUT_BITMAP_HELVETICA_18_NAME \
  "-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso8859-1"
#define GLUT_BITMAP_TIMES_ROMAN_10_NAME \
  "-adobe-times-medium-r-normal--10-100-75-75-p-54-iso8859-1"
#define GLUT_BITMAP_TIMES_ROMAN_24_NAME \
  "-adobe-times-medium-r-normal--24-240-75-75-p-124-iso8859-1"

#ifndef GLUT_STROKE_ROMAN
# define GLUT_STROKE_ROMAN      ((void *) &glutStrokeRoman)
# define GLUT_STROKE_MONO_ROMAN ((void *) &glutStrokeMonoRoman)
#endif

void *glutBitmap8By13        = NULL;
void *glutBitmap9By15        = NULL;
void *glutBitmapHelvetica10  = NULL;
void *glutBitmapHelvetica12  = NULL;
void *glutBitmapHelvetica18  = NULL;
void *glutBitmapTimesRoman10 = NULL;
void *glutBitmapTimesRoman24 = NULL;
void *glutStrokeRoman        = NULL;
void *glutStrokeMonoRoman    = NULL;

static CFontPtr glutBitmap8By13Font;
static CFontPtr glutBitmap9By15Font;
static CFontPtr glutBitmapHelvetica10Font;
static CFontPtr glutBitmapHelvetica12Font;
static CFontPtr glutBitmapHelvetica18Font;
static CFontPtr glutBitmapTimesRoman10Font;
static CFontPtr glutBitmapTimesRoman24Font;

static CGLMenu *glut_menu = NULL;

static CHRTime glut_start_time;

void
glutAddMenuEntry(const char *label, int value)
{
  if (glut_menu != NULL)
    new CGLMenuButton(glut_menu, label, value);
}

void
glutAddSubMenu(const char * /*label*/, int /*subMenu*/)
{
  CGLMgrInst->unimplemented("glutAddSubMenu");
}

void
glutAttachMenu(int button)
{
  if (glut_menu != NULL)
    CGLMgrInst->getCurrentWindow()->setMenu(button, glut_menu->getId());
}

void
glutBitmapCharacter(void *font, int character)
{
  CFontPtr pfont;

  if (font == GLUT_BITMAP_8_BY_13) {
    if (! glutBitmap8By13Font)
      glutBitmap8By13Font =
        CFontMgrInst->lookupFont(GLUT_BITMAP_8_BY_13_NAME);

    pfont = glutBitmap8By13Font;
  }
  else if (font == GLUT_BITMAP_9_BY_15) {
    if (! glutBitmap9By15Font)
      glutBitmap9By15Font =
        CFontMgrInst->lookupFont(GLUT_BITMAP_9_BY_15_NAME);

    pfont = glutBitmap9By15Font;
  }
  else if (font == GLUT_BITMAP_TIMES_ROMAN_10) {
    if (! glutBitmapTimesRoman10Font)
      glutBitmapTimesRoman10Font =
        CFontMgrInst->lookupFont(GLUT_BITMAP_TIMES_ROMAN_10_NAME);

    pfont = glutBitmapTimesRoman10Font;
  }
  else if (font == GLUT_BITMAP_TIMES_ROMAN_24) {
    if (! glutBitmapTimesRoman24Font)
      glutBitmapTimesRoman24Font =
        CFontMgrInst->lookupFont(GLUT_BITMAP_TIMES_ROMAN_24_NAME);

    pfont = glutBitmapTimesRoman24Font;
  }
  else if (font == GLUT_BITMAP_HELVETICA_10) {
    if (! glutBitmapHelvetica10Font)
      glutBitmapHelvetica10Font =
        CFontMgrInst->lookupFont(GLUT_BITMAP_HELVETICA_10_NAME);

    pfont = glutBitmapHelvetica10Font;
  }
  else if (font == GLUT_BITMAP_HELVETICA_12) {
    if (! glutBitmapHelvetica12Font)
      glutBitmapHelvetica12Font =
        CFontMgrInst->lookupFont(GLUT_BITMAP_HELVETICA_12_NAME);

    pfont = glutBitmapHelvetica12Font;
  }
  else if (font == GLUT_BITMAP_HELVETICA_18) {
    if (! glutBitmapHelvetica18Font)
      glutBitmapHelvetica18Font =
        CFontMgrInst->lookupFont(GLUT_BITMAP_HELVETICA_18_NAME);

    pfont = glutBitmapHelvetica18Font;
  }
  else {
    assert(false);

    return;
  }

  CGL *gl = CGLMgrInst->getCurrentGL();

  gl->setFont(pfont);

  gl->drawChar(character);
}

void
glutButtonBoxFunc(void (*)(int button, int state))
{
  CGLMgrInst->unimplemented("glutButtonBoxFunc");
}

void
glutChangeToMenuEntry(int /*item*/, const char* /*label*/, int /*value*/)
{
  CGLMgrInst->unimplemented("glutChangeToMenuEntry");
}

void
glutChangeToSubMenu(int /*item*/, const char* /*label*/, int /*value*/)
{
  CGLMgrInst->unimplemented("glutChangeToSubMenu");
}

void
glutCopyColormap(int /*win*/)
{
  CGLMgrInst->unimplemented("glutCopyColormap");
}

int
glutCreateMenu(void (*callback)(int menu))
{
  glut_menu = new CGLMenu(callback);

  return glut_menu->getId();
}

int
glutCreateSubWindow(int window, int x, int y, int width, int height)
{
  CGLMgrInst->createSubWindow(window, x, y, width, height);

  return CGLMgrInst->getCurrentWindowId();
}

int
glutCreateWindow(const char *name)
{
  CGLMgrInst->createWindow(name);

  return CGLMgrInst->getCurrentWindowId();
}

void
glutDetachMenu(int id)
{
  CGLMgrInst->getCurrentWindow()->setMenu(-1, id);
}

void
glutDestroyMenu(int id)
{
  CGLMenu *menu = CGLMenu::lookupMenu(id);

  delete menu;
}

void
glutDestroyWindow(int window)
{
  CGLMgrInst->deleteWindow(window);
}

int
glutDeviceGet(GLenum info)
{
  switch (info) {
    case GLUT_HAS_KEYBOARD:
      return 1;
    case GLUT_HAS_MOUSE:
      return 1;
    case GLUT_HAS_SPACEBALL:
      return 0;
    case GLUT_HAS_DIAL_AND_BUTTON_BOX:
      return 0;
    case GLUT_HAS_TABLET:
      return 0;
    case GLUT_NUM_MOUSE_BUTTONS:
      return 3;
    case GLUT_NUM_SPACEBALL_BUTTONS:
      return 0;
    case GLUT_NUM_BUTTON_BOX_BUTTONS:
      return 0;
    case GLUT_NUM_DIALS:
      return 0;
    case GLUT_NUM_TABLET_BUTTONS:
      return 0;
    default:
      CGLMgrInst->setErrorNum(GL_INVALID_ENUM);
      return 0;
  }
}

void
glutDialsFunc(void (*)(int x, int y))
{
  CGLMgrInst->unimplemented("glutDialsFunc");
}

void
glutDisplayFunc(void (*func)())
{
  CGLMgrInst->getCurrentWindow()->setDisplayFunc(func);
}

void
glutEntryFunc(void (*)(int state))
{
  CGLMgrInst->unimplemented("glutEntryFunc");
}

void
glutEstablishOverlay()
{
  CGLMgrInst->unimplemented("glutEstablishOverlay");
}

int
glutExtensionSupported(const char *extension)
{
  if      (strcmp(extension, "GL_ARB_depth_texture") == 0)
    return true;
  else if (strcmp(extension, "GL_ARB_shadow") == 0)
    return true;
  else {
    CGLMgrInst->unimplemented("glutExtensionSupported");

    return false;
  }
}

void
glutFullScreen()
{
  CGLMgrInst->unimplemented("glutFullScreen");
}

int
glutGet(GLenum query)
{
  switch (query) {
    case GLUT_WINDOW_X:
      return CGLMgrInst->getWindowPosition().getX();
    case GLUT_WINDOW_Y:
      return CGLMgrInst->getWindowPosition().getY();
    case GLUT_WINDOW_WIDTH:
      return CGLMgrInst->getWindowSize().getWidth();
    case GLUT_WINDOW_HEIGHT:
      return CGLMgrInst->getWindowSize().getHeight();
    case GLUT_WINDOW_BUFFER_SIZE:
      return 32;
    case GLUT_WINDOW_STENCIL_SIZE:
      return 8;
    case GLUT_WINDOW_DEPTH_SIZE:
      return 8;
    case GLUT_WINDOW_RED_SIZE:
      return 8;
    case GLUT_WINDOW_GREEN_SIZE:
      return 8;
    case GLUT_WINDOW_BLUE_SIZE:
      return 8;
    case GLUT_WINDOW_ALPHA_SIZE:
      return 8;
    case GLUT_WINDOW_ACCUM_RED_SIZE:
      return 8;
    case GLUT_WINDOW_ACCUM_GREEN_SIZE:
      return 9;
    case GLUT_WINDOW_ACCUM_BLUE_SIZE:
      return 8;
    case GLUT_WINDOW_ACCUM_ALPHA_SIZE:
      return 8;
    case GLUT_WINDOW_DOUBLEBUFFER:
      return 1;
    case GLUT_WINDOW_RGBA:
      return 1;
    case GLUT_WINDOW_PARENT:
      CGLMgrInst->unimplemented("glutGet GLUT_WINDOW_PARENT");
      return 0;
    case GLUT_WINDOW_NUM_CHILDREN:
      CGLMgrInst->unimplemented("glutGet GLUT_WINDOW_NUM_CHILDREN");
      return 0;
    case GLUT_WINDOW_COLORMAP_SIZE:
      return 0;
    case GLUT_WINDOW_NUM_SAMPLES:
      CGLMgrInst->unimplemented("glutGet GLUT_WINDOW_NUM_SAMPLES");
      return 0;
    case GLUT_WINDOW_STEREO:
      return 0;
    case GLUT_WINDOW_CURSOR:
      return CGLMgrInst->getCurrentWindow()->getCursor();
    case GLUT_SCREEN_WIDTH:
      return CXMachineInst->getCXScreen(0)->getWidth();
    case GLUT_SCREEN_HEIGHT:
      return CXMachineInst->getCXScreen(0)->getHeight();
    case GLUT_SCREEN_WIDTH_MM:
      CGLMgrInst->unimplemented("glutGet GLUT_SCREEN_WIDTH_MM");
      return 0;
    case GLUT_SCREEN_HEIGHT_MM:
      CGLMgrInst->unimplemented("glutGet GLUT_SCREEN_HEIGHT_MM");
      return 0;
    case GLUT_MENU_NUM_ITEMS: {
      if (glut_menu)
        return glut_menu->getNumItems();
      else
        return 0;
    }
    case GLUT_DISPLAY_MODE_POSSIBLE:
      CGLMgrInst->unimplemented("glutGet GLUT_DISPLAY_MODE_POSSIBLE");
      return 0;
    case GLUT_INIT_DISPLAY_MODE:
      return CGLGlutInst->getDisplayMode();
      return 0;
    case GLUT_INIT_WINDOW_X:
      return CGLMgrInst->getWindowRect().getXMin();
    case GLUT_INIT_WINDOW_Y:
      return CGLMgrInst->getWindowRect().getYMin();
    case GLUT_INIT_WINDOW_WIDTH:
      return CGLMgrInst->getWindowRect().getWidth();
    case GLUT_INIT_WINDOW_HEIGHT:
      return CGLMgrInst->getWindowRect().getHeight();
    case GLUT_ELAPSED_TIME: {
      CHRTime t = COSTime::getHRTime();

      CHRTime dt = COSTime::diffHRTime(glut_start_time, t);

      return int(1000*dt.secs + dt.usecs*0.001);
    }
    case GLUT_WINDOW_FORMAT_ID:
      CGLMgrInst->unimplemented("glutGet GLUT_WINDOW_FORMAT_ID");
      return 0;
    default:
      CGLMgrInst->setErrorNum(GL_INVALID_ENUM);
      return 0;
  }
}

GLfloat
glutGetColor(int /*cell*/, int /*component*/)
{
  CGLMgrInst->unimplemented("glutGetColor");

  return 0.0;
}

int
glutGetMenu()
{
  if (glut_menu)
    return glut_menu->getId();
  else
    return 0;
}

int
glutGetModifiers()
{
  CGLMgrInst->unimplemented("glutGetModifiers");

  return CMODIFIER_NONE;
}

void *
glutGetProcAddress(const char * /*procName*/)
{
  CGLMgrInst->unimplemented("glutGetProcAddress");

  return NULL;
}

int
glutGetWindow()
{
  return CGLMgrInst->getCurrentWindowId();
}

void
glutHideOverlay()
{
  CGLMgrInst->unimplemented("glutHideOverlay");
}

void
glutHideWindow()
{
  CGLMgrInst->getCurrentWindow()->hide();
}

void
glutIconifyWindow()
{
  CGLMgrInst->getCurrentWindow()->iconize();
}

void
glutIdleFunc(void (*func)())
{
  CGLMgrInst->getCurrentWindow()->setIdleFunc(func);
}

void
glutInit(int *argc, char **argv)
{
  CArgs args("-display:s -geometry:s -indirect:f -iconic:f");

  args.parse(argc, argv);

  if (args.isStringArgSet("-geometry")) {
    CStrParse parse(args.getStringArg("-geometry"));

    bool ok   = true;
    bool size = false;
    bool pos  = false;

    int x, y, w, h;

    if (! parse.isChar('+')) {
      size = true;

      ok = parse.readInteger(&w);

      if (ok && parse.isChar('x')) {
        parse.skipChar();

        ok = parse.readInteger(&h);
      }
      else
        ok = false;
    }

    if (ok && parse.isChar('+')) {
      pos = true;

      ok = parse.readInteger(&x);

      if (ok && parse.isChar('+')) {
        parse.skipChar();

        ok = parse.readInteger(&y);
      }
      else
        ok = false;
    }

    if (ok) {
      if (pos)
        glutInitWindowPosition(x, y);

      if (size)
        glutInitWindowSize(w, h);
    }
    else
      std::cerr << "Invalid geometry " << args.getStringArg("-geometry") << std::endl;
  }

  glut_start_time = COSTime::getHRTime();

  CTimerMgrInst->setExternalUpdate(true);
}

void
glutInitDisplayMode(uint mode)
{
  CGLGlutInst->setDisplayMode(mode);

  if (mode & GLUT_SINGLE)
    mode &= ~GLUT_SINGLE;

  if (mode & GLUT_DOUBLE)
    mode &= ~GLUT_DOUBLE;

  if (mode & GLUT_RGB)
    mode &= ~GLUT_RGB;

  if (mode & GLUT_RGBA)
    mode &= ~GLUT_RGBA;

  if (mode & GLUT_DEPTH)
    mode &= ~GLUT_DEPTH;

  if (mode & GLUT_ACCUM)
    mode &= ~GLUT_ACCUM;

  if (mode & GLUT_STENCIL)
    mode &= ~GLUT_STENCIL;

  if (mode != 0)
    CGLMgrInst->unimplemented(CStrUtil::strprintf("glutInitDisplayMode: %d", mode));
}

void
glutInitWindowPosition(int x, int y)
{
  CGLMgrInst->setWindowPosition(CIPoint2D(x, y));
}

void
glutInitWindowSize(int width, int height)
{
  CGLMgrInst->setWindowSize(CISize2D(width, height));
}

void
glutKeyboardFunc(void (*func)(uchar key, int x, int y))
{
  CGLMgrInst->getCurrentWindow()->setKeyboardFunc(func);
}

void
glutKeyboardUpFunc(void (*)(uchar key, int x, int y))
{
  CGLMgrInst->unimplemented("glutKeyboardUpFunc");
}

int
glutLayerGet(GLenum info)
{
  switch (info) {
    case GLUT_OVERLAY_POSSIBLE:
      return 0;
    case GLUT_LAYER_IN_USE:
      return 0;
    case GLUT_HAS_OVERLAY:
      return 0;
    case GLUT_TRANSPARENT_INDEX:
      return 0;
    case GLUT_NORMAL_DAMAGED:
      return 0;
    case GLUT_OVERLAY_DAMAGED:
      return 0;
    default:
      CGLMgrInst->setErrorNum(GL_INVALID_ENUM);
      return 0;
  }
}

void
glutMainLoop()
{
  CGLMgrInst->mainLoop();
}

void
glutMenuStateFunc(void (*)(int))
{
  // deprecated version of glutMenuStatusFunc
  CGLMgrInst->unimplemented("glutMenuStateFunc");
}

void
glutMenuStatusFunc(void (*)(int status, int x, int y))
{
  CGLMgrInst->unimplemented("glutMenuStatusFunc");
}

void
glutMouseFunc(void (*func)(int button, int state, int x, int y))
{
  CGLMgrInst->getCurrentWindow()->setMouseFunc(func);
}

void
glutMotionFunc(void (*func)(int x, int y))
{
  CGLMgrInst->getCurrentWindow()->setMotionFunc(func);
}

void
glutOverlayDisplayFunc(void (*)())
{
  CGLMgrInst->unimplemented("glutOverlayDisplayFunc");
}

void
glutPassiveMotionFunc(void (*)(int x, int y))
{
  CGLMgrInst->unimplemented("glutPassiveMotionFunc");
}

void
glutPopWindow()
{
  CGLMgrInst->getCurrentWindow()->raise();
}

void
glutPositionWindow(int x, int y)
{
  CGLMgrInst->setWindowPosition(CIPoint2D(x, y));
}

void
glutPostOverlayRedisplay()
{
  CGLMgrInst->unimplemented("glutPostOverlayRedisplay");
}

void
glutPostRedisplay()
{
  CGLWind *window = CGLMgrInst->getCurrentWindow();

  if (window)
    window->setRedisplay(true);
}

void
glutPushWindow()
{
  CGLMgrInst->getCurrentWindow()->lower();
}

void
glutRemoveMenuItem(int /*item*/)
{
  CGLMgrInst->unimplemented("glutRemoveMenuItem");
}

void
glutRemoveOverlay()
{
  CGLMgrInst->unimplemented("glutRemoveOverlay");
}

void
glutReshapeFunc(void (*func)(int width, int height))
{
  CGLMgrInst->getCurrentWindow()->setResizeFunc(func);
}

void
glutReshapeWindow(int width, int height)
{
  CGLMgrInst->setWindowSize(CISize2D(width, height));
}

void
glutSetColor(int /*cell*/, GLfloat /*red*/, GLfloat /*green*/, GLfloat /*blue*/)
{
  CGLMgrInst->unimplemented("glutSetColor");
}

void
glutSetCursor(int cursor)
{
  if (! CGLMgrInst->getCurrentWindow()->setCursor(cursor))
    CGLMgrInst->setErrorNum(GL_INVALID_ENUM);
}

void
glutSetIconTitle(const char *title)
{
  CGLMgrInst->getCurrentWindow()->setIconTitle(title);
}

void
glutSetMenu(int menu)
{
  glut_menu = CGLMenu::lookupMenu(menu);
}

void
glutSetWindow(int win)
{
  CGLMgrInst->setCurrentWindowId(win);
}

void
glutSetWindowTitle(const char *title)
{
  CGLMgrInst->getCurrentWindow()->setTitle(title);
}

void
glutShowOverlay()
{
  CGLMgrInst->unimplemented("glutShowOverlay");
}

void
glutShowWindow()
{
  CGLMgrInst->getCurrentWindow()->show();
}

void
glutSpaceballMotionFunc(void (*)(int x, int y, int z))
{
  CGLMgrInst->unimplemented("glutSpaceballMotionFunc");
}

void
glutSpaceballRotateFunc(void (*)(int x, int y, int z))
{
  CGLMgrInst->unimplemented("glutSpaceballRotateFunc");
}

void
glutSpaceballButtonFunc(void (* )(int button, int state))
{
  CGLMgrInst->unimplemented("glutSpaceballButtonFunc");
}

void
glutSpecialFunc(void (*func)(int key, int x, int y))
{
  CGLMgrInst->getCurrentWindow()->setSpecialFunc(func);
}

void
glutSpecialUpFunc(void (*)(int key, int x, int y))
{
  CGLMgrInst->unimplemented("glutSpecialUpFunc");
}

void
glutStrokeCharacter(void * /*font*/, int /*character*/)
{
  CGLMgrInst->unimplemented("glutStrokeCharacter");
}

void
glutSwapBuffers()
{
  CGLMgrInst->getCurrentWindow()->flushEvents();
}

void
glutTabletMotionFunc(void (*)(int x, int y))
{
  CGLMgrInst->unimplemented("glutTabletMotionFunc");
}

void
glutTabletButtonFunc(void (*)(int button, int state, int x, int y))
{
  CGLMgrInst->unimplemented("glutTabletButtonFunc");
}

void
glutTimerFunc(uint msecs, void (*func)(int), int value)
{
  CGLMgrInst->getCurrentWindow()->addTimerFunc(msecs, func, value);
}

void
glutUseLayer(GLenum /*layer*/)
{
  CGLMgrInst->unimplemented("glutUseLayer");
}

void
glutVisibilityFunc(void (*func)(int state))
{
  CGLMgrInst->getCurrentWindow()->setVisibilityFunc(func);
}

//------------------

void
glutSolidCube(GLdouble size)
{
  GLfloat v[8][3];

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  size / 2;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  size / 2;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  size / 2;

  for (GLint i = 5; i >= 0; i--) {
    glBegin(GL_POLYGON);

    glNormal3fv(&CGL_glut_cube_normal[i][0]);

    glVertex3fv(&v[CGL_glut_cube_faces[i][0]][0]);
    glVertex3fv(&v[CGL_glut_cube_faces[i][1]][0]);
    glVertex3fv(&v[CGL_glut_cube_faces[i][2]][0]);
    glVertex3fv(&v[CGL_glut_cube_faces[i][3]][0]);

    glEnd();
  }
}

void
glutWireCube(GLdouble size)
{
  GLfloat v[8][3];

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  size / 2;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  size / 2;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  size / 2;

  for (GLint i = 5; i >= 0; i--) {
    glBegin(GL_LINE_LOOP);

    glNormal3fv(&CGL_glut_cube_normal[i][0]);

    glVertex3fv(&v[CGL_glut_cube_faces[i][0]][0]);
    glVertex3fv(&v[CGL_glut_cube_faces[i][1]][0]);
    glVertex3fv(&v[CGL_glut_cube_faces[i][2]][0]);
    glVertex3fv(&v[CGL_glut_cube_faces[i][3]][0]);

    glEnd();
  }
}

//------------------

void
glutAddBodyRevX(int mode, double *x, double *y, int num_xy, int num_patches)
{
  static const double EPSILON_E6 = 1E-6;

  std::vector<double> c; c.resize(num_patches);
  std::vector<double> s; s.resize(num_patches);

  double theta           = 0.0;
  double theta_increment = 2.0*M_PI/num_patches;

  for (int i = 0; i < num_patches; i++) {
    c[i] = cos(theta);
    s[i] = sin(theta);

    theta += theta_increment;
  }

  int num_points = 0;

  std::vector<int> index1; index1.resize(num_patches + 1);
  std::vector<int> index2; index2.resize(num_patches + 1);

  int *pindex1 = &index1[0];
  int *pindex2 = &index2[0];

  std::vector<CPoint3D> points ; points .resize(num_patches*num_xy);
  std::vector<CPoint3D> normals; normals.resize(num_patches*num_xy);

  if (fabs(x[0]) < EPSILON_E6) {
    points [num_points] = CPoint3D(0.0, y[0], 0.0);
    normals[num_points] = CPoint3D(0.0, y[0], 0.0);

    for (int i = 0; i <= num_patches; i++)
      pindex1[i] = num_points;

    num_points++;
  }
  else {
    for (int i = 0; i < num_patches; i++) {
      points [num_points] = CPoint3D(x[0]*c[i], y[0], -x[0]*s[i]);
      normals[num_points] = CPoint3D(x[0]*c[i], y[0], -x[0]*s[i]);

      pindex1[i] = num_points;

      num_points++;
    }

    pindex1[num_patches] = pindex1[0];
  }

  for (int j = 1; j < num_xy; j++) {
    if (fabs(x[j]) < EPSILON_E6) {
      points [num_points] = CPoint3D(0.0, y[j], 0.0);
      normals[num_points] = CPoint3D(0.0, y[j], 0.0);

      for (int i = 0; i <= num_patches; i++)
        pindex2[i] = num_points;

      num_points++;
    }
    else {
      for (int i = 0; i < num_patches; i++) {
        points [num_points] = CPoint3D(x[j]*c[i], y[j], -x[j]*s[i]);
        normals[num_points] = CPoint3D(x[j]*c[i], y[j], -x[j]*s[i]);

        pindex2[i] = num_points;

        num_points++;
      }

      pindex2[num_patches] = pindex2[0];
    }

    if (pindex1[0] != pindex1[1]) {
      if (pindex2[0] == pindex2[1]) {
        for (int i = 0; i < num_patches; i++) {
          glBegin(mode);

          glNormal3d(normals[pindex1[i + 1]].x,
                     normals[pindex1[i + 1]].z,
                     normals[pindex1[i + 1]].y);
          glVertex3d(points [pindex1[i + 1]].x,
                     points [pindex1[i + 1]].z,
                     points [pindex1[i + 1]].y);

          glNormal3d(normals[pindex2[i    ]].x,
                     normals[pindex2[i    ]].z,
                     normals[pindex2[i    ]].y);
          glVertex3d(points [pindex2[i    ]].x,
                     points [pindex2[i    ]].z,
                     points [pindex2[i    ]].y);

          glNormal3d(normals[pindex1[i    ]].x,
                     normals[pindex1[i    ]].z,
                     normals[pindex1[i    ]].y);
          glVertex3d(points [pindex1[i    ]].x,
                     points [pindex1[i    ]].z,
                     points [pindex1[i    ]].y);

          glEnd();
        }
      }
      else {
        for (int i = 0; i < num_patches; i++) {
          glBegin(mode);

          glNormal3d(normals[pindex1[i + 1]].x,
                     normals[pindex1[i + 1]].z,
                     normals[pindex1[i + 1]].y);
          glVertex3d(points [pindex1[i + 1]].x,
                     points [pindex1[i + 1]].z,
                     points [pindex1[i + 1]].y);

          glNormal3d(normals[pindex2[i + 1]].x,
                     normals[pindex2[i + 1]].z,
                     normals[pindex2[i + 1]].y);
          glVertex3d(points [pindex2[i + 1]].x,
                     points [pindex2[i + 1]].z,
                     points [pindex2[i + 1]].y);

          glNormal3d(normals[pindex2[i    ]].x,
                     normals[pindex2[i    ]].z,
                     normals[pindex2[i    ]].y);
          glVertex3d(points [pindex2[i    ]].x,
                     points [pindex2[i    ]].z,
                     points [pindex2[i    ]].y);

          glNormal3d(normals[pindex1[i    ]].x,
                     normals[pindex1[i    ]].z,
                     normals[pindex1[i    ]].y);
          glVertex3d(points [pindex1[i    ]].x,
                     points [pindex1[i    ]].z,
                     points [pindex1[i    ]].y);

          glEnd();
        }
      }
    }
    else {
      if (pindex2[0] != pindex2[1]) {
        for (int i = 0; i < num_patches; i++) {
          glBegin(mode);

          glNormal3d(normals[pindex2[i + 1]].x,
                     normals[pindex2[i + 1]].z,
                     normals[pindex2[i + 1]].y);
          glVertex3d(points [pindex2[i + 1]].x,
                     points [pindex2[i + 1]].z,
                     points [pindex2[i + 1]].y);

          glNormal3d(normals[pindex2[i    ]].x,
                     normals[pindex2[i    ]].z,
                     normals[pindex2[i    ]].y);
          glVertex3d(points [pindex2[i    ]].x,
                     points [pindex2[i    ]].z,
                     points [pindex2[i    ]].y);

          glNormal3d(normals[pindex1[i    ]].x,
                     normals[pindex1[i    ]].z,
                     normals[pindex1[i    ]].y);
          glVertex3d(points [pindex1[i    ]].x,
                     points [pindex1[i    ]].z,
                     points [pindex1[i    ]].y);

          glEnd();
        }
      }
    }

    int *pindex = pindex2;

    pindex2 = pindex1;
    pindex1 = pindex;
  }
}

void
glutAddBodyRevY(int mode, double *x, double *y, int num_xy, int num_patches)
{
  static const double EPSILON_E6 = 1E-6;

  std::vector<double> c; c.resize(num_patches);
  std::vector<double> s; s.resize(num_patches);

  double theta           = 0.0;
  double theta_increment = 2.0*M_PI/num_patches;

  for (int i = 0; i < num_patches; i++) {
    c[i] = cos(theta);
    s[i] = sin(theta);

    theta += theta_increment;
  }

  int num_points = 0;

  std::vector<int> index1; index1.resize(num_patches + 1);
  std::vector<int> index2; index2.resize(num_patches + 1);

  int *pindex1 = &index1[0];
  int *pindex2 = &index2[0];

  std::vector<CPoint3D> points ; points .resize(num_patches*num_xy);
  std::vector<CPoint3D> normals; normals.resize(num_patches*num_xy);

  if (fabs(y[0]) < EPSILON_E6) {
    points [num_points] = CPoint3D(x[0], 0.0, 0.0);
    normals[num_points] = CPoint3D(x[0], 0.0, 0.0);

    for (int i = 0; i <= num_patches; i++)
      pindex1[i] = num_points;

    num_points++;
  }
  else {
    for (int i = 0; i < num_patches; i++) {
      points [num_points] = CPoint3D(x[0], y[0]*c[i], -y[0]*s[i]);
      normals[num_points] = CPoint3D(x[0], y[0]*c[i], -y[0]*s[i]);

      pindex1[i] = num_points;

      num_points++;
    }

    pindex1[num_patches] = pindex1[0];
  }

  for (int j = 1; j < num_xy; j++) {
    if (fabs(y[j]) < EPSILON_E6) {
      points [num_points] = CPoint3D(x[j], 0.0, 0.0);
      normals[num_points] = CPoint3D(x[j], 0.0, 0.0);

      for (int i = 0; i <= num_patches; i++)
        pindex2[i] = num_points;

      num_points++;
    }
    else {
      for (int i = 0; i < num_patches; i++) {
        points [num_points] = CPoint3D(x[j], y[j]*c[i], -y[j]*s[i]);
        normals[num_points] = CPoint3D(x[j], y[j]*c[i], -y[j]*s[i]);

        pindex2[i] = num_points;

        num_points++;
      }

      pindex2[num_patches] = pindex2[0];
    }

    if (pindex1[0] != pindex1[1]) {
      if (pindex2[0] == pindex2[1]) {
        for (int i = 0; i < num_patches; i++) {
          glBegin(mode);

          glNormal3d(normals[pindex1[i + 1]].x,
                     normals[pindex1[i + 1]].z,
                     normals[pindex1[i + 1]].y);
          glVertex3d(points [pindex1[i + 1]].x,
                     points [pindex1[i + 1]].z,
                     points [pindex1[i + 1]].y);

          glNormal3d(normals[pindex2[i    ]].x,
                     normals[pindex2[i    ]].z,
                     normals[pindex2[i    ]].y);
          glVertex3d(points [pindex2[i    ]].x,
                     points [pindex2[i    ]].z,
                     points [pindex2[i    ]].y);

          glNormal3d(normals[pindex1[i    ]].x,
                     normals[pindex1[i    ]].z,
                     normals[pindex1[i    ]].y);
          glVertex3d(points [pindex1[i    ]].x,
                     points [pindex1[i    ]].z,
                     points [pindex1[i    ]].y);

          glEnd();
        }
      }
      else {
        for (int i = 0; i < num_patches; i++) {
          glBegin(mode);

          glNormal3d(normals[pindex1[i + 1]].x,
                     normals[pindex1[i + 1]].z,
                     normals[pindex1[i + 1]].y);
          glVertex3d(points [pindex1[i + 1]].x,
                     points [pindex1[i + 1]].z,
                     points [pindex1[i + 1]].y);

          glNormal3d(normals[pindex2[i + 1]].x,
                     normals[pindex2[i + 1]].z,
                     normals[pindex2[i + 1]].y);
          glVertex3d(points [pindex2[i + 1]].x,
                     points [pindex2[i + 1]].z,
                     points [pindex2[i + 1]].y);

          glNormal3d(normals[pindex2[i    ]].x,
                     normals[pindex2[i    ]].z,
                     normals[pindex2[i    ]].y);
          glVertex3d(points [pindex2[i    ]].x,
                     points [pindex2[i    ]].z,
                     points [pindex2[i    ]].y);

          glNormal3d(normals[pindex1[i    ]].x,
                     normals[pindex1[i    ]].z,
                     normals[pindex1[i    ]].y);
          glVertex3d(points [pindex1[i    ]].x,
                     points [pindex1[i    ]].z,
                     points [pindex1[i    ]].y);

          glEnd();
        }
      }
    }
    else {
      if (pindex2[0] != pindex2[1]) {
        for (int i = 0; i < num_patches; i++) {
          glBegin(mode);

          glNormal3d(normals[pindex2[i + 1]].x,
                     normals[pindex2[i + 1]].z,
                     normals[pindex2[i + 1]].y);
          glVertex3d(points [pindex2[i + 1]].x,
                     points [pindex2[i + 1]].z,
                     points [pindex2[i + 1]].y);

          glNormal3d(normals[pindex2[i    ]].x,
                     normals[pindex2[i    ]].z,
                     normals[pindex2[i    ]].y);
          glVertex3d(points [pindex2[i    ]].x,
                     points [pindex2[i    ]].z,
                     points [pindex2[i    ]].y);

          glNormal3d(normals[pindex1[i    ]].x,
                     normals[pindex1[i    ]].z,
                     normals[pindex1[i    ]].y);
          glVertex3d(points [pindex1[i    ]].x,
                     points [pindex1[i    ]].z,
                     points [pindex1[i    ]].y);

          glEnd();
        }
      }
    }

    int *pindex = pindex2;

    pindex2 = pindex1;
    pindex1 = pindex;
  }
}

//------------------

void
glutSphere(uint mode, GLdouble radius, GLint slices, GLint stacks)
{
  std::vector<double> x; x.resize(stacks);
  std::vector<double> y; y.resize(stacks);

  double a = -M_PI*0.5;

  double da = M_PI/(stacks - 1);

  for (int i = 0; i < stacks; ++i) {
    x[i] = radius*cos(a);
    y[i] = radius*sin(a);

    a += da;
  }

  glutAddBodyRevX(mode, &x[0], &y[0], stacks, slices);
}

void
glutSolidSphere(GLdouble radius, GLint slices, GLint stacks)
{
  glutSphere(GL_POLYGON, radius, slices, stacks);
}

void
glutWireSphere(GLdouble radius, GLint slices, GLint stacks)
{
  glutSphere(GL_LINE_LOOP, radius, slices, stacks);
}

//------------------

void
glutCone(uint mode, GLdouble base, GLdouble height, GLint slices, GLint stacks)
{
  std::vector<double> x; x.resize(stacks);
  std::vector<double> y; x.resize(stacks);

  x[0         ] = base; y[0         ] = 0;
  x[stacks - 1] = 0   ; y[stacks - 1] = height;

  double dx = stacks > 2 ? (x[stacks - 1] - x[0])/(stacks - 1) : 0;
  double dy = stacks > 2 ? (y[stacks - 1] - y[0])/(stacks - 1) : 0;

  for (int i = 1; i < stacks - 1; ++i) {
    x[i] = x[i - 1] + dx;
    y[i] = y[i - 1] + dy;
  }

  glutAddBodyRevX(mode, &x[0], &y[0], stacks, slices);
}

void
glutWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)
{
  glutCone(GL_LINE_LOOP, base, height, slices, stacks);
}

void
glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)
{
  glutCone(GL_POLYGON, base, height, slices, stacks);
}

//------------------

static void
glutRecordItem(const CVector3D &n1, const CVector3D &n2,
               const CVector3D &n3, uint shadeType)
{
  CVector3D q0(n1 - n2);
  CVector3D q1(n2 - n3);

  q1 = q0.crossProduct(q1).normalized();

  glBegin(shadeType);

  glNormal3dv(q1.getValues());

  glVertex3dv(n1.getValues());
  glVertex3dv(n2.getValues());
  glVertex3dv(n3.getValues());

  glEnd();
}

static void
glutSubDivide(const CVector3D &v0, const CVector3D &v1,
              const CVector3D &v2, uint shadeType)
{
  CVector3D w0, w1, w2;

  int depth = 1;

  for (int i = 0; i < depth; i++) {
    for (int j = 0; i + j < depth; j++) {
      int k = depth - i - j;

      w0 = CVector3D( i     *v0 +  j     *v1 +  k     *v2).normalize();
      w1 = CVector3D((i + 1)*v0 +  j     *v1 + (k - 1)*v2).normalize();
      w2 = CVector3D( i     *v0 + (j + 1)*v1 + (k - 1)*v2).normalize();

      glutRecordItem(w1, w0, w2, shadeType);
    }
  }
}

static void
glutDrawTriangle(int i, double data[][3], int ndx[][3], uint shadeType)
{
  double *x0 = data[ndx[i][0]];
  double *x1 = data[ndx[i][1]];
  double *x2 = data[ndx[i][2]];

  glutSubDivide(CVector3D(x0[0], x0[1], x0[2]),
                CVector3D(x1[0], x1[1], x1[2]),
                CVector3D(x2[0], x2[1], x2[2]),
                shadeType);
}

//------------------

/* octahedron data: The octahedron produced is centered at the
   origin and has radius 1.0 */
static double
glutOctahedronData[6][3] =
{
  { 1.0,  0.0,  0.0},
  {-1.0,  0.0,  0.0},
  { 0.0,  1.0,  0.0},
  { 0.0, -1.0,  0.0},
  { 0.0,  0.0,  1.0},
  { 0.0,  0.0, -1.0}
};

static int
glutOctahedronIndex[8][3] =
{
  {0, 4, 2},
  {1, 2, 4},
  {0, 3, 4},
  {1, 4, 3},
  {0, 2, 5},
  {1, 5, 2},
  {0, 5, 3},
  {1, 3, 5}
};

static void
glutOctahedron(GLenum shadeType)
{
  for (int i = 0; i < 8; i++)
    glutDrawTriangle(i, glutOctahedronData, glutOctahedronIndex, shadeType);
}

void
glutWireOctahedron()
{
  glutOctahedron(GL_LINE_LOOP);
}

void
glutSolidOctahedron()
{
  glutOctahedron(GL_TRIANGLES);
}

//------------------

#define SQRT_3 1.73205080756887729

static double
glutTetrahedronData[4][3] = {
  { SQRT_3,  SQRT_3,  SQRT_3},
  { SQRT_3, -SQRT_3, -SQRT_3},
  {-SQRT_3,  SQRT_3, -SQRT_3},
  {-SQRT_3, -SQRT_3,  SQRT_3}
};

static int
glutTetrahedronIndex[4][3] = {
  {0, 1, 3},
  {2, 1, 0},
  {3, 2, 0},
  {1, 2, 3}
};

void
glutTetrahedron(GLenum shadeType)
{
  for (int i = 0; i < 4; ++i)
    glutDrawTriangle(i, glutTetrahedronData, glutTetrahedronIndex, shadeType);
}

void
glutWireTetrahedron()
{
  glutTetrahedron(GL_LINE_LOOP);
}

void
glutSolidTetrahedron()
{
  glutTetrahedron(GL_TRIANGLES);
}

//------------------

void
glutTorus(GLdouble r, GLdouble R, GLint nsides, GLint rings, GLenum type)
{
  GLdouble p0[3], p1[3], p2[3], p3[3];
  GLdouble n0[3], n1[3], n2[3], n3[3];

  for (int i = 0; i < rings; i++) {
    GLdouble theta  = (GLdouble)  i     *2.0*M_PI/rings;
    GLdouble theta1 = (GLdouble) (i + 1)*2.0*M_PI/rings;

    for (int j = 0; j < nsides; j++) {
      GLdouble phi  = (GLdouble)  j     *2.0*M_PI/nsides;
      GLdouble phi1 = (GLdouble) (j + 1)*2.0*M_PI/nsides;

      p0[0] =  cos(theta )*(R + r*cos(phi));
      p0[1] = -sin(theta )*(R + r*cos(phi));
      p0[2] = r*sin(phi);

      p1[0] =  cos(theta1)*(R + r*cos(phi));
      p1[1] = -sin(theta1)*(R + r*cos(phi));
      p1[2] = r*sin(phi);

      p2[0] =  cos(theta1)*(R + r*cos(phi1));
      p2[1] = -sin(theta1)*(R + r*cos(phi1));
      p2[2] = r*sin(phi1);

      p3[0] =  cos(theta )*(R + r*cos(phi1));
      p3[1] = -sin(theta )*(R + r*cos(phi1));
      p3[2] = r*sin(phi1);

      n0[0] =  cos(theta)*(cos(phi));
      n0[1] = -sin(theta)*(cos(phi));
      n0[2] = sin(phi);

      n1[0] =  cos(theta1)*(cos(phi));
      n1[1] = -sin(theta1)*(cos(phi));
      n1[2] = sin(phi);

      n2[0] =  cos(theta1)*(cos(phi1));
      n2[1] = -sin(theta1)*(cos(phi1));
      n2[2] = sin(phi1);

      n3[0] =  cos(theta)*(cos(phi1));
      n3[1] = -sin(theta)*(cos(phi1));
      n3[2] = sin(phi1);

      glBegin(type);

      glNormal3dv(n3); glVertex3dv(p3);
      glNormal3dv(n2); glVertex3dv(p2);
      glNormal3dv(n1); glVertex3dv(p1);
      glNormal3dv(n0); glVertex3dv(p0);

      glEnd();
    }
  }
}

void
glutWireTorus(GLdouble innerRadius, GLdouble outerRadius,
              GLint nsides, GLint rings)
{
  glutTorus(innerRadius, outerRadius, nsides, rings, GL_LINE_LOOP);
}

void
glutSolidTorus(GLdouble innerRadius, GLdouble outerRadius,
               GLint nsides, GLint rings)
{
  glutTorus(innerRadius, outerRadius, nsides, rings, GL_QUADS);
}

//------------------

static CVector3D dodec[20];

static void
glutInitDodecahedron(void)
{
  double alpha = sqrt(2.0 / (3.0 + sqrt(5.0)));

  double beta = 1.0 + sqrt(6.0 / (3.0 + sqrt(5.0)) -
                2.0 + 2.0 * sqrt(2.0 / (3.0 + sqrt(5.0))));

  dodec[ 0] = CVector3D( -alpha,      0,   beta);
  dodec[ 1] = CVector3D(  alpha,      0,   beta);
  dodec[ 2] = CVector3D(     -1,     -1,     -1);
  dodec[ 3] = CVector3D(     -1,     -1,      1);
  dodec[ 4] = CVector3D(     -1,      1,     -1);
  dodec[ 5] = CVector3D(     -1,      1,      1);
  dodec[ 6] = CVector3D(      1,     -1,     -1);
  dodec[ 7] = CVector3D(      1,     -1,      1);
  dodec[ 8] = CVector3D(      1,      1,     -1);
  dodec[ 9] = CVector3D(      1,      1,      1);
  dodec[10] = CVector3D(   beta,  alpha,      0);
  dodec[11] = CVector3D(   beta, -alpha,      0);
  dodec[12] = CVector3D(  -beta,  alpha,      0);
  dodec[13] = CVector3D(  -beta, -alpha,      0);
  dodec[14] = CVector3D( -alpha,      0,  -beta);
  dodec[15] = CVector3D(  alpha,      0,  -beta);
  dodec[16] = CVector3D(      0,   beta,  alpha);
  dodec[17] = CVector3D(      0,   beta, -alpha);
  dodec[18] = CVector3D(      0,  -beta,  alpha);
  dodec[19] = CVector3D(      0,  -beta, -alpha);
}

static void
glutPentagon(int a, int b, int c, int d, int e, GLenum shadeType)
{
  CVector3D d1 = dodec[a] - dodec[b];
  CVector3D d2 = dodec[b] - dodec[c];

  CVector3D n0 = d1.crossProduct(d2).normalized();

  glBegin(shadeType);

  glNormal3dv(n0.getValues());

  glVertex3dv(dodec[a].getValues());
  glVertex3dv(dodec[b].getValues());
  glVertex3dv(dodec[c].getValues());
  glVertex3dv(dodec[d].getValues());
  glVertex3dv(dodec[e].getValues());

  glEnd();
}

static void
glutDodecahedron(GLenum type)
{
  static bool inited = 0;

  if (! inited) {
    inited = true;

    glutInitDodecahedron();
  }

  glutPentagon( 0,  1,  9, 16,  5, type);
  glutPentagon( 1,  0,  3, 18,  7, type);
  glutPentagon( 1,  7, 11, 10,  9, type);
  glutPentagon(11,  7, 18, 19,  6, type);
  glutPentagon( 8, 17, 16,  9, 10, type);
  glutPentagon( 2, 14, 15,  6, 19, type);
  glutPentagon( 2, 13, 12,  4, 14, type);
  glutPentagon( 2, 19, 18,  3, 13, type);
  glutPentagon( 3,  0,  5, 12, 13, type);
  glutPentagon( 6, 15,  8, 10, 11, type);
  glutPentagon( 4, 17,  8, 15, 14, type);
  glutPentagon( 4, 12,  5, 16, 17, type);
}

void
glutWireDodecahedron()
{
  glutDodecahedron(GL_LINE_LOOP);
}

void
glutSolidDodecahedron()
{
  glutDodecahedron(GL_TRIANGLE_FAN);
}

//------------------

/* icosahedron data: These numbers are rigged to make an
   icosahedron of radius 1.0 */

#define X 0.525731112119133606
#define Z 0.850650808352039932

static double
icosahedron_idata[12][3] =
{
  {-X,  0,  Z},
  { X,  0,  Z},
  {-X,  0, -Z},
  { X,  0, -Z},
  { 0,  Z,  X},
  { 0,  Z, -X},
  { 0, -Z,  X},
  { 0, -Z, -X},
  { Z,  X,  0},
  {-Z,  X,  0},
  { Z, -X,  0},
  {-Z, -X,  0}
};

static int
icosahedron_index[20][3] =
{
  { 0,  4,  1},
  { 0,  9,  4},
  { 9,  5,  4},
  { 4,  5,  8},
  { 4,  8,  1},
  { 8, 10,  1},
  { 8,  3, 10},
  { 5,  3,  8},
  { 5,  2,  3},
  { 2,  7,  3},
  { 7, 10,  3},
  { 7,  6, 10},
  { 7, 11,  6},
  {11,  0,  6},
  { 0,  1,  6},
  { 6,  1, 10},
  { 9,  0, 11},
  { 9, 11,  2},
  { 9,  2,  5},
  { 7,  2, 11},
};

static void
glutIcosahedron(GLenum shadeType)
{
  for (int i = 0; i < 20; i++)
    glutDrawTriangle(i, icosahedron_idata, icosahedron_index, shadeType);
}

void
glutWireIcosahedron(void)
{
  glutIcosahedron(GL_LINE_LOOP);
}

void
glutSolidIcosahedron(void)
{
  glutIcosahedron(GL_TRIANGLES);
}

//------------------

/* Rim, body, lid, and bottom data must be reflected in x
   and y; handle and spout data across the y axis only.  */

static long
teapot_patchdata[][16] = {
  /* rim */
  {102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  /* body */
  {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
  {24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40},
  /* lid */
  {96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,},
  {0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117},
  /* bottom */
  {118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120, 40, 39, 38, 37},
  /* handle */
  {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56},
  {53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67},
  /* spout */
  {68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83},
  {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95}
};

static float
teapot_cpdata[][3] = {
  { 0.2   ,  0     , 2.7},
  { 0.2   , -0.112 , 2.7},
  { 0.112 , -0.2   , 2.7},
  { 0     , -0.2   , 2.7},
  { 1.3375,  0     , 2.53125},
  { 1.3375, -0.749 , 2.53125},
  { 0.749 , -1.3375, 2.53125},
  { 0     , -1.3375, 2.53125},
  { 1.4375,  0     , 2.53125},
  { 1.4375, -0.805 , 2.53125},
  { 0.805 , -1.4375, 2.53125},
  { 0     , -1.4375, 2.53125},
  { 1.5   ,  0     , 2.4},
  { 1.5   , -0.84  , 2.4},
  { 0.84  , -1.5   , 2.4},
  { 0     , -1.5   , 2.4},
  { 1.75  ,  0     , 1.875},
  { 1.75  , -0.98  , 1.875},
  { 0.98  , -1.75  , 1.875},
  { 0     , -1.75  , 1.875},
  { 2     ,  0     , 1.35},
  { 2     , -1.12  , 1.35},
  { 1.12  , -2     , 1.35},
  { 0     , -2     , 1.35},
  { 2     ,  0     , 0.9},
  { 2     , -1.12  , 0.9},
  { 1.12  , -2     , 0.9},
  { 0     , -2     , 0.9},
  {-2     ,  0     , 0.9},
  { 2     ,  0     , 0.45},
  { 2     , -1.12  , 0.45},
  { 1.12  , -2     , 0.45},
  { 0     , -2     , 0.45},
  { 1.5   ,  0     , 0.225},
  { 1.5   , -0.84  , 0.225},
  { 0.84  , -1.5   , 0.225},
  { 0     , -1.5   , 0.225},
  { 1.5   ,  0     , 0.15},
  { 1.5   , -0.84  , 0.15},
  { 0.84  , -1.5   , 0.15},
  { 0     , -1.5   , 0.15},
  {-1.6   ,  0     , 2.025},
  {-1.6   , -0.3   , 2.025},
  {-1.5   , -0.3   , 2.25},
  {-1.5   ,  0     , 2.25},
  {-2.3   ,  0     , 2.025},
  {-2.3   , -0.3   , 2.025},
  {-2.5   , -0.3   , 2.25},
  {-2.5   ,  0     , 2.25},
  {-2.7   ,  0     , 2.025},
  {-2.7   , -0.3   , 2.025},
  {-3     , -0.3   , 2.25},
  {-3     ,  0     , 2.25},
  {-2.7   ,  0     , 1.8},
  {-2.7   , -0.3   , 1.8},
  {-3     , -0.3   , 1.8},
  {-3     ,  0     , 1.8},
  {-2.7   ,  0     , 1.575},
  {-2.7   , -0.3   , 1.575},
  {-3     , -0.3   , 1.35},
  {-3     ,  0     , 1.35},
  {-2.5   ,  0     , 1.125},
  {-2.5   , -0.3   , 1.125},
  {-2.65  , -0.3   , 0.9375},
  {-2.65  ,  0     , 0.9375},
  {-2     , -0.3   , 0.9},
  {-1.9   , -0.3   , 0.6},
  {-1.9   ,  0     , 0.6},
  { 1.7   ,  0     , 1.425},
  { 1.7   , -0.66  , 1.425},
  { 1.7   , -0.66  , 0.6},
  { 1.7   ,  0     , 0.6},
  { 2.6   ,  0     , 1.425},
  { 2.6   , -0.66  , 1.425},
  { 3.1   , -0.66  , 0.825},
  { 3.1   ,  0     , 0.825},
  { 2.3   ,  0     , 2.1},
  { 2.3   , -0.25  , 2.1},
  { 2.4   , -0.25  , 2.025},
  { 2.4   ,  0     , 2.025},
  { 2.7   ,  0     , 2.4},
  { 2.7   , -0.25  , 2.4},
  { 3.3   , -0.25  , 2.4},
  { 3.3   ,  0     , 2.4},
  { 2.8   ,  0     , 2.475},
  { 2.8   , -0.25  , 2.475},
  { 3.525 , -0.25  , 2.49375},
  { 3.525 ,  0     , 2.49375},
  { 2.9   ,  0     , 2.475  },
  { 2.9   , -0.15  , 2.475  },
  { 3.45  , -0.15  , 2.5125 },
  { 3.45  ,  0     , 2.5125 },
  { 2.8   ,  0     , 2.4    },
  { 2.8   , -0.15  , 2.4    },
  { 3.2   , -0.15  , 2.4    },
  { 3.2   ,  0     , 2.4    },
  { 0     ,  0     , 3.15   },
  { 0.8   ,  0     , 3.15   },
  { 0.8   , -0.45  , 3.15   },
  { 0.45  , -0.8   , 3.15   },
  { 0     , -0.8   , 3.15   },
  { 0     ,  0     , 2.85   },
  { 1.4   ,  0     , 2.4    },
  { 1.4   , -0.784 , 2.4    },
  { 0.784 , -1.4   , 2.4    },
  { 0     , -1.4   , 2.4    },
  { 0.4   ,  0     , 2.55   },
  { 0.4   , -0.224 , 2.55   },
  { 0.224 , -0.4   , 2.55   },
  { 0     , -0.4   , 2.55   },
  { 1.3   ,  0     , 2.55   },
  { 1.3   , -0.728 , 2.55   },
  { 0.728 , -1.3   , 2.55   },
  { 0     , -1.3   , 2.55   },
  { 1.3   ,  0     , 2.4    },
  { 1.3   , -0.728 , 2.4    },
  { 0.728 , -1.3   , 2.4    },
  { 0     , -1.3   , 2.4    },
  { 0     ,  0     , 0      },
  { 1.425 , -0.798 , 0      },
  { 1.5   ,  0     , 0.075  },
  { 1.425 ,  0     , 0      },
  { 0.798 , -1.425 , 0      },
  { 0     , -1.5   , 0.075  },
  { 0     , -1.425 , 0      },
  { 1.5   , -0.84  , 0.075  },
  { 0.84  , -1.5   , 0.075  }
};

static float
teapot_tex[2][2][2] =
{
  { {0, 0}, {1, 0} },
  { {0, 1}, {1, 1} }
};

static void
glutTeapot(GLint grid, GLdouble scale, GLenum type)
{
  float p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];

  long i, j, k, l;

  glPushAttrib(GL_ENABLE_BIT | GL_EVAL_BIT);

  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_MAP2_VERTEX_3);
  glEnable(GL_MAP2_TEXTURE_COORD_2);

  glPushMatrix();

  glRotatef   (270.0, 1.0, 0.0, 0.0);
  glScalef    (0.5*scale, 0.5*scale, 0.5*scale);
  glTranslatef(0.0, 0.0, -1.5);

  for (i = 0; i < 10; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 4; k++) {
        for (l = 0; l < 3; l++) {
          p[j][k][l] = teapot_cpdata[teapot_patchdata[i][j*4 +      k ]][l];
          q[j][k][l] = teapot_cpdata[teapot_patchdata[i][j*4 + (3 - k)]][l];

          if (l == 1)
            q[j][k][l] *= -1.0;

          if (i < 6) {
            r[j][k][l] = teapot_cpdata[teapot_patchdata[i][j*4 + (3 - k)]][l];

            if (l == 0)
              r[j][k][l] *= -1.0;

            s[j][k][l] = teapot_cpdata[teapot_patchdata[i][j*4 +      k ]][l];

            if (l == 0)
              s[j][k][l] *= -1.0;

            if (l == 1)
              s[j][k][l] *= -1.0;
          }
        }
      }
    }

    glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2,
            &teapot_tex[0][0][0]);

    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &p[0][0][0]);

    glMapGrid2f(grid, 0.0, 1.0, grid, 0.0, 1.0);

    glEvalMesh2(type, 0, grid, 0, grid);

    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &q[0][0][0]);

    glEvalMesh2(type, 0, grid, 0, grid);

    if (i < 6) {
      glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &r[0][0][0]);

      glEvalMesh2(type, 0, grid, 0, grid);

      glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &s[0][0][0]);

      glEvalMesh2(type, 0, grid, 0, grid);
    }
  }

  glPopMatrix();
  glPopAttrib();
}

void
glutSolidTeapot(GLdouble scale)
{
  glutTeapot(14, scale, GL_FILL);
}

void
glutWireTeapot(GLdouble scale)
{
  glutTeapot(10, scale, GL_LINE);
}

void
glutWindowStatusFunc(void (*)(int))
{
  CGLMgrInst->unimplemented("glutWindowStatusFunc");
}
