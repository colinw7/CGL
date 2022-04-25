#include <CGLShaderInit.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void resizeProc(GLFWwindow* window, int width, int height);

namespace CGLApp {

void
init()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

GLFWwindow *
createWindow(int w, int h)
{
  screen_width  = w;
  screen_height = h;

  // glfw window creation
  // --------------------
  auto *window = glfwCreateWindow(w, h, "Window", NULL, NULL);
  if (! window) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, resizeProc);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (! gladLoadGLLoader(GLADloadproc(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return nullptr;
  }

  return window;
}

void
enableDepthTest()
{
  glEnable(GL_DEPTH_TEST);
}

void
clear(float r, float g, float b, float a, GLenum buffers)
{
  glClearColor(r, g, b, a);

  glClear(buffers);
}

void
mainLoop(GLFWwindow *window, const std::function<void ()> &proc)
{
  while (! glfwWindowShouldClose(window)) {
    proc();

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void
term()
{
  // glfw: terminate, clearing all previously allocated GLFW resources.
  glfwTerminate();
}

}

//----

void
resizeProc(GLFWwindow *, int width, int height)
{
  CGLApp::screen_width  = width;
  CGLApp::screen_height = height;

  glViewport(0, 0, width, height);
}
