#pragma once

#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace s21 {
struct S21UtilsController {
  static std::string getFileName(const std::string& filePath);

  static glm::vec3 float3ToVec3(float* input);

  static void setLaunchPathToObject(std::string& str,
                                    const std::string& newStr);
};
}  // namespace s21
