#include "s21_window.h"

#include <stdexcept>

namespace s21 {

S21Window::S21Window(int w, int h, std::string name)
    : width{w}, height{h}, windowName{name} {
  initWindow();
}

S21Window::~S21Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void S21Window::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window =
      glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void S21Window::createWindowSurface(VkInstance instance,
                                    VkSurfaceKHR* surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to craete window surface");
  }
}

void S21Window::framebufferResizeCallback(GLFWwindow* window, int width,
                                          int height) {
  auto s21Window =
      reinterpret_cast<S21Window*>(glfwGetWindowUserPointer(window));
  s21Window->framebufferResized = true;
  s21Window->width = width;
  s21Window->height = height;
}

}  // namespace s21