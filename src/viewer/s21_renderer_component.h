#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "s21_device.h"
#include "s21_swap_chain.h"
#include "s21_window.h"

namespace s21 {
class S21RendererComponent {
 public:
  S21RendererComponent(S21Window& window, S21Device& device);
  ~S21RendererComponent();

  S21RendererComponent(const S21RendererComponent&) = delete;
  S21RendererComponent& operator=(const S21RendererComponent&) = delete;

  VkRenderPass getSwapChainRenderPass() const {
    return s21SwapChain->getRenderPass();
  }
  float getAspectRatio() const { return s21SwapChain->extentAspectRatio(); }
  bool isFrameInProgress() const { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted &&
           "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getFrameIndex() const {
    assert(isFrameStarted &&
           "Cannot get frame index when frame not in progress");
    return currentFrameIndex;
  }

  VkCommandBuffer beginFrame();
  void endFrame();
  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer,
                                float* clearData);
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

 private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapChain();

  S21Window& s21Window;
  S21Device& s21Device;
  std::unique_ptr<S21SwapChain> s21SwapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex;
  int currentFrameIndex{0};
  bool isFrameStarted{false};
};
}  // namespace s21