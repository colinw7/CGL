#ifndef CGLShader_H
#define CGLShader_H

#include <CGLMatrix3D.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

class CGLShader {
 public:
  CGLShader() {
    program_ = glCreateProgram();
  }

 ~CGLShader() {
    glDeleteProgram(program_);
  }

  int program() const { return program_; }

  bool loadVertexFile(const std::string &filename) {
    return loadFile(Type::Vertex, filename);
  }

  bool loadFragmentFile(const std::string &filename) {
    return loadFile(Type::Fragment, filename);
  }

  bool loadGeometryFile(const std::string &filename) {
    return loadFile(Type::Geometry, filename);
  }

  bool loadVertexString(const char *str) {
    return loadString(Type::Vertex, str);
  }

  bool loadFragmentString(const char *str) {
    return loadString(Type::Fragment, str);
  }

  bool loadGeometryString(const char *str) {
    return loadString(Type::Geometry, str);
  }

  void attachVertex() {
    attach(Type::Vertex);
  }

  void attachFragment() {
    attach(Type::Fragment);
  }

  void attachGeometry() {
    attach(Type::Geometry);
  }

  void link() {
    glLinkProgram(program_);

    checkCompileErrors(Type::Program, program_);

    for (const auto &pi : shaderIds_)
      glDeleteShader(pi.second);

    shaderIds_.clear();
  }

  void use() {
    glUseProgram(program_);
  }

  //---

  void setBool(const std::string &name, bool value) const {
    glUniform1i(getUniformLocation(name), int(value));
  }
  void setInt(const std::string &name, int value) const {
    glUniform1i(getUniformLocation(name), value);
  }
  void setFloat(const std::string &name, float value) const {
    glUniform1f(getUniformLocation(name), value);
  }

  //---

#if 0
  void setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(getUniformLocation(name), 1, &value[0]);
  }
#endif
  void setVec2(const std::string &name, float x, float y) const {
    glUniform2f(getUniformLocation(name), x, y);
  }
#if 0
  void setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
  }
#endif
  void setVec3(const std::string &name, const CGLVector3D &v) const {
    glUniform3f(getUniformLocation(name), v.x(), v.y(), v.z());
  }
  void setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(getUniformLocation(name), x, y, z);
  }
#if 0
  void setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(getUniformLocation(name), 1, &value[0]);
  }
#endif
  void setVec4(const std::string &name, float x, float y, float z, float w) {
    glUniform4f(getUniformLocation(name), x, y, z, w);
  }

  //---

  void setVec3Array(const std::string &name, const CGLVector3D *v, int n) {
    glUniform3fv(getUniformLocation(name), n, reinterpret_cast<const float *>(v));
  }

  //---

#if 0
  void setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
  }
  void setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
  }
  void setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
  }
#endif

  void setMatrix(const std::string &name, const CGLMatrix3D &matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix.getData());
  }

  //---

  int getUniformLocation(const std::string &name) const {
    auto loc = glGetUniformLocation(program_, name.c_str());
    if (loc < 0) std::cerr << "Failed to find '" << name << "'\n";
    return loc;
  }

 private:
  enum class Type {
    Vertex   = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
    Program  = 999
  };

  bool loadFile(Type type, const std::string &filename) {
    std::ifstream file;

    if (! std::filesystem::exists(filename)) {
      std::cerr << "ERROR: Shader file '" << filename << " does not exist\n";
      return false;
    }

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::string code;

    try {
      file.open(filename);

      std::stringstream sstream;

      sstream << file.rdbuf();

      file.close();

      code = sstream.str();
    }
    catch (std::ifstream::failure& e) {
      std::cerr << "ERROR: Shader file '" << filename << " not succesfully read\n";
      return false;
    }

    const char *ccode = code.c_str();

    shaderIds_[type] = glCreateShader(GLenum(type));

    glShaderSource(shaderIds_[type], 1, &ccode, NULL);

    glCompileShader(shaderIds_[type]);

    checkCompileErrors(type, shaderIds_[type]);

    return true;
  }

  bool loadString(Type type, const char *str) {
    shaderIds_[type] = glCreateShader(GLenum(type));

    glShaderSource(shaderIds_[type], 1, &str, NULL);

    glCompileShader(shaderIds_[type]);

    checkCompileErrors(type, shaderIds_[type]);

    return true;
  }

  void attach(Type type) {
    glAttachShader(program_, shaderIds_[type]);
  }

  void checkCompileErrors(Type type, GLuint shader) {
    GLint success;

    GLchar infoLog[1024];

    auto typeName = this->typeName(type);

    if (type != Type::Program) {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

      if (! success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR: Shader Compilation Error Type: " <<
                     typeName << "\n" << infoLog << "\n";
      }
    }
    else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);

      if (! success) {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR:: Program Linking Error Type: " <<
                     typeName << "\n" << infoLog << "\n";
      }
    }
  }

  std::string typeName(Type type) const {
    switch (type) {
      case Type::Vertex  : return "Vertex";
      case Type::Fragment: return "Fragment";
      case Type::Geometry: return "Geometry";
      case Type::Program : return "Program";
      default:             return "";
    }
  }

 private:
  using ShaderIds = std::map<Type, unsigned int>;

  unsigned int program_ { 0 };
  ShaderIds    shaderIds_;
};

#endif
