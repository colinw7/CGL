#ifndef CGLBuffer_H
#define CGLBuffer_H

#include <vector>
#include <cassert>

class CGLBuffer {
 public:
  struct Point {
    float x { 0.0 };
    float y { 0.0 };
    float z { 0.0 };

    Point() { }

    Point(float x, float y, float z) :
     x(x), y(y), z(z) {
    }
  };

  struct Color {
    float r { 0.0 };
    float g { 0.0 };
    float b { 0.0 };

    Color() { }

    Color(float r, float g, float b) :
     r(r), g(g), b(b) {
    }
  };

  struct TexturePoint {
    float x { 0.0 };
    float y { 0.0 };

    TexturePoint() { }

    TexturePoint(float x, float y) :
     x(x), y(y) {
    }
  };

  using Points        = std::vector<Point>;
  using Colors        = std::vector<Color>;
  using TexturePoints = std::vector<TexturePoint>;
  using Indices       = std::vector<int>;

 public:
  enum Parts {
    POINT   = (1<<0),
    NORMAL  = (1<<1),
    COLOR   = (1<<2),
    TEXTURE = (1<<3)
  };

 public:
  CGLBuffer() {
    initIds();
  }

 ~CGLBuffer() {
    term();
  }

  CGLBuffer(const CGLBuffer &buffer) {
    initFrom(buffer);
  }

  CGLBuffer &operator=(const CGLBuffer &buffer) {
    term();

    initFrom(buffer);

    return *this;
  }

  //---

  void addPoint(float x, float y, float z) {
    addPoint(Point(x, y, z));
  }

  void addPoint(const Point &p) {
    data_.types |= static_cast<unsigned int>(Parts::POINT);

    data_.points.push_back(p);

    data_.dataValid = false;
  }

  //---

  void addNormal(float x, float y, float z) {
    addNormal(Point(x, y, z));
  }

  void addNormal(const Point &p) {
    data_.types |= static_cast<unsigned int>(Parts::NORMAL);

    data_.normals.push_back(p);

    data_.dataValid = false;
  }

  void clearNormals() {
    data_.types &= ~static_cast<unsigned int>(Parts::NORMAL);

    data_.dataValid = false;
  }

  //---

  void addColor(float r, float g, float b) {
    addColor(Color(r, g, b));
  }

  void addColor(const Color &c) {
    data_.types |= static_cast<unsigned int>(Parts::COLOR);

    data_.colors.push_back(c);

    data_.dataValid = false;
  }

  void clearColors() {
    data_.types &= ~static_cast<unsigned int>(Parts::COLOR);

    data_.dataValid = false;
  }

  //---

  void addTexturePoint(float x, float y) {
    addTexturePoint(TexturePoint(x, y));
  }

  void addTexturePoint(const TexturePoint &p) {
    data_.types |= static_cast<unsigned int>(Parts::TEXTURE);

    data_.texturePoints.push_back(p);

    data_.dataValid = false;
  }

  void clearTexturePoints() {
    data_.types &= ~static_cast<unsigned int>(Parts::TEXTURE);

    data_.dataValid = false;
  }

  //---

  void addIndex(int i) {
    data_.indices.push_back(i);

    data_.indicesSet = true;
  }

  //---

