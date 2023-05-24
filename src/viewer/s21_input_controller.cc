#include "s21_input_controller.h"

namespace s21 {

void S21InputController::moveInPlaneXZ(GLFWwindow* window, float deltaTime,
                                       S21Object& gameObject) {
  float yaw = gameObject.transform.rotation.y;
  const glm::vec3 forwardDirection{sin(yaw), 0.f, cos(yaw)};
  const glm::vec3 rightDirection{forwardDirection.z, 0.f, -forwardDirection.x};
  const glm::vec3 upDirection{0.f, -1.f, 0.f};

  glm::vec3 moveDirection{0.f};
  if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
    moveDirection += forwardDirection;
  if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
    moveDirection -= forwardDirection;
  if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
    moveDirection += rightDirection;
  if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
    moveDirection -= rightDirection;

  if (glm::dot(moveDirection, moveDirection) >
      std::numeric_limits<float>::epsilon()) {
    gameObject.transform.translation +=
        moveSpeed * deltaTime * glm::normalize(moveDirection);
  }
}

}  // namespace s21
