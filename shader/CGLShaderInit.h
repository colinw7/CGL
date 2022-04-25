#ifndef CGLShaderInit_H
#define CGLShaderInit_H

#include <glad/glad.h>
#include <functional>

struct GLFWwindow;

namespace CGLApp {

static int screen_width  { 100 };
static int screen_height { 100 };

void init();

GLFWwindow *createWindow(int w, int h);

void enableDepthTest();

void clear(float r, float g, float b, float a, GLenum buffers);

void mainLoop(GLFWwindow *window, const std::function<void ()> &proc);

void term();

}

#endif
