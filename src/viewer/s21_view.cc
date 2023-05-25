#include "s21_view.h"

#include <iostream>

#include "s21_object.h"

namespace s21 {

typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line,
                                        const char* section, void* user_data);
extern ImGuiDemoMarkerCallback GImGuiDemoMarkerCallback;
extern void* GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback GImGuiDemoMarkerCallback = NULL;
void* GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)                                \
  do {                                                            \
    if (GImGuiDemoMarkerCallback != NULL)                         \
      GImGuiDemoMarkerCallback(__FILE__, __LINE__, section,       \
                               GImGuiDemoMarkerCallbackUserData); \
  } while (0)

S21View::S21View() {
  setLaunchPathToObject(filePath_, "");
  fileName_ = getFileName(filePath_);
  std::cout << "fileName_:" << fileName_ + '\n';
}

void S21View::userInterface() {
  showMainMenu();

  ImGui::Begin("Viewer options");

  IMGUI_DEMO_MARKER("Object");
  if (ImGui::CollapsingHeader("Object")) {
    IMGUI_DEMO_MARKER("Object/Transform");
    if (ImGui::TreeNode("Transform")) {
      ImGui::PushItemWidth(75.0f);
      ImGui::Text("Location");
      ImGui::SameLine();
      ImGui::InputFloat("##LocX", &location_.x);
      ImGui::SameLine();
      ImGui::InputFloat("##LocY", &location_.y);
      ImGui::SameLine();
      ImGui::InputFloat("##LocZ", &location_.z);

      ImGui::Text("Rotation");
      ImGui::SameLine();
      ImGui::InputFloat("##RotX", &rotation_.x);
      ImGui::SameLine();
      ImGui::InputFloat("##RotY", &rotation_.y);
      ImGui::SameLine();
      ImGui::InputFloat("##RotZ", &rotation_.z);

      ImGui::Text("Scale   ");
      ImGui::SameLine();
      ImGui::InputFloat("##SclX", &scale_.x);
      ImGui::SameLine();
      ImGui::InputFloat("##SclY", &scale_.y);
      ImGui::SameLine();
      ImGui::InputFloat("##SclZ", &scale_.z);

      ImGui::PopItemWidth();
      ImGui::TreePop();
    }
    IMGUI_DEMO_MARKER("Object/Projection");
    if (ImGui::TreeNode("Projection")) {
      const char* items_[] = {"perspective", "orthographic"};
      static const char* currentItem = items_[0];
      ImGui::PushItemWidth(125.0f);
      if (ImGui::BeginCombo("##combo", currentItem)) {
        for (int n = 0; n < IM_ARRAYSIZE(items_); n++) {
          bool isSelected = (currentItem == items_[n]);
          if (ImGui::Selectable(items_[n], isSelected)) {
            currentItem = items_[n];

            currentProjection_ = currentItem == items_[0] ? 0 : 1;
          }
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }
        }

        ImGui::EndCombo();
      }

      ImGui::PopItemWidth();
      ImGui::TreePop();
    }
  }
  IMGUI_DEMO_MARKER("Styles");
  if (ImGui::CollapsingHeader("Styles")) {
    IMGUI_DEMO_MARKER("Styles/Background");
    if (ImGui::TreeNode("Background")) {
      ImGui::PushItemWidth(125.0f);

      ImGui::ColorPicker3("BackgroundColor", backgroundColor_, 0);

      ImGui::PopItemWidth();
      ImGui::TreePop();
    }
    IMGUI_DEMO_MARKER("Styles/Edges");
    if (ImGui::TreeNode("Edges")) {
      ImGui::PushItemWidth(125.0f);

      ImGui::ColorPicker3("EdgesColor", edgesColor_, 0);

      ImGui::NewLine();
      ImGui::Text("Width");
      ImGui::SameLine();
      ImGui::InputFloat("##EdgeWidth", &edgeWidth_);
      ImGui::NewLine();

      const char* items_[] = {"Solid", "Stipple"};
      static const char* currentItem = items_[0];
      if (ImGui::BeginCombo("##comboStyle", currentItem)) {
        for (int n = 0; n < IM_ARRAYSIZE(items_); n++) {
          bool isSelected = (currentItem == items_[n]);
          if (ImGui::Selectable(items_[n], isSelected)) {
            currentItem = items_[n];

            currentLineStyle_ = currentItem == items_[0] ? 0xFFFF : 0x00FF;
          }
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }
        }

        ImGui::EndCombo();
      }

      ImGui::PopItemWidth();
      ImGui::TreePop();
    }
    IMGUI_DEMO_MARKER("Styles/Vertices");
    if (ImGui::TreeNode("Vertices")) {
      ImGui::PushItemWidth(125.0f);

      ImGui::ColorPicker3("VerticesColor", verticesColor_, 0);

      const char* items_[] = {"None", "Square"};
      static const char* currentItem = items_[0];
      if (ImGui::BeginCombo("##comboStyle", currentItem)) {
        for (int n = 0; n < IM_ARRAYSIZE(items_); n++) {
          bool isSelected = (currentItem == items_[n]);
          if (ImGui::Selectable(items_[n], isSelected)) {
            currentItem = items_[n];

            isDisplayVertices_ = currentItem == items_[0] ? 0 : 1;
          }
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }
        }

        ImGui::EndCombo();
      }

      ImGui::PopItemWidth();
      ImGui::TreePop();
    }
  }

  ImGui::End();
}

