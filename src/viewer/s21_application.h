#pragma once

#include <vector>

#include "s21_device.h"
#include "s21_object.h"
#include "s21_renderer_component.h"
#include "s21_window.h"

#include "s21_root.h"

namespace s21 {
class S21Application {
 public:
  S21Application();
  ~S21Application();

  S21Application(const S21Application&) = delete;
  S21Application& operator=(const S21Application&) = delete;

  void run();

 private:
  void loadObject();
  void initImgui();

  S21Window& s21Window = S21Root::getWindow();
  S21Device& s21Device = S21Root::getDevice();
  S21RendererComponent& s21RendererComponent = S21Root::getRendererComponent();
  S21View& s21View = S21Root::getView();
  std::vector<S21Object>& s21Objects = S21Root::getObject();
 
  std::string currentPathToViewerModel;
};
}  // namespace s21