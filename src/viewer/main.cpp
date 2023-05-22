#include "s21_application.h"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
  IMGUI_CHECKVERSION();
  ImGuiContext* mContext = ImGui::CreateContext();
  ImGui::SetCurrentContext(mContext);
  s21::S21Application app{};

  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}