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
        data[0] = point.x; data[1] = point.y;
        n = 2;
        break;
      case GL_3D:
        data[0] = point.x; data[1] = point.y; data[2] = point.z;
        n = 3;
        break;
      case GL_3D_COLOR:
        data[0] = point.x; data[1] = point.y; data[2] = point.z;
        data[3] = rgba.getRed  (); data[4] = rgba.getGreen();
        data[5] = rgba.getBlue (); data[6] = rgba.getAlpha();
        n = 7;
        break;
        break;
      case GL_3D_COLOR_TEXTURE:
        data[0] = point.x; data[1] = point.y; data[2] = point.z;
        data[3] = rgba.getRed  (); data[4] = rgba.getGreen();
        data[5] = rgba.getBlue (); data[6] = rgba.getAlpha();
        data[7] = tmap.x; data[8] = tmap.y; data[9] = tmap.z; data[10] = 1.0;
        n = 11;
        break;
      case GL_4D_COLOR_TEXTURE:
        data[0] = point.x; data[1] = point.y; data[2] = point.z; data[3] = 1.0;
        data[4] = rgba.getRed  (); data[5] = rgba.getGreen();
        data[6] = rgba.getBlue (); data[7] = rgba.getAlpha();
        data[8] = tmap.x; data[9] = tmap.y; data[10] = tmap.z; data[11] = 1.0;
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
