#pragma once

#include <vector>

#include "s21_device.h"
#include "s21_object.h"
#include "s21_renderer_component.h"
#include "s21_window.h"

namespace s21 {
class S21Root {
 public:
  static S21Window& getWindow();
  static S21Device& getDevice();
  static S21RendererComponent& getRendererComponent();
  static S21View& getView();
  static std::vector<S21Object>& getObject();

 private:
  S21Root() = default;

  static constexpr int windowWidth_ = 1024;
  static constexpr int windowHeight_ = 720;
};
}  // namespace s21