#pragma once

#include "s21_object.h"
#include "s21_window.h"

namespace s21 {
class S21InputController {
 public:
  struct KeyMappings {
    int moveLeft = GLFW_KEY_LEFT;
    int moveRight = GLFW_KEY_RIGHT;
    int moveForward = GLFW_KEY_UP;
    int moveBackward = GLFW_KEY_DOWN;
  };

  void moveInPlaneXZ(GLFWwindow* window, float deltaTime,
                     S21Object& gameObject);

  KeyMappings keys{};
  float moveSpeed{3.0f};
};
}  // namespace s21
