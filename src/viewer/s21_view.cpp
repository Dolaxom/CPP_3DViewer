#include "s21_view.h"

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

S21View::S21View() { setLaunchPathToObject(filePath, ""); }

void S21View::drawInterface() {
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
      static const char* current_item_ = items_[0];
      ImGui::PushItemWidth(125.0f);
      if (ImGui::BeginCombo("##combo", current_item_)) {
        for (int n = 0; n < IM_ARRAYSIZE(items_); n++) {
          bool is_selected = (current_item_ == items_[n]);
          if (ImGui::Selectable(items_[n], is_selected)) {
            current_item_ = items_[n];

            currentProjection_ = current_item_ == items_[0] ? 0 : 1;
          }
          if (is_selected) {
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
      static const char* current_item_ = items_[0];
      if (ImGui::BeginCombo("##comboStyle", current_item_)) {
        for (int n = 0; n < IM_ARRAYSIZE(items_); n++) {
          bool is_selected = (current_item_ == items_[n]);
          if (ImGui::Selectable(items_[n], is_selected)) {
            current_item_ = items_[n];

            currentLineStyle_ = current_item_ == items_[0] ? 0xFFFF : 0x00FF;
          }
          if (is_selected) {
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

      ImGui::PopItemWidth();
      ImGui::TreePop();
    }
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
    filePath = file_dialog.selected_path;
    setLaunchPathToObject(filePath, filePath);
  }
}

glm::vec3 S21View::float3ToVec3(float* input) {
  glm::vec3 result;
  result.x = input[0];
  result.y = input[1];
  result.z = input[2];

  return result;
}
}  // namespace s21
