#ifndef CGLInput_H
#define CGLInput_H

#include <CGLCamera.h>
#include <glad/glad.h>

class CGLInput {
 public:
  CGLInput(GLFWwindow *window, CGLCamera *camera);

  void capture();

  void mouseProc(double xpos, double ypos);

  void scrollProc(double xoffset, double yoffset);

  void keyProc();

 private:
  GLFWwindow* window_ { nullptr };
  CGLCamera*  camera_ { nullptr };
};

//---

namespace {

static CGLInput *s_input = nullptr;

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* , double xpos, double ypos) {
  if (s_input)
    s_input->mouseProc(xpos, ypos);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* , double xoffset, double yoffset)
{
  if (s_input)
    s_input->scrollProc(xoffset, yoffset);
}

}

//---

CGLInput::
CGLInput(GLFWwindow *window, CGLCamera *camera) :
 window_(window), camera_(camera)
{
  s_input = this;

  glfwSetCursorPosCallback(window_, mouse_callback);
  glfwSetScrollCallback   (window_, scroll_callback);
}

void
CGLInput::
capture()
{
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void
CGLInput::
mouseProc(double xpos, double ypos)
{
  if (camera_->isFirstPos()) {
    camera_->setLastPos(xpos, ypos);

    camera_->setFirstPos(false);
  }

  float xoffset, yoffset;

  camera_->deltaPos(xpos, ypos, xoffset, yoffset);

  camera_->setLastPos(xpos, ypos);

  camera_->processMouseMovement(xoffset, yoffset);
}

void
CGLInput::
scrollProc(double , double yoffset)
{
  camera_->processMouseScroll(yoffset);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame
// and react accordingly
void
CGLInput::
keyProc()
{
  if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window_, true);

  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
    camera_->processKeyboard(CGLCamera::Movement::FORWARD, camera_->deltaTime());
  if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
    camera_->processKeyboard(CGLCamera::Movement::BACKWARD, camera_->deltaTime());
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
    camera_->processKeyboard(CGLCamera::Movement::LEFT, camera_->deltaTime());
  if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
    camera_->processKeyboard(CGLCamera::Movement::RIGHT, camera_->deltaTime());
}

#endif
