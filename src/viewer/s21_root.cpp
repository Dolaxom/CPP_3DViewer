#include "s21_root.h"

namespace s21 {
S21Window& S21Root::getWindow() {
  static S21Window s21Window{windowWidth_, windowHeight_, "3DViewer"};
  return s21Window;
}

S21Device& S21Root::getDevice() {
  static S21Device s21Device{getWindow()};
  return s21Device;
}

S21RendererComponent& S21Root::getRendererComponent() {
  static S21RendererComponent s21Renderer{getWindow(), getDevice()};
  return s21Renderer;
}

S21View& S21Root::getView() {
  static S21View s21View;
  return s21View;
}

std::vector<S21Object>& S21Root::getObject() {
  static std::vector<S21Object> s21Objects;
  return s21Objects;
}
}  // namespace s21