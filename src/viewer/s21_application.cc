#include "s21_application.h"

#include "s21_camera.h"
#include "s21_input_controller.h"
#include "s21_render_system.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

// std
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace s21 {

S21Application::S21Application() {
  currentViewerModel = s21View.getFilePath();
  initImgui();
  loadGameObjects();
}

S21Application::~S21Application() {}

void S21Application::run() {
  S21RenderSystem simpleRenderSystem{s21Device,
                                     s21Renderer.getSwapChainRenderPass()};
  S21Camera camera{};

  auto viewerObject = S21Object::createGameObject();
  S21InputController offsetController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!s21Window.shouldClose()) {
    glfwPollEvents();

    if (currentViewerModel != s21View.getFilePath()) {
      currentViewerModel = s21View.getFilePath();
      s21Objects.clear();
      loadGameObjects();
    }

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                   currentTime)
            .count();
    currentTime = newTime;

    offsetController.moveInPlaneXZ(s21Window.getWindow(), frameTime,
                                   viewerObject);

    float aspect = s21Renderer.getAspectRatio();

    if (!s21View.getCurrentProjection()) {
      camera.setPerspectiveProjection(glm::radians(60.0f), aspect, 0.1f,
                                      500.0f);
    } else {
      camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 500.0f);
    }

    if (auto commandBuffer = s21Renderer.beginFrame()) {
      s21Renderer.beginSwapChainRenderPass(commandBuffer,
                                           s21View.getBackgroundColor());

      simpleRenderSystem.renderPoints(commandBuffer, s21Objects, camera,
                                      viewerObject, s21View);
      simpleRenderSystem.renderGameObjects(commandBuffer, s21Objects, camera,
                                           viewerObject, s21View);

      s21Renderer.endSwapChainRenderPass(commandBuffer);
      s21Renderer.endFrame();
    }
  }

  vkDeviceWaitIdle(s21Device.device());
}

void S21Application::loadGameObjects() {
  std::pair<uint32_t, uint32_t> modelAttributes;
  std::shared_ptr<S21Model> s21Model =
      S21Model::createModelFromFile(s21Device, currentViewerModel, modelAttributes);
  auto obj = S21Object::createGameObject();
  obj.model = s21Model;
  obj.transform.translation = {0.0f, 0.0f, 2.5f};
  obj.transform.scale = {1.0f, -1.0f, 1.0f};
  obj.transform.rotation = {0.0f, glm::radians(180.0f), 0.0f};
  s21Objects.push_back(std::move(obj));
  s21View.setVerticesCount(modelAttributes.first);
  s21View.setFacesCount(modelAttributes.second);
}

void S21Application::initImgui() {
  VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
  pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;
  vkCreateDescriptorPool(s21Device.device(), &pool_info, nullptr,
                         &g_DescriptorPool);

  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForVulkan(s21Window.getWindow(), true);

  ImGui_ImplVulkan_InitInfo info{};
  info.Instance = s21Device.getInstance();
  info.PhysicalDevice = s21Device.getPhysicalDevice();
  info.Device = s21Device.device();
  info.Queue = s21Device.graphicsQueue();
  info.DescriptorPool = g_DescriptorPool;
  info.MinImageCount = 2;
  info.ImageCount = S21SwapChain::MAX_FRAMES_IN_FLIGHT;

  ImGui_ImplVulkan_Init(&info, s21Renderer.getSwapChainRenderPass());
}

}  // namespace s21