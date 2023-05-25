#include "s21_renderer_component.h"

#include <array>
#include <cassert>
#include <stdexcept>

namespace s21 {

S21RendererComponent::S21RendererComponent(S21Window& window, S21Device& device)
    : s21Window{window}, s21Device{device} {
  recreateSwapChain();
  createCommandBuffers();
}

S21RendererComponent::~S21RendererComponent() { freeCommandBuffers(); }

void S21RendererComponent::recreateSwapChain() {
  auto extent = s21Window.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = s21Window.getExtent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(s21Device.device());

  if (s21SwapChain == nullptr) {
    s21SwapChain = std::make_unique<S21SwapChain>(s21Device, extent);
  } else {
    std::shared_ptr<S21SwapChain> oldSwapChain = std::move(s21SwapChain);
    s21SwapChain =
        std::make_unique<S21SwapChain>(s21Device, extent, oldSwapChain);

    if (!oldSwapChain->compareSwapFormats(*s21SwapChain.get())) {
      throw std::runtime_error(
          "Swap chain image(or depth) format has changed!");
    }
  }
}

void S21RendererComponent::createCommandBuffers() {
  commandBuffers.resize(S21SwapChain::MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = s21Device.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(s21Device.device(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void S21RendererComponent::freeCommandBuffers() {
  vkFreeCommandBuffers(s21Device.device(), s21Device.getCommandPool(),
                       static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
}

VkCommandBuffer S21RendererComponent::beginFrame() {
  assert(!isFrameStarted && "Can't call beginFrame while already in progress");

  auto result = s21SwapChain->acquireNextImage(&currentImageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return nullptr;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  isFrameStarted = true;

  auto commandBuffer = getCurrentCommandBuffer();
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }
  return commandBuffer;
}

void S21RendererComponent::endFrame() {
  assert(isFrameStarted &&
         "Can't call endFrame while frame is not in progress");
  auto commandBuffer = getCurrentCommandBuffer();

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }

  auto result =
      s21SwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      s21Window.wasWindowResized()) {
    s21Window.resetWindowResizedFlag();
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  isFrameStarted = false;
  currentFrameIndex =
      (currentFrameIndex + 1) % S21SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void S21RendererComponent::beginSwapChainRenderPass(VkCommandBuffer commandBuffer,
                                           float* clearData) {
  assert(isFrameStarted &&
         "Can't call beginSwapChainRenderPass if frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't begin render pass on command buffer from a different frame");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = s21SwapChain->getRenderPass();
  renderPassInfo.framebuffer = s21SwapChain->getFrameBuffer(currentImageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = s21SwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {clearData[0], clearData[1], clearData[2], 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(s21SwapChain->getSwapChainExtent().width);
  viewport.height =
      static_cast<float>(s21SwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, s21SwapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void S21RendererComponent::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted &&
         "Can't call endSwapChainRenderPass if frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't end render pass on command buffer from a different frame");
  vkCmdEndRenderPass(commandBuffer);
}

}  // namespace s21