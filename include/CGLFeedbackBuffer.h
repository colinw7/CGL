struct CGLFeedbackBuffer {
  int    size;
  int    type;
  float *buffer;
  int    pos;

  CGLFeedbackBuffer() :
    size(0), type(GL_3D), buffer(NULL), pos(0) {
  }

  CGLFeedbackBuffer(int size1, int type1, float *buffer1) :
    size(size1), type(type1), buffer(buffer1), pos(0) {
  }

  void append(CGeomVertex3D *vertex) {
    float data[12];

    const CPoint3D &point = vertex->getModel();
    const CRGBA    &rgba  = vertex->getColor();
    const CPoint3D &tmap  = vertex->getTextureMap();

    int n = 0;

    switch (type) {
      case GL_2D:
        data[0] = float(point.x); data[1] = float(point.y);
        n = 2;
        break;
      case GL_3D:
        data[0] = float(point.x); data[1] = float(point.y); data[2] = float(point.z);
        n = 3;
        break;
      case GL_3D_COLOR:
        data[0] = float(point.x); data[1] = float(point.y); data[2] = float(point.z);
        data[3] = rgba.getRedF (); data[4] = rgba.getGreenF();
        data[5] = rgba.getBlueF(); data[6] = rgba.getAlphaF();
        n = 7;
        break;
        break;
      case GL_3D_COLOR_TEXTURE:
        data[0] = float(point.x); data[1] = float(point.y); data[2] = float(point.z);
        data[3] = rgba.getRedF (); data[4] = rgba.getGreenF();
        data[5] = rgba.getBlueF(); data[6] = rgba.getAlphaF();
        data[7] = float(tmap.x); data[8] = float(tmap.y); data[9] = float(tmap.z); data[10] = 1.0;
        n = 11;
        break;
      case GL_4D_COLOR_TEXTURE:
        data[0] = float(point.x); data[1] = float(point.y); data[2] = float(point.z); data[3] = 1.0;
        data[4] = rgba.getRedF (); data[5] = rgba.getGreenF();
        data[6] = rgba.getBlueF(); data[7] = rgba.getAlphaF();
        data[8] = float(tmap.x); data[9] = float(tmap.y); data[10] = float(tmap.z); data[11] = 1.0;
        n = 12;
        break;
    }

    append(data, n);
  }

  void append(float *data, int num) {
    for (int i = 0; i < num; ++i)
      if (buffer && pos < size)
        buffer[pos++] = data[i];
  }
};
