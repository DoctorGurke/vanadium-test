#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace Debug
{
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessageCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData);

    void SetupDebuggingMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT &debugUtilsMessengerCI);
    void SetupDebugging(VkInstance instance);
    void FreeDebugCallback(VkInstance instance);
    void SetupLabels(VkInstance instance);
    void BeginLabel(VkCommandBuffer cmdbuffer,
                    std::string caption,
                    glm::vec4 color);
    void EndLabel(VkCommandBuffer cmdbuffer);
    void CheckVulkan(VkResult result);
}