#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include "core/log.h"

namespace Debug
{
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessageCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData);

    void CheckResult(const VkResult& result);

    void SetupDebuggingMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT &debugUtilsMessengerCI);
    VkResult SetupDebugging(VkInstance instance);
    void FreeDebugCallback(VkInstance instance);
    void SetupLabels(VkInstance instance);
    void BeginLabel(VkCommandBuffer cmdbuffer,
                    std::string caption,
                    glm::vec4 color);
    void EndLabel(VkCommandBuffer cmdbuffer);
    void CheckVulkan(VkResult result);
}