#pragma once

#include <vector>

#include "s21_device.h"
#include "s21_object.h"
#include "s21_renderer.h"
#include "s21_window.h"

namespace s21 {
class S21Application {
 public:
  static constexpr int WIDTH = 1024;
  static constexpr int HEIGHT = 720;

  S21Application();
  ~S21Application();

  S21Application(const S21Application&) = delete;
  S21Application& operator=(const S21Application&) = delete;

  void run();

 private:
  void loadObject();
  void initImgui();

  S21Window s21Window{WIDTH, HEIGHT, "3DViewer"};
  S21Device s21Device{s21Window};
  S21Renderer s21Renderer{s21Window, s21Device};
  S21View s21View;
  std::vector<S21Object> s21Objects;
  std::string currentViewerModel;  // Path to model
};
}  // namespace s21