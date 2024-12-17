#include "vulkan_renderer.h"

#include "core/log.h"
#include "vulkan/vk_initializers.h"

VulkanRenderer::VulkanRenderer(const RendererProperties &properties)
    : settings(properties)
{
    requestedInstanceExtensions.push_back(
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    requestedInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    extraFeatures.dynamicRendering = VK_TRUE;
    extraFeatures.synchronization2 = VK_TRUE;
}

bool VulkanRenderer::Initialize()
{
    const VkResult result = InitVulkan();
    if (result != VK_SUCCESS)
    {
        // Log::Error(std::format("Failed initializing Vulkan! [{0}]",
        //                        string_VkResult(result)));
        return false;
    }
    Log::System("Vulkan Init Done");
    return true;
}

// Initialize vulkan backend. Create Instance, grabs physical device, creates logical device (VulkanDevice),
// get graphics queue, verify supported depth stencil format
VkResult VulkanRenderer::InitVulkan()
{
    VkResult result = CreateInstance();
    if (result != VK_SUCCESS)
    {
        return result;
    }

    if (settings.Debug)
    {
        Debug::SetupDebugging(instance);
    }

    // grab physical device & info
    result = PickPhysicalDevice(settings.PreferIntegratedGraphics);

    if (result != VK_SUCCESS)
    {
        return result;
    }

    Log::System(std::format("Physical Device: {0}",
                            deviceProperties.deviceName));

    vulkanDevice = new VulkanDevice(physicalDevice);
    result = vulkanDevice->createLogicalDevice(
        enabledFeatures,
        enabledDeviceExtensions,
        &extraFeatures);

    if (result != VK_SUCCESS)
    {
        return result;
    }

    device = vulkanDevice->logicalDevice;

    // creating VulkanDevice->logical device checks for queues
    vkGetDeviceQueue(device,
                     vulkanDevice->queueFamilyIndices.graphics,
                     0,
                     &queue);

    // verify supported depth stencil format for attachment
    const VkBool32 validDepthStencilFormat = GetSupportedDepthStencilFormat(
        physicalDevice,
        &depthFormat);
    if (!validDepthStencilFormat)
    {
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    return VK_SUCCESS;
}

// Creates vulkan instance, gathers supportedInstanceExtensions, verifies requested extensions,
// hook up debug utils/debug messenger and validation if requested
VkResult VulkanRenderer::CreateInstance()
{
    std::vector<const char *> enabledInstanceExtensions = {};

    // gather supported instance extensions
    uint32_t supportedExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr,
                                           &supportedExtensionCount,
                                           nullptr);
    if (supportedExtensionCount > 0)
    {
        std::vector<VkExtensionProperties> extensions(
            supportedExtensionCount);
        if (vkEnumerateInstanceExtensionProperties(
                nullptr,
                &supportedExtensionCount,
                &extensions.front()) == VK_SUCCESS)
        {
            for (const VkExtensionProperties &extension : extensions)
            {
                supportedInstanceExtensions.push_back(
                    extension.extensionName);
            }
        }
    }

    // verify requested instance extensions
    if (requestedInstanceExtensions.size() > 0)
    {
        for (const char *requestedExtension : requestedInstanceExtensions)
        {
            if (std::ranges::find(supportedInstanceExtensions.begin(),
                                  supportedInstanceExtensions.end(),
                                  requestedExtension) ==
                supportedInstanceExtensions.end())
            {
                Log::Warning(std::format(
                    "Requested Instance extension {0} is not supported.",
                    requestedExtension));
            }
            enabledInstanceExtensions.push_back(requestedExtension);
        }
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = settings.Title.c_str();
    appInfo.pEngineName = settings.Title.c_str();
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    // hook up debugger if extension present
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{};
    if (settings.Debug && std::ranges::find(
                              supportedInstanceExtensions.begin(),
                              supportedInstanceExtensions.end(),
                              VK_EXT_DEBUG_UTILS_EXTENSION_NAME) !=
                              supportedInstanceExtensions.end())
    {
        Debug::SetupDebuggingMessengerCreateInfo(debugUtilsMessengerCI);
        debugUtilsMessengerCI.pNext = instanceCreateInfo.pNext;
        instanceCreateInfo.pNext = &debugUtilsMessengerCI;
        enabledInstanceExtensions.push_back(
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // add requested instance extensions to instance create info
    if (enabledInstanceExtensions.size() > 0)
    {
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(
            enabledInstanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames =
            enabledInstanceExtensions.data();
    }

    // add validation layer according to renderer setting and availability
    if (settings.Debug)
    {
        const auto validationLayerName = "VK_LAYER_KHRONOS_validation";
        uint32_t instanceLayerCount;
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        std::vector<VkLayerProperties> instanceLayerProperties(
            instanceLayerCount);
        vkEnumerateInstanceLayerProperties(&instanceLayerCount,
                                           instanceLayerProperties.data());

        // verify validation layer is supported
        bool validationLayerPresent = false;
        for (const VkLayerProperties &layer : instanceLayerProperties)
        {
            if (strcmp(layer.layerName, validationLayerName) == 0)
            {
                validationLayerPresent = true;
                break;
            }
        }

        // add instance layer to instance create info
        if (validationLayerPresent)
        {
            instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
            instanceCreateInfo.enabledLayerCount = 1;
            Log::System("Vulkan Validation Layer enabled.");
        }
        else
        {
            Log::Warning(
                "Validation Layer not present! Validation is disabled.");
        }
    }

    const VkResult result = vkCreateInstance(
        &instanceCreateInfo,
        nullptr,
        &instance);

    // enable debug labels if available
    if (std::ranges::find(supportedInstanceExtensions.begin(),
                          supportedInstanceExtensions.end(),
                          VK_EXT_DEBUG_UTILS_EXTENSION_NAME) !=
        supportedInstanceExtensions.end())
    {
        Debug::SetupLabels(instance);
    }

    return result;
}

// Select vulkan capable physical device, prefer discrete unless specified otherwise
VkResult VulkanRenderer::PickPhysicalDevice(const bool preferIntegrated)
{
    uint32_t gpuCount = 0;
    vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
    if (gpuCount == 0)
    {
        Log::Error("No Physical Device with Vulkan support found!");
        return VK_ERROR_DEVICE_LOST;
    }

    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
    if (const VkResult result = vkEnumeratePhysicalDevices(instance,
                                                           &gpuCount,
                                                           physicalDevices.data());
        result != VK_SUCCESS)
    {
        // Log::Error(std::format("Coult not enumerate Physical Devices: {0}",
        //                        string_VkResult(result)));
        return result;
    }

    // select discrete or integrated gpu based on setting (PreferIntegratedGpu)
    uint32_t selectedDevice = 0;
    if (gpuCount > 1)
    {
        for (int i = 0; i < physicalDevices.size(); i++)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(physicalDevices[i], &props);
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && preferIntegrated)
            {
                selectedDevice = i;
                break;
            }
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                !preferIntegrated)
            {
                selectedDevice = i;
            }
        }
    }

    physicalDevice = physicalDevices[selectedDevice];

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    return VK_SUCCESS;
}

/**
 * Grab best supported depth stencil format candidate, returns whether or not a valid format was found.
 * @param depthStencilFormat Output for valid depth stencil format
 */
VkBool32 VulkanRenderer::GetSupportedDepthStencilFormat(
    VkPhysicalDevice physicalDevice,
    VkFormat *depthStencilFormat)
{
    const std::vector<VkFormat> formats{
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
    };

    for (auto &format : formats)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(
            physicalDevice,
            format,
            &properties);
        if (properties.optimalTilingFeatures &
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            *depthStencilFormat = format;
            return true;
        }
    }

    return false;
}

void VulkanRenderer::OnUpdate() {}

VulkanRenderer::~VulkanRenderer()
{
    delete vulkanDevice;

    if (settings.Debug)
    {
        Debug::FreeDebugCallback(instance);
    }

    vkDestroyInstance(instance, nullptr);
}