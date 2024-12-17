#pragma once

#include "graphics/renderer.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vk_device.h"

class VulkanRenderer : public IRenderer
{
public:
    VulkanRenderer(const RendererProperties &properties);
    bool Initialize() override;
    void OnUpdate() override;
    ~VulkanRenderer() override;

private:
    RendererProperties settings;
    VkResult InitVulkan();
    VkResult CreateInstance();
    VkResult PickPhysicalDevice(const bool preferIntegrated = false);
    VkBool32 GetSupportedDepthStencilFormat(VkPhysicalDevice physicalDevice,
                                            VkFormat *depthStencilFormat);
    std::vector<std::string> supportedInstanceExtensions;
    std::vector<const char *> requestedInstanceExtensions;
    VkInstance instance{VK_NULL_HANDLE};
    // selected physical device
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    VkPhysicalDeviceProperties deviceProperties{};
    VkDevice device{VK_NULL_HANDLE};
    VkPhysicalDeviceFeatures enabledFeatures{};
    std::vector<const char *> enabledDeviceExtensions;
    VkPhysicalDeviceVulkan13Features extraFeatures{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    VulkanDevice *vulkanDevice;
    // Handle to the device graphics queue that command buffers are submitted to
    VkQueue queue{VK_NULL_HANDLE};
    // Depth buffer format (selected during Vulkan initialization)
    VkFormat depthFormat;
};