#include <CGLWindow.h>
#include <CGLApp.h>
#include <CGeomObject3D.h>
#include <CGLWidget.h>
#include <CImageLib.h>
#include <GL/glut.h>

#ifdef USE_XMASK
#include <GL/glx.h>
#include "X11/extensions/shape.h"
#endif

CWindow *
CGLWindowFactory::
createWindow(int x, int y, uint width, uint height)
{
  return CGLAppInst->createWindow(x, y,  width, height);
}

CWindow *
CGLWindowFactory::
createWindow(CWindow *parent, int x, int y, uint width, uint height)
{
  CGLWindow *gparent = dynamic_cast<CGLWindow *>(parent);

  assert(gparent);

  return gparent->addWindow(x, y, int(width), int(height));
}

//-------------

CGLWindow::
CGLWindow(CGLWindow *parent) :
 parent_(parent)
{
  disp_attr_ = std::make_unique<CGLDispAttr>();
}

CGLWindow::
~CGLWindow()
{
  glutDestroyWindow(id_);
}

void
CGLWindow::
init(int x, int y, uint width, uint height, const std::string &title)
{
  int id = 0;

  if (parent_ == NULL) {
    if (x >= 0 && y >= 0)
      glutInitWindowPosition(x, y);

    if (width > 0 && height > 0)
      glutInitWindowSize(int(width), int(height));

    id = glutCreateWindow(title.c_str());
  }
  else {
    int parentId = parent_->getId();

    id = glutCreateSubWindow(parentId, x, y, int(width), int(height));
  }

  setId(id);

  setSize(int(width), int(height));

  CGLAppInst->addWindow(this);

  //CGLLight *light = CGLLightMgrInst->createLight();

  //light->setAmbient(CRGBA(0.3, 0.3, 0.3));
  //light->setDiffuse(CRGBA(0.7, 0.7, 0.7));

  glEnable(GL_COLOR_MATERIAL);

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  //glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

  CRGBA ambient(0.4, 0.4, 0.4);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, CRGBAToFV(ambient).fvalues);

  glEnable(GL_NORMALIZE);

  glEnable(GL_LIGHT0);
}

void
CGLWindow::
setTitle(const std::string &title)
{
  setCurrent();

  glutSetWindowTitle(title.c_str());
}

CGLWindow *
CGLWindow::
addWindow(int x, int y, int width, int height)
{
  CGLWindow *window = CGLAppInst->newWindow(this);

  window->init(x, y, uint(width), uint(height));

  window->setup();

  return window;
}

CGLRenderer3D *
CGLWindow::
getRenderer() const
{
  if (! renderer_) {
    CGLWindow *th = const_cast<CGLWindow *>(this);

    th->renderer_ = std::make_unique<CGLRenderer3D>(th);

    th->renderer_->updateSize();
  }

  return renderer_.get();
}

CGLLight *
CGLWindow::
createLight()
{
  CGLLight *light = CGLLightMgrInst->createLight();

  light->setAmbient(CRGBA(0.3, 0.3, 0.3));
  light->setDiffuse(CRGBA(0.7, 0.7, 0.7));

  return light;
}

void
CGLWindow::
enableDispAttr()
{
  disp_attr_->setEnabled(true);
}

CGLDispAttr *
CGLWindow::
getDispAttr()
{
  return disp_attr_.get();
}

void
CGLWindow::
addControl(uint dim, CGLControl::Orientation orientation)
{
  if (control_ == NULL) {
    if (dim == 2)
      control_ = std::make_unique<CGLControl2D>(orientation);
    else {
      auto *control = new CGLControl3D;

      control_ = ControlP(control);
    }
  }
}

CGLControl *
CGLWindow::
getControl()
{
  return control_.get();
}

void
CGLWindow::
addWidget(CGLWidget *widget)
{
  widgetList_.push_back(widget);
}

//----------

CGLPixelRenderer *
CGLWindow::
getPixelRenderer() const
{
  return dynamic_cast<CGLPixelRenderer *>(getRenderer()->getPixelRenderer());
}

void
CGLWindow::
getPosition(int *x, int *y) const
{
  *x = pos_.x;
  *y = pos_.y;
}

void
CGLWindow::
getSize(uint *w, uint *h) const
{
  GLint values[4];

  glGetIntegerv(GL_VIEWPORT, values);

  *w = uint(values[2]);
  *h = uint(values[3]);
}

