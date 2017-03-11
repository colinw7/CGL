#include <CGLControl3D.h>
#include <CGLUtil.h>
#include <GL/glut.h>

CGLControl3D::
CGLControl3D() :
 left_          (0.0),
 right_         (0.0),
 bottom_        (0.0),
 top_           (0.0),
 size_          (2.0),
 center_x_      (0.0),
 center_y_      (0.0),
 znear_         (-10.0),
 zfar_          ( 10.0),
 mouse_x_       (0),
 mouse_y_       (0),
 mouse_left_    (false),
 mouse_middle_  (false),
 mouse_right_   (false),
 drag_pos_x_    (0.0),
 drag_pos_y_    (0.0),
 drag_pos_z_    (0.0),
 ref_point_     (0.0, 0.0, 0.0),
 selectionFunc_ (NULL),
 pickFunc_      (NULL),
 disable_redraw_(false)
{
  getMatrix();
}

void
CGLControl3D::
setSize(double size)
{
  size_ = size;
}

void
CGLControl3D::
setCenter(double x, double y)
{
  center_x_ = x;
  center_y_ = y;
}

void
CGLControl3D::
reshape(int w, int h)
{
  if (! getEnabled()) return;

  glViewport(0, 0, w, h);

  double aspect = double(w)/double(h);

  double size2 = size_/2.0;

  left_   = center_x_ - size2*aspect;
  right_  = center_x_ + size2*aspect;
  bottom_ = center_y_ - size2;
  top_    = center_y_ + size2;

  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();

  glOrtho(left_, right_, bottom_, top_, znear_, zfar_);

  glMatrixMode(GL_MODELVIEW);
}

void
CGLControl3D::
display()
{
}

void
CGLControl3D::
mousePress(const CMouseEvent &event)
{
  if (! getEnabled()) return;

  mouse(CBUTTON_ACTION_PRESS, event);
}

void
CGLControl3D::
mouseRelease(const CMouseEvent &event)
{
  if (! getEnabled()) return;

  mouse(CBUTTON_ACTION_RELEASE, event);
}

void
CGLControl3D::
mouse(CButtonAction action, const CMouseEvent &event)
{
  if (! getEnabled()) return;

  /* Do picking */
  //if (action == CBUTTON_ACTION_PRESS)
  //  pick(event.getX(), glutGet(GLUT_WINDOW_HEIGHT) - 1 - event.getY(), 3, 3);

  mouse_x_ = event.getX();
  mouse_y_ = event.getY();

  if (action == CBUTTON_ACTION_RELEASE) {
    switch (event.getButton()) {
      case CBUTTON_LEFT  : mouse_left_   = false; break;
      case CBUTTON_MIDDLE: mouse_middle_ = false; break;
      case CBUTTON_RIGHT : mouse_right_  = false; break;
      default            :                        break;
    }
  }
  else {
    switch (event.getButton()) {
      case CBUTTON_LEFT  : mouse_left_   = true ; break;
      case CBUTTON_MIDDLE: mouse_middle_ = true ; break;
      case CBUTTON_RIGHT : mouse_right_  = true ; break;
      default            :                        break;
    }
  }

  GLint viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);

  pos(&drag_pos_x_, &drag_pos_y_, &drag_pos_z_, event.getX(), event.getY(), viewport);

  if (! disable_redraw_) {
    glutPostRedisplay();

    glFlush();
  }
}

