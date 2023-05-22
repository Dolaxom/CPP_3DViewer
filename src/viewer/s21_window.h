#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace s21 {

class S21Window {
 public:
  S21Window(int w, int h, std::string name);
  ~S21Window();

  S21Window(const S21Window&) = delete;
  S21Window& operator=(const S21Window&) = delete;

  GLFWwindow* getWindow() const { return window; };

  bool shouldClose() { return glfwWindowShouldClose(window); }
  VkExtent2D getExtent() {
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }
  bool wasWindowResized() { return framebufferResized; }
  void resetWindowResizedFlag() { framebufferResized = false; }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

 private:
  static void framebufferResizeCallback(GLFWwindow* window, int width,
                                        int height);
  void initWindow();

  int width;
  int height;
  bool framebufferResized = false;

  std::string windowName;
  GLFWwindow* window;
};
}  // namespace s21