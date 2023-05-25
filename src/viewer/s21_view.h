#pragma once

#include "imgui/file_browser/ImGuiFileBrowser.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"
#include "s21_object.h"
#include "s21_utils.h"
#include "s21_window.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace s21 {

class S21Object;

class S21View {
 public:
  S21View();

  void userInterface();
  void statisticsInterface();

  void moveInPlaneXZ(S21Object& gameObject);
  void setVerticesCount(uint32_t count);
  void setFacesCount(uint32_t count);
  int getCurrentProjection() const { return currentProjection_; }
  int getVerticesDisplayMode() const { return isDisplayVertices_; }
  std::string getFilePath() const { return filePath_; }
  glm::vec3 getEdgesColor() { return float3ToVec3(edgesColor_); }
  glm::vec3 getVerticesColor() { return float3ToVec3(verticesColor_); }
  float* getBackgroundColor() { return backgroundColor_; }
  float getEdgeWidth() const { return edgeWidth_; }

  uint16_t getStipPatternt() const { return currentLineStyle_; }

 private:
  void showMainMenu();
  imgui_addons::ImGuiFileBrowser file_dialog;

  glm::vec3 location_{};
  glm::vec3 rotation_{};
  glm::vec3 scale_{1.0f};

  int currentProjection_ = 0;           // 0 = perspective; 1 = orthographic;
  int isDisplayVertices_ = 0;           // 0 = None, 1 = Square
  uint16_t currentLineStyle_ = 0xFFFF;  // 0xFFFF = solid, 0x00FF = stipple;

  std::string filePath_{};

  uint32_t verticesCount_{};
  uint32_t facesCount_{};
  std::string fileName_{};

  float backgroundColor_[3]{};
  float edgesColor_[3]{1.0f, 1.0f, 1.0f};
  float verticesColor_[3]{1.0f, 1.0f, 1.0f};

  float edgeWidth_ = 1.0f;

  glm::vec3 float3ToVec3(float* input);
  std::string getFileName(const std::string& filePath);
};

}  // namespace s21
