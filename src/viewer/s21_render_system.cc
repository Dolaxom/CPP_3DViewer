#include "s21_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <array>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <stdexcept>

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

namespace s21 {

struct SimplePushConstantData {
  glm::mat4 transform{1.f};
  alignas(16) glm::vec3 color{};
};

S21RenderSystem::S21RenderSystem(S21Device& device, VkRenderPass renderPass)
    : s21Device{device} {
  createPipelineLayout();
  createPipeline(renderPass);
  createSecondPipeline(renderPass);
}

S21RenderSystem::~S21RenderSystem() {
  vkDestroyPipelineLayout(s21Device.device(), pipelineLayout, nullptr);
  vkDestroyPipelineLayout(s21Device.device(), pipelineSecondLayout, nullptr);
}

void S21RenderSystem::createPipelineLayout() {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  if (vkCreatePipelineLayout(s21Device.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  if (vkCreatePipelineLayout(s21Device.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineSecondLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create second pipeline layout!");
  }
}

void S21RenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  S21Pipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  s21Pipeline =
      std::make_unique<S21Pipeline>(s21Device, "simple_shader.vert.spv",
                                    "simple_shader.frag.spv", pipelineConfig);
}

void S21RenderSystem::createSecondPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  S21Pipeline::defaultPipelineConfigInfo(pipelineConfig, true);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineSecondLayout;
  s21SecondPipeline =
      std::make_unique<S21Pipeline>(s21Device, "simple_shader.vert.spv",
                                    "simple_shader.frag.spv", pipelineConfig);
}

void S21RenderSystem::renderObject(VkCommandBuffer commandBuffer,
                                   std::vector<S21Object>& gameObjects,
                                   const S21Camera& camera, S21Object& offset,
                                   S21View& view) {
  vkCmdSetLineWidth(commandBuffer, view.getEdgeWidth());

  PFN_vkCmdSetLineStippleEXT vkCmdSetLineStippleEXT =
      reinterpret_cast<PFN_vkCmdSetLineStippleEXT>(
          vkGetDeviceProcAddr(s21Device.device(), "vkCmdSetLineStippleEXT"));
  if (!vkCmdSetLineStippleEXT) {
    throw std::runtime_error("vkCmdSetLineStippleEXT wrong.");
  }

  vkCmdSetLineStippleEXT(commandBuffer, 1, view.getStipPatternt());

  s21Pipeline->bind(commandBuffer);

  for (auto& obj : gameObjects) {
    view.moveInPlaneXZ(obj);
    SimplePushConstantData push{};
    push.color = view.getEdgesColor();
    push.transform =
        camera.getProjection() * obj.transform.mat4() * offset.transform.mat4();
    vkCmdPushConstants(
        commandBuffer, pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
        sizeof(SimplePushConstantData), &push);
    obj.model->bind(commandBuffer);
    obj.model->draw(commandBuffer);

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    view.userInterface();
    view.statisticsInterface();
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer, 0);
  }
}

void S21RenderSystem::renderObjectPoints(VkCommandBuffer commandBuffer,
                                         std::vector<S21Object>& gameObjects,
                                         const S21Camera& camera,
                                         S21Object& offset, S21View& view) {
  if (!view.getVerticesDisplayMode()) {
    return;
  }

  s21SecondPipeline->bind(commandBuffer);

  for (auto& obj : gameObjects) {
    view.moveInPlaneXZ(obj);
    SimplePushConstantData push{};
    glm::vec3 clr = {1.0f, 0.0f, 0.0f};
    push.color = view.getVerticesColor();
    push.transform =
        camera.getProjection() * obj.transform.mat4() * offset.transform.mat4();
    vkCmdPushConstants(
        commandBuffer, pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
        sizeof(SimplePushConstantData), &push);
    obj.model->bind(commandBuffer);
    obj.model->draw(commandBuffer);
  }
}
}  // namespace s21