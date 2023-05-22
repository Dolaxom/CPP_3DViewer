#pragma once

#include <exception>
#include <fstream>
#include <functional>
#include <string>

namespace s21 {
// newStr is empty = read from conf, else = write to conf
inline void setLaunchPathToObject(std::string& str, const std::string& newStr) {
  if (newStr.empty()) {
    std::ifstream confFile("launch_parameters.conf");

    if (confFile.is_open()) {
      std::string pathLine{};
      std::getline(confFile, pathLine);
      //  Remove UTF-8 BOM (Byte Order Mark) symbols
      if (!pathLine.empty() && pathLine[0] == '\xEF' && pathLine[1] == '\xBB' &&
          pathLine[2] == '\xBF') {
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
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hashCombine(seed, rest), ...);
}

}  // namespace s21