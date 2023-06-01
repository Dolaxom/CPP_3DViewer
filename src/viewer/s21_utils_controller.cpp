#include "s21_utils_controller.h"

#include "s21_utils.h"

namespace s21 {
std::string S21UtilsController::getFileName(const std::string& filePath) {
  return S21Utils::getFileName(filePath);
}

glm::vec3 S21UtilsController::float3ToVec3(float* input) {
  return S21Utils::float3ToVec3(input);
}

void S21UtilsController::setLaunchPathToObject(std::string& str,
                                               const std::string& newStr) {
  S21Utils::setLaunchPathToObject(str, newStr);
}

}  // namespace s21