void
CGLWindow::
getScreenSize(uint *w, uint *h) const
{
  // TODO:
  *w = 1280;
  *h = 1024;
}

void
CGLWindow::
move(int x, int y)
{
  pos_ = CIPoint2D(x, y);

  setCurrent();

  glutPositionWindow(x, y);

  glFlush();
}

void
CGLWindow::
resize(uint w, uint h)
{
  reshape(int(w), int(h));
}

void
CGLWindow::
destroy()
{
}

void
CGLWindow::
map()
{
  setCurrent();

  glutShowWindow();
}

void
CGLWindow::
unmap()
{
  setCurrent();

  glutHideWindow();
}

bool
CGLWindow::
isMapped()
{
  // TODO:
  return true;
}

void
CGLWindow::
iconize()
{
  // TODO:
}

void
CGLWindow::
deiconize()
{
  // TODO:
}

void
CGLWindow::
maximize()
{
  // TODO:
}

void
CGLWindow::
demaximize()
{
  // TODO:
}

void
CGLWindow::
lower()
{
  setCurrent();

  glutPushWindow();
}

void
CGLWindow::
raise()
{
  setCurrent();

  glutPopWindow();
}

void
CGLWindow::
setWindowTitle(const std::string &)
{
  // TODO:
  setCurrent();
}

void
CGLWindow::
setIconTitle(const std::string &title)
{
  glutSetIconTitle(title.c_str());
}

void
CGLWindow::
getWindowTitle(std::string &title) const
{
  // TODO:
  title = "";
}

void
CGLWindow::
getIconTitle(std::string &title) const
{
  // TODO:
  title = "";
}

void
CGLWindow::
expose()
{
  // TODO
}

void
CGLWindow::
setEventAdapter(CEventAdapter *adapter)
{
  event_adapter_ = EventAdapterP(adapter);
}

bool
CGLWindow::
setSelectText(const std::string &)
{
  // TODO:
  return false;
}

bool
CGLWindow::
setProperty(const std::string &, const std::string &)
{
  // TODO:
  return false;
}

//----------

void
CGLWindow::
setId(int id)
{
  id_ = id;
}

void
CGLWindow::
reshape(int w, int h)
{
  setSize(w, h);

  setCurrent();

  glutReshapeWindow(size_.getWidth(), size_.getHeight());

  glFlush();
}

void
CGLWindow::
setSize(int w, int h)
{
  if (h == 0) h = 1;

  glViewport(0, 0, w, h);

  size_ = CISize2D(w, h);

  if (renderer_)
    renderer_->updateSize();
}

void
CGLWindow::
redisplay()
{
  setCurrent();

  glutPostRedisplay();

  glFlush();
}

void
CGLWindow::
initDisplay()
{
  glEnable(GL_ALPHA_TEST);
  //glAlphaFunc(GL_GREATER, 0.1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_LINE_STIPPLE);
  glLineStipple(1, 0xFF);

  glLineWidth(1.0);

  glClearColor(0, 0, 0, 1);
}

void
CGLWindow::
updateDisplay()
{
  //glFlush();

  glutSwapBuffers();
}

void
CGLWindow::
setCurrent()
{
  glutSetWindow(id_);
}

void
CGLWindow::
reshapeCB(int w, int h)
{
  setSize(w, h);

  if (control_)
    control_->reshape(w, h);

  CEventAdapter *event_adapter = getEventAdapter();

  if (event_adapter)
    event_adapter->resizeEvent();
  else
    resizeEvent();
}

void
CGLWindow::
displayCB()
{
  initDisplay();

  if (control_)
    control_->display();

  CEventAdapter *event_adapter = getEventAdapter();

  if (event_adapter)
    event_adapter->exposeEvent();
  else
    exposeEvent();

  updateDisplay();
}

bool
CGLWindow::
exposeEvent()
{
  drawScene();

  initOverlay();

  drawOverlay();

  termOverlay();

  return true;
}

void
CGLWindow::
drawScene()
{
}