void S21View::statisticsInterface() {
  ImGui::Begin("Statistics");

  IMGUI_DEMO_MARKER("Object");
  if (ImGui::CollapsingHeader("Object")) {
    ImGui::PushItemWidth(75.0f);
    ImGui::Text("filename:");
    ImGui::SameLine();
    ImGui::Text(fileName_.c_str());
    ImGui::NewLine();
    ImGui::Text("Vertices count:");
    ImGui::SameLine();
    ImGui::Text(std::to_string(verticesCount_).c_str());
    ImGui::NewLine();
    ImGui::Text("Faces count:");
    ImGui::SameLine();
    ImGui::Text(std::to_string(facesCount_).c_str());
    ImGui::NewLine();
    ImGui::PopItemWidth();
  }

  ImGui::End();
}

void S21View::moveInPlaneXZ(S21Object& gameObject) {
  glm::vec3 scale{1.0f};
  scale += scale_;
  gameObject.transform.scale = scale_;

  glm::vec3 rotate_{0};
  rotate_ += rotation_;
  gameObject.transform.rotation = glm::radians(rotate_);
  gameObject.transform.rotation.x =
      glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
  gameObject.transform.rotation.y =
      glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

  float yaw = gameObject.transform.rotation.y;
  const glm::vec3 forwardDirection{sin(yaw), 0.f, cos(yaw)};
  const glm::vec3 rightDirection{forwardDirection.z, 0.f, -forwardDirection.x};
  const glm::vec3 upDirection{0.f, -1.f, 0.f};

  glm::vec3 moveDirection_{0.f};
  moveDirection_ += location_;

  gameObject.transform.translation = moveDirection_;
}

void S21View::setVerticesCount(uint32_t count) { verticesCount_ = count; }

void S21View::setFacesCount(uint32_t count) { facesCount_ = count; }

void S21View::showMainMenu() {
  bool open = false;
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Menu")) {
      if (ImGui::MenuItem("Load new .obj", NULL)) open = true;

      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (open) ImGui::OpenPopup("Load new .obj");

  if (file_dialog.showFileDialog(
          "Load new .obj", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
          ImVec2(700, 310), ".obj")) {
    filePath_ = file_dialog.selected_path;
    setLaunchPathToObject(filePath_, filePath_);
    fileName_ = getFileName(filePath_);
  }
}

glm::vec3 S21View::float3ToVec3(float* input) {
  glm::vec3 result;
  result.x = input[0];
  result.y = input[1];
  result.z = input[2];

  return result;
}

std::string S21View::getFileName(const std::string& filePath) {
  size_t lastSlash = filePath.find_last_of("\\/");

  if (lastSlash != std::string::npos) {
    return filePath.substr(lastSlash + 1);
  }

  return filePath;
}

}  // namespace s21