  void load() {
    initData();

    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(data_.vertexArrayId);

    // send geometry data to buffer
    glBindBuffer(GL_ARRAY_BUFFER, data_.vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, data_.numData*sizeof(float), data_.data, GL_STATIC_DRAW);

    // send indices data to buffer
    if (data_.indicesSet) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data_.indBufferId);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_.numIndData*sizeof(int),
                   data_.indData, GL_STATIC_DRAW);
    }

    GLuint vid  = 0;
    uint   span = 0;

    // store points in vertex array
    if (data_.types & static_cast<unsigned int>(Parts::POINT)) {
      glVertexAttribPointer(vid, 3, GL_FLOAT, GL_FALSE, GLsizei(data_.span*sizeof(float)),
                            reinterpret_cast<void *>(span*sizeof(float)));
      glEnableVertexAttribArray(vid++);
      span += 3;
    }

    // store normals in vertex array
    if (data_.types & static_cast<unsigned int>(Parts::NORMAL)) {
      glVertexAttribPointer(vid, 3, GL_FLOAT, GL_FALSE, GLsizei(data_.span*sizeof(float)),
                            reinterpret_cast<void *>(span*sizeof(float)));
      glEnableVertexAttribArray(vid++);
      span += 3;
    }

    // store colors in vertex array
    if (data_.types & static_cast<unsigned int>(Parts::COLOR)) {
      glVertexAttribPointer(vid, 3, GL_FLOAT, GL_FALSE, GLsizei(data_.span*sizeof(float)),
                            reinterpret_cast<void *>(span*sizeof(float)));
      glEnableVertexAttribArray(vid++);
      span += 3;
    }

    // store texture points in vertex array
    if (data_.types & static_cast<unsigned int>(Parts::TEXTURE)) {
      glVertexAttribPointer(vid, 2, GL_FLOAT, GL_FALSE, GLsizei(data_.span*sizeof(float)),
                            reinterpret_cast<void *>(span*sizeof(float)));
      glEnableVertexAttribArray(vid++);
      span += 2;
    }

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the
    // vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object
    // IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO,
    // but this rarely happens. Modifying other VAOs requires a call to glBindVertexArray
    // anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
  }

  void bind() {
    //std::cerr << "Bind: " << data_.vertexArrayId << " " <<
    //             data_.vertexBufferId << " " << data_.indBufferId << "\n";

    // seeing as we only have a single VAO there's no need to bind it every time,
    // but we'll do so to keep things a bit more organized
    glBindVertexArray(data_.vertexArrayId);

#if 0
    // bind buffers (needed ?)
    glBindBuffer(GL_ARRAY_BUFFER, data_.vertexBufferId);

    if (data_.indicesSet)
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data_.indBufferId);
#endif
  }

  void unbind() {
    glBindVertexArray(0);
  }

  void drawTriangles() {
    glDrawArrays(GL_TRIANGLES, 0, data_.points.size());
  }

 private:
  void term() {
    delete [] data_.data;
    delete [] data_.indData;

    glDeleteBuffers(1, &data_.vertexBufferId);
    glDeleteBuffers(1, &data_.indBufferId);

    glDeleteVertexArrays(1, &data_.vertexArrayId);

    data_ = Data();
  }

  void initFrom(const CGLBuffer &buffer) {
    data_ = buffer.data_;

    if (buffer.data_.numData) {
      data_.data = new float [buffer.data_.numData];

      memcpy(data_.data, buffer.data_.data, buffer.data_.numData*sizeof(float));
    }
    else
      data_.data = nullptr;

    if (buffer.data_.numIndData) {
      data_.indData = new int [buffer.data_.numIndData];

      memcpy(data_.indData, buffer.data_.indData, buffer.data_.numIndData*sizeof(int));
    }
    else
      data_.indData = nullptr;

    initIds();
  }

  void initIds() {
    glGenVertexArrays(1, &data_.vertexArrayId);

    glGenBuffers(1, &data_.vertexBufferId);
    glGenBuffers(1, &data_.indBufferId);

    //std::cerr << "initIds: " << data_.vertexArrayId << " " <<
    //             data_.vertexBufferId << " " << data_.indBufferId << "\n";
  }

  void initData() {
    if (! data_.dataValid) {
      delete [] data_.data;
      delete [] data_.indData;

      //---

      data_.numData = 0;

      if (data_.types & static_cast<unsigned int>(Parts::POINT)) {
        data_.numData += data_.points.size()*3;
        data_.span += 3;
      }

      if (data_.types & static_cast<unsigned int>(Parts::NORMAL)) {
        data_.numData += data_.normals.size()*3;
        data_.span += 3;
      }

      if (data_.types & static_cast<unsigned int>(Parts::COLOR)) {
        assert(data_.colors.size() == data_.points.size());
        data_.numData += data_.colors.size()*3;
        data_.span += 3;
      }

      if (data_.types & static_cast<unsigned int>(Parts::TEXTURE)) {
        assert(data_.texturePoints.size() == data_.points.size());
        data_.numData += data_.texturePoints.size()*2;
        data_.span += 2;
      }

      data_.data = new float [data_.numData];

      int  i  = 0;
      auto np = data_.points.size();

      for (size_t ip = 0; ip < np; ++ip) {
        if (data_.types & static_cast<unsigned int>(Parts::POINT)) {
          const auto &p = data_.points[ip];

          data_.data[i++] = p.x;
          data_.data[i++] = p.y;
          data_.data[i++] = p.z;
        }

        if (data_.types & static_cast<unsigned int>(Parts::NORMAL)) {
          const auto &p = data_.normals[ip];

          data_.data[i++] = p.x;
          data_.data[i++] = p.y;
          data_.data[i++] = p.z;
        }

        if (data_.types & static_cast<unsigned int>(Parts::COLOR)) {
          const auto &c = data_.colors[ip];

          data_.data[i++] = c.r;
          data_.data[i++] = c.g;
          data_.data[i++] = c.b;
        }

        if (data_.types & static_cast<unsigned int>(Parts::TEXTURE)) {
          const auto &p = data_.texturePoints[ip];

          data_.data[i++] = p.x;
          data_.data[i++] = p.y;
        }
      }

      //---

      data_.numIndData = uint(data_.indices.size());

      data_.indData = new int [data_.numIndData];

      i = 0;

      for (const auto &ind : data_.indices)
        data_.indData[i++] = ind;

      //---

      data_.dataValid = true;
    }
  }

 private:
  struct Data {
    unsigned int  types          { 0 };
    unsigned int  vertexArrayId  { 0 };
    unsigned int  vertexBufferId { 0 };
    unsigned int  indBufferId    { 0 };
    float*        data           { nullptr };
    unsigned int  numData        { 0 };
    int*          indData        { nullptr };
    unsigned int  numIndData     { 0 };
    unsigned int  span           { 0 };
    bool          dataValid      { false };
    Points        points;
    Points        normals;
    Colors        colors;
    TexturePoints texturePoints;
    Indices       indices;
    bool          indicesSet     { false };
  };

  Data data_;
};

#endif