void
CGLWindow::
initOverlay()
{
  glMatrixMode(GL_MODELVIEW ); glPushMatrix();
  glMatrixMode(GL_PROJECTION); glPushMatrix();

  //----

  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();

  uint w = uint(glutGet(GLUT_WINDOW_WIDTH ));
  uint h = uint(glutGet(GLUT_WINDOW_HEIGHT));

  glOrtho(0.0, w, h, 0.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
}

void
CGLWindow::
termOverlay()
{
  glMatrixMode(GL_PROJECTION); glPopMatrix();
  glMatrixMode(GL_MODELVIEW ); glPopMatrix();
}

void
CGLWindow::
drawOverlay()
{
  drawWidgets();
}

void
CGLWindow::
drawWidgets()
{
  WidgetList::const_iterator p1, p2;

  for (p1 = widgetList_.begin(), p2 = widgetList_.end(); p1 != p2; ++p1) {
    if (! (*p1)->getVisible()) continue;

    (*p1)->exposeEvent();
  }
}

void
CGLWindow::
buttonPressCB(const CMouseEvent &event)
{
  CEventAdapter *event_adapter = getEventAdapter();

  if (getControl())
    getControl()->mousePress(event);

  if (event_adapter)
    event_adapter->buttonPressEvent(event);
  else
    buttonPressEvent(event);

  WidgetList::const_iterator p1, p2;

  for (p1 = widgetList_.begin(), p2 = widgetList_.end(); p1 != p2; ++p1) {
    if (! (*p1)->getVisible()) continue;

    (*p1)->buttonPressEvent(event);
  }
}

void
CGLWindow::
buttonReleaseCB(const CMouseEvent &event)
{
  CEventAdapter *event_adapter = getEventAdapter();

  if (getControl())
    getControl()->mouseRelease(event);

  if (event_adapter)
    event_adapter->buttonReleaseEvent(event);
  else
    buttonReleaseEvent(event);
}

void
CGLWindow::
buttonMotionCB(const CMouseEvent &event)
{
  CEventAdapter *event_adapter = getEventAdapter();

  if (getControl())
    getControl()->mouseMotion(event);

  if (event_adapter)
    event_adapter->buttonMotionEvent(event);
  else
    buttonMotionEvent(event);
}

void
CGLWindow::
keyPressCB(const CKeyEvent &event)
{
  CEventAdapter *event_adapter = getEventAdapter();

  if (event_adapter)
    event_adapter->keyPressEvent(event);
  else {
    if (disp_attr_)
      disp_attr_->keyPress(event);

    keyPressEvent(event);
  }
}

void
CGLWindow::
keyReleaseCB(const CKeyEvent &event)
{
  CEventAdapter *event_adapter = getEventAdapter();

  if (event_adapter)
    event_adapter->keyReleaseEvent(event);
  else
    keyReleaseEvent(event);
}

void
CGLWindow::
enterCB()
{
  CEventAdapter *event_adapter = getEventAdapter();

  if (event_adapter)
    event_adapter->enterEvent();
  else
    enterEvent();
}

void
CGLWindow::
leaveCB()
{
  CEventAdapter *event_adapter = getEventAdapter();

  if (event_adapter)
    event_adapter->leaveEvent();
  else
    leaveEvent();
}

void
CGLWindow::
idleCB()
{
  CEventAdapter *event_adapter = getEventAdapter();

  if (event_adapter)
    event_adapter->idleEvent();
  else
    idleEvent();
}

void
CGLWindow::
hideCB()
{
  CEventAdapter *event_adapter = getEventAdapter();

  if (event_adapter)
    event_adapter->hideEvent();
  else
    hideEvent();
}

void
CGLWindow::
showCB()
{
  CEventAdapter *event_adapter = getEventAdapter();

  if (event_adapter)
    event_adapter->showEvent();
  else
    showEvent();
}

int
CGLWindow::
pickScene(const CMouseEvent &e)
{
  int x = e.getX();
  int y = e.getY();

  // flip y
  y = glutGet(GLUT_WINDOW_HEIGHT) - 1 - y;

  int delX = 3;
  int delY = 3;

  GLuint buffer[1024];

  const int bufferSize = sizeof(buffer)/sizeof(GLuint);

  GLint    viewport[4];
  GLdouble projection[16];

  glSelectBuffer(bufferSize, buffer); /* Selection buffer for hit records */

  glRenderMode(GL_SELECT); /* OpenGL selection mode */

  glInitNames(); /* Clear OpenGL name stack */

  glPushName(0);

  glMatrixMode(GL_PROJECTION);

  glPushMatrix(); /* Push current projection matrix */

  glGetIntegerv(GL_VIEWPORT, viewport); /* Get the current viewport size  */

  glGetDoublev(GL_PROJECTION_MATRIX, projection); /* Get projection matrix */

  glLoadIdentity(); /* Reset the projection matrix */

  gluPickMatrix(x, y, delX, delY, viewport); /* Set picking matrix */

  glMultMatrixd(projection); /* Apply projection matrix */

  glMatrixMode(GL_MODELVIEW);

  //--------

  in_select_ = true;

  drawScene();

  in_select_ = false;

  //--------

  GLint hits = glRenderMode(GL_RENDER); /* Return to normal rendering mode  */

  /* Diagnostic output to stdout */

#if 0
  if (hits != 0) {
    printf("hits = %d\n", hits);

    for (uint i = 0, j = 0; i < uint(hits); ++i) {
      printf("\tsize = %u, min = %u, max = %u : ",
             buffer[j], buffer[j + 1], buffer[j + 2]);

      for (int k = 0; k < (int) buffer[j]; ++k)
        printf("%u ", buffer[j + 3 + k]);

      printf("\n");

      j += 3 + buffer[j];
    }
  }
#endif

  /* Determine the nearest hit */

  GLint  min  = -1;
  GLuint minZ = uint(-1);

  if (hits) {
    for (uint i = 0, j = 0; i < uint(hits); ++i) {
      if (buffer[j + 1] < minZ) {
        /* If name stack is empty, return -1                */
        /* If name stack is not empty, return top-most name */

        if (buffer[j] == 0)
          min = -1;
        else
          min = int(buffer[j + 2 + buffer[j]]);

        minZ = buffer[j + 1];
      }

      j += buffer[j] + 3;
    }
  }

  glMatrixMode(GL_PROJECTION);

  glPopMatrix(); /* Restore projection matrix */

  glMatrixMode(GL_MODELVIEW);

  pick_name_ = min;

  return pick_name_;
}

int
CGLWindow::
pickOverlay(const CMouseEvent &e)
{
  int x = e.getX();
  int y = e.getY();

  initOverlay();

  // flip y
  y = glutGet(GLUT_WINDOW_HEIGHT) - 1 - y;

  int delX = 3;
  int delY = 3;

  GLuint buffer[1024];

  const int bufferSize = sizeof(buffer)/sizeof(GLuint);

  GLint    viewport[4];
  GLdouble projection[16];

  glSelectBuffer(bufferSize, buffer); /* Selection buffer for hit records */

  glRenderMode(GL_SELECT); /* OpenGL selection mode */

  glInitNames(); /* Clear OpenGL name stack */

  glPushName(0);

  glMatrixMode(GL_PROJECTION);

  glPushMatrix(); /* Push current projection matrix */

  glGetIntegerv(GL_VIEWPORT, viewport); /* Get the current viewport size  */

  glGetDoublev(GL_PROJECTION_MATRIX, projection); /* Get projection matrix */

  glLoadIdentity(); /* Reset the projection matrix */

  gluPickMatrix(x, y, delX, delY, viewport); /* Set picking matrix */

  glMultMatrixd(projection); /* Apply projection matrix */

  glMatrixMode(GL_MODELVIEW);

  //--------

  in_select_ = true;

  drawOverlay();

  in_select_ = false;

  //--------

  GLint hits = glRenderMode(GL_RENDER); /* Return to normal rendering mode  */

  /* Diagnostic output to stdout */

#if 0
  if (hits != 0) {
    printf("hits = %d\n", hits);

    for (int i = 0, j = 0; i < hits; ++i) {
      printf("\tsize = %u, min = %u, max = %u : ",
             buffer[j], buffer[j + 1], buffer[j + 2]);

      for (int k = 0; k < (int) buffer[j]; ++k)
        printf("%u ", buffer[j + 3 + k]);

      printf("\n");

      j += 3 + buffer[j];
    }
  }
#endif

  /* Determine the nearest hit */

  GLint  min  = -1;
  GLuint minZ = uint(-1);

  if (hits) {
    for (uint i = 0, j = 0; i < uint(hits); ++i) {
      if (buffer[j + 1] < minZ) {
        /* If name stack is empty, return -1                */
        /* If name stack is not empty, return top-most name */

        if (buffer[j] == 0)
          min = -1;
        else
          min = int(buffer[uint(j + 2 + buffer[j])]);

        minZ = buffer[j + 1];
      }

      j += buffer[j] + 3;
    }
  }

  glMatrixMode(GL_PROJECTION);

  glPopMatrix(); /* Restore projection matrix */

  glMatrixMode(GL_MODELVIEW);

  pick_name_ = min;

  termOverlay();

  if (pick_name_ == CGLWidgetMgrInst->getId()) {
    CGLWidgetMgrInst->getRoot()->buttonPressEvent(e);

    return -1;
  }

  return pick_name_;
}

//--------

void
CGLWindow::
setShapeFromImage(CImagePtr image)
{
#ifdef USE_XMASK
  uint w, h;

  getSize(&w, &h);

  CImagePtr image1 = image->resize(int(w), int(h));

  Pixmap mask;

  Display *display = glXGetCurrentDisplay();
  Window   xwindow = glXGetCurrentDrawable();

  mask = XCreatePixmap(display, xwindow, w, h, 1);

  GC gc = XCreateGC(display, mask, 0, nullptr);

  XSetForeground(display, gc, 0);

  XFillRectangle(display, mask, gc, 0, 0, w, h);

  XSetForeground(display, gc, 1);

  double alpha;

  for (uint y = 0, ind = 0; y < h; ++y) {
    for (uint x = 0; x < w; ++x, ++ind) {
      alpha = image1->getAlpha(int(ind));

      if (alpha >= 0.5)
        XDrawPoint(display, mask, gc, int(x), int(y));
    }
  }

  XFreeGC(display, gc);

  XShapeCombineMask(display, xwindow, ShapeBounding, 0, 0, mask, ShapeSet);
#else
  assert(image);
#endif
}

//---------

void
CGLWindow::
addObject(CGeomObject3D &object, const CMatrix3D &matrix)
{
  glDisable(GL_COLOR_MATERIAL);

  glColor3d(0.5, 0.5, 0.5);

  CMatrix3D nmatrix = matrix.inverse().transposed();

  CMatrix3D vmatrix(CMatrix3D::Type::IDENTITY);

  const CGeomObject3D::FaceList &faces = object.getFaces();

  auto pf1 = faces.begin();
  auto pf2 = faces.end  ();

  for ( ; pf1 != pf2; ++pf1) {
    const CGeomFace3D *face = *pf1;

    const CGeomFace3D::VertexList &vertices = face->getVertices();

    auto pv1 = vertices.begin();
    auto pv2 = vertices.end  ();

    for ( ; pv1 != pv2; ++pv1) {
      CGeomVertex3D &vertex = object.getVertex(*pv1);

      vertex.place(matrix);
      vertex.view (vmatrix);
    }

    const CRGBA &ambient   = face->getMaterial().getAmbient  ();
    const CRGBA &diffuse   = face->getMaterial().getDiffuse  ();
    const CRGBA &specular  = face->getMaterial().getSpecular ();
    double       shininess = face->getMaterial().getShininess();

    CVector3D normal;

    if (face->getNormalSet())
      normal = nmatrix*face->getNormal();
    else
      normal = object.verticesNormal(vertices);

    bool two_sided = face->getTwoSided();

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, CRGBAToFV(ambient).fvalues);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, CRGBAToFV(diffuse).fvalues);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, CRGBAToFV(specular).fvalues);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, float(shininess));

    glColor3d(diffuse.getRed(), diffuse.getGreen(), diffuse.getBlue());

    if (! two_sided) {
      glBegin(GL_POLYGON);

      pv1 = vertices.begin();

      for ( ; pv1 != pv2; ++pv1) {
        const CGeomVertex3D &vertex = object.getVertex(*pv1);

        const CPoint3D &point = vertex.getViewed();

        if (vertex.hasNormal()) {
          CVector3D normal1 = nmatrix*vertex.getNormal();

          glNormal3d(normal1.getX(), normal1.getY(), normal1.getZ());
        }
        else
          glNormal3d(normal.getX(), normal.getY(), normal.getZ());

        glVertex3d(point.x, point.y, point.z);
      }

      glEnd();
    }
    else {
      glBegin(GL_POLYGON);

      pv1 = vertices.begin();

      for ( ; pv1 != pv2; ++pv1) {
        const CGeomVertex3D &vertex = object.getVertex(*pv1);

        const CPoint3D &point = vertex.getViewed();

        CVector3D normal1;

        if (vertex.hasNormal())
          normal1 = nmatrix*vertex.getNormal();
        else
          normal1 = normal;

        glNormal3d(normal1.getX(), normal1.getY(), normal1.getZ());

        glVertex3d(point.x, point.y, point.z);
      }

      glEnd();

      glBegin(GL_POLYGON);

      auto prv1 = vertices.rbegin();
      auto prv2 = vertices.rend  ();

      for ( ; prv1 != prv2; ++prv1) {
        const CGeomVertex3D &vertex = object.getVertex(*prv1);

        const CPoint3D &point = vertex.getViewed();

        CVector3D normal1;

        if (vertex.hasNormal())
          normal1 = nmatrix*vertex.getNormal();
        else
          normal1 = -1*normal;

        glNormal3d(normal1.getX(), normal1.getY(), normal1.getZ());

        glVertex3d(point.x, point.y, point.z);
      }

      glEnd();
    }
  }

  glEnable(GL_COLOR_MATERIAL);

  //-----

  const CGeomObject3D::LineList &lines = object.getLines();

  auto pl1 = lines.begin();
  auto pl2 = lines.end  ();

  for ( ; pl1 != pl2; ++pl1) {
    CGeomVertex3D vertex1 = (*pl1)->getStartVertex();
    CGeomVertex3D vertex2 = (*pl1)->getEndVertex  ();

    glBegin(GL_LINES);

    const CRGBA &color = (*pl1)->getMaterial().getDiffuse();

    vertex1.place(matrix);
    vertex1.view (vmatrix);

    vertex2.place(matrix);
    vertex2.view (vmatrix);

    const CPoint3D &point1 = vertex1.getViewed();
    const CPoint3D &point2 = vertex2.getViewed();

    glColor3d(color.getRed(), color.getGreen(), color.getBlue());

    glVertex3d(point1.x, point1.y, point1.z);
    glVertex3d(point2.x, point2.y, point2.z);

    glEnd();
  }

  CGLAppInst->displayErrors();
}