void
CGLControl3D::
mouseMotion(const CMouseEvent &event)
{
  if (! getEnabled()) return;

  bool changed = false;

  int dx = event.getX() - mouse_x_;
  int dy = event.getY() - mouse_y_;

  if (dx == 0 && dy == 0)
    return;

  GLint viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);

  // zoom
  if      (mouse_middle_ || (mouse_left_ && mouse_right_)) {
    double s = exp(double(dy)*0.01);

    glTranslatef( ref_point_.x,  ref_point_.y,  ref_point_.z);

    glScalef(s, s, s);

    glTranslatef(-ref_point_.x, -ref_point_.y, -ref_point_.z);

    changed = true;
  }
  // rotate
  else if (mouse_left_) {
    double ax = dy;
    double ay = dx;
    double az = 0.0;

    double angle = vlen(ax, ay, az)/(double)(viewport[2]+1)*180.0;

    /* Use inverse matrix to determine local axis of rotation */

    double bx = imatrix_[0]*ax + imatrix_[4]*ay + imatrix_[8] *az;
    double by = imatrix_[1]*ax + imatrix_[5]*ay + imatrix_[9] *az;
    double bz = imatrix_[2]*ax + imatrix_[6]*ay + imatrix_[10]*az;

    glTranslatef( ref_point_.x,  ref_point_.y,  ref_point_.z);

    glRotatef(angle, bx, by, bz);

    glTranslatef(-ref_point_.x, -ref_point_.y, -ref_point_.z);

    changed = true;
  }
  // move
  else if (mouse_right_) {
    double px, py, pz;

    pos(&px, &py, &pz, event.getX(), event.getY(), viewport);

    glLoadIdentity();

    glTranslatef(px - drag_pos_x_, py - drag_pos_y_, pz - drag_pos_z_);

    glMultMatrixd(matrix_);

    drag_pos_x_ = px;
    drag_pos_y_ = py;
    drag_pos_z_ = pz;

    changed = true;
  }

  mouse_x_ = event.getX();
  mouse_y_ = event.getY();

  if (changed) {
    getMatrix();

    if (! disable_redraw_) {
      glutPostRedisplay();

      glFlush();
    }
  }
}

double
CGLControl3D::
vlen(double x, double y, double z)
{
  return sqrt(x*x + y*y + z*z);
}

void
CGLControl3D::
pos(double *px, double *py, double *pz, int x, int y, int *viewport)
{
  /*
    Use the ortho projection and viewport information
    to map from mouse co-ordinates back into world
    co-ordinates
  */

  *px = double(x - viewport[0])/double(viewport[2]);
  *py = double(y - viewport[1])/double(viewport[3]);

  *px = left_ + (*px)*(right_  - left_);
  *py = top_  + (*py)*(bottom_ - top_ );
  *pz = znear_;
}

void
CGLControl3D::
getMatrix()
{
  glGetDoublev(GL_MODELVIEW_MATRIX, matrix_);

  CGLUtil::invertMatrix(matrix_, imatrix_);
}

void
CGLControl3D::
setSelectionFunc(SelectionFunc selectionFunc)
{
  selectionFunc_ = selectionFunc;
}

void
CGLControl3D::
setPickFunc(PickFunc pickFunc)
{
  pickFunc_ = pickFunc;
}

/* Draw in selection mode */

void
CGLControl3D::
pick(GLdouble x, GLdouble y, GLdouble delX, GLdouble delY)
{
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

  glMultMatrixd(projection); /* Apply projection matrix        */

  glMatrixMode(GL_MODELVIEW);

  if (selectionFunc_)
    selectionFunc_(); /* Draw the scene in selection mode */

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
  GLuint minZ = -1;

  if (hits) {
    for (int i = 0, j = 0; i < hits; ++i) {
      if (buffer[j + 1] < minZ) {
        /* If name stack is empty, return -1                */
        /* If name stack is not empty, return top-most name */

        if (buffer[j] == 0)
          min = -1;
        else
          min = buffer[j+2+buffer[j]];

        minZ = buffer[j+1];
      }

      j += buffer[j] + 3;
    }
  }

  glMatrixMode(GL_PROJECTION);

  glPopMatrix(); /* Restore projection matrix */

  glMatrixMode(GL_MODELVIEW);

  if (pickFunc_)
    pickFunc_(min); /* Pass pick event back to application */
}
