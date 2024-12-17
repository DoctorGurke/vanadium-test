#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include "core/log.h"

namespace Debug
{
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
    VkDebugUtilsMessengerEXT debugUtilsMessenger;

    PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT;

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
};

// Default debug callback
VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessageCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    // TODO: string prefix from messageSeverity
    std::string prefix = "VALIDATION";

    std::string message = std::format("[{0}] {1}",
                                      prefix,
                                      pCallbackData->pMessage);

    Log::System(message);

    // whether or not validation should abort vulkan
    return VK_FALSE;
}

// Used to populate a VkDebugUtilsMessengerCreateInfoEXT with our messenger function and desired flags
void Debug::SetupDebuggingMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &debugUtilsMessengerCI)
{
    debugUtilsMessengerCI.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCI.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCI.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugUtilsMessengerCI.pfnUserCallback = DebugUtilsMessageCallback;
}

// Load debug function pointers and set debug callback
void Debug::SetupDebugging(VkInstance instance)
{
    Debug::vkCreateDebugUtilsMessengerEXT = reinterpret_cast<
        PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT"));
    Debug::vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<
        PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT"));

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{};
    SetupDebuggingMessengerCreateInfo(debugUtilsMessengerCI);
    const VkResult result = vkCreateDebugUtilsMessengerEXT(
        instance,
        &debugUtilsMessengerCI,
        nullptr,
        &Debug::debugUtilsMessenger);
    assert(result == VK_SUCCESS);
    Log::System("Vulkan Debugger Init");
}

// Clear debug callback
void Debug::FreeDebugCallback(VkInstance instance)
{
    if (debugUtilsMessenger != VK_NULL_HANDLE)
    {
        Debug::vkDestroyDebugUtilsMessengerEXT(
            instance,
            debugUtilsMessenger,
            nullptr);
    }
}

// grab debug ext handles
void Debug::SetupLabels(VkInstance instance)
{
    Log::System("Vulkan Debugger Label Init");

    Debug::vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<
        PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(
        instance,
        "vkCmdBeginDebugUtilsLabelEXT"));
    Debug::vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<
        PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(
        instance,
        "vkCmdEndDebugUtilsLabelEXT"));
    Debug::vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<
        PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(
        instance,
        "vkCmdInsertDebugUtilsLabelEXT"));
}

void Debug::BeginLabel(VkCommandBuffer cmdbuffer,
                       std::string caption,
                       glm::vec4 color)
{
    if (!Debug::vkCmdBeginDebugUtilsLabelEXT)
    {
        return;
    }
    VkDebugUtilsLabelEXT labelInfo{};
    labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    labelInfo.pLabelName = caption.c_str();
    memcpy(labelInfo.color, &color[0], sizeof(float) * 4);
    Debug::vkCmdBeginDebugUtilsLabelEXT(cmdbuffer, &labelInfo);
}

void Debug::EndLabel(VkCommandBuffer cmdbuffer)
{
    if (!Debug::vkCmdEndDebugUtilsLabelEXT)
    {
        return;
    }
    Debug::vkCmdEndDebugUtilsLabelEXT(cmdbuffer);
}

void Debug::CheckVulkan(VkResult result)
{
    if (result != VK_SUCCESS)
    {
        Log::Error("Vulkan Failed!");
        assert(result == VK_SUCCESS);
    }
}