#if 0
void
CGLWindow::
addWireframeObject(CGeomObject3D &object, const CMatrix3D &matrix)
{
  CMatrix3D vmatrix(CMATRIX_TYPE_IDENTITY);

  const CGeomObject3D::FaceList &faces = object.getFaces();

  auto pf1 = faces.begin();
  auto pf2 = faces.end  ();

  for ( ; pf1 != pf2; ++pf1) {
    const CGeomFace3D::VertexList &vertices = (*pf1)->getVertices();

    glBegin(GL_LINE_LOOP);

    const CRGBA &color = (*pf1)->getMaterial().getDiffuse();

    auto pv1 = vertices.begin();
    auto pv2 = vertices.end  ();

    for ( ; pv1 != pv2; ++pv1) {
      object.getVertex(*pv1).place(matrix);
      object.getVertex(*pv1).view(vmatrix);
    }

    //CVector3D normal = object.verticesNormal(vertices);

    pv1 = vertices.begin();

    for ( ; pv1 != pv2; ++pv1) {
      const CPoint3D &point = object.getVertex(*pv1).getViewed();

      glColor3d(color.getRed(), color.getGreen(), color.getBlue());

      //glNormal3d(normal.getX(), normal.getY(), normal.getZ());

      glVertex3d(point.x, point.y, point.z);
    }

    glEnd();
  }

  const CGeomObject3D::LineList &lines = object.getLines();

  auto pl1 = lines.begin();
  auto pl2 = lines.end  ();

  for ( ; pl1 != pl2; ++pl1) {
    CGeomVertex3D vertex1 = (*pl1)->getStartVertex();
    CGeomVertex3D vertex2 = (*pl1)->getEndVertex  ();

    glBegin(GL_LINES);

    const CRGBA &color = (*pl1)->getMaterial().getDiffuse();

    vertex1.place(matrix); vertex1.view(vmatrix);
    vertex2.place(matrix); vertex2.view(vmatrix);

    const CPoint3D &point1 = vertex1.getViewed();
    const CPoint3D &point2 = vertex2.getViewed();

    glColor3d(color.getRed(), color.getGreen(), color.getBlue());

    glVertex3d(point1.x, point1.y, point1.z);
    glVertex3d(point2.x, point2.y, point2.z);

    glEnd();
  }
}
#endif
