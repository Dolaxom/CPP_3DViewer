#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <exception>
#include <fstream>
#include <functional>
#include <string>

namespace s21 {
struct S21Utils {
  S21Utils() = default;

  static std::string getFileName(const std::string& filePath) {
    size_t lastSlash = filePath.find_last_of("\\/");

    if (lastSlash != std::string::npos) {
      return filePath.substr(lastSlash + 1);
    }

    return filePath;
  }
  
  static glm::vec3 float3ToVec3(float* input) {
    glm::vec3 result;
    result.x = input[0];
    result.y = input[1];
    result.z = input[2];

    return result;
  }

  // newStr is empty = read from conf, else = write to conf
  static void setLaunchPathToObject(std::string& str,
                                    const std::string& newStr) {
    if (newStr.empty()) {
      std::ifstream confFile("launch_parameters.conf");

      if (confFile.is_open()) {
        std::string pathLine{};
        std::getline(confFile, pathLine);
        //  Remove UTF-8 BOM (Byte Order Mark) symbols
        if (!pathLine.empty() && pathLine[0] == '\xEF' &&
            pathLine[1] == '\xBB' && pathLine[2] == '\xBF') {
          pathLine.erase(0, 3);
        }
        str = pathLine;
        confFile.close();
      } else {
        throw std::runtime_error("can`t find launch_parameters.conf");
      }
    } else {
      std::ofstream outputFile("launch_parameters.conf");
      outputFile << newStr;
      outputFile.close();
    }
  }

  template <typename T, typename... Rest>
  static void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
  }
};

}  // namespace s21