#pragma once

#include "s21_camera.h"
#include "s21_device.h"
#include "s21_input_controller.h"
#include "s21_object.h"
#include "s21_pipeline.h"

// std
#include <memory>
#include <vector>

namespace s21 {
class S21View;

class S21RenderSystem {
 public:
  S21RenderSystem(S21Device& device, VkRenderPass renderPass);
  ~S21RenderSystem();

  S21RenderSystem(const S21RenderSystem&) = delete;
  S21RenderSystem& operator=(const S21RenderSystem&) = delete;

  void renderGameObjects(VkCommandBuffer commandBuffer,
                         std::vector<S21Object>& gameObjects,
                         const S21Camera& camera, S21Object& offset,
                         S21View& view);

  void renderPoints(VkCommandBuffer commandBuffer,
                    std::vector<S21Object>& gameObjects,
                    const S21Camera& camera, S21Object& offset, S21View& view);

 private:
  void createPipelineLayout();
  void createPipeline(VkRenderPass renderPass);
  void createSecondPipeline(VkRenderPass renderPass);

  S21Device& s21Device;

  std::unique_ptr<S21Pipeline> s21Pipeline;
  std::unique_ptr<S21Pipeline> s21SecondPipeline;
  VkPipelineLayout pipelineLayout;
  VkPipelineLayout pipelineSecondLayout;
};
}  // namespace s21