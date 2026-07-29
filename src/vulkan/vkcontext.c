#include "vulkan/vkcontext.h"

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>
#include <mira/darray.h>

#include <core/vnl_macros.h>
#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <vnl_ds/vnl_list.h>

typedef struct VkQueueFamilyIndices {
    bool has_graphics_family;
    bool has_present_family;
    u32 graphics_family;
    u32 present_family;
} VkQueueFamilyIndices;

typedef struct VkContext {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSurfaceKHR surface;
} VkContext;

#ifdef MIRA_CLARITY_DEBUG
static const char *validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
static const u32 validation_layer_count =
    sizeof(validation_layers) / sizeof(validation_layers[0]);

static bool vk_check_validation_layer_support(void) {
    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    if (layer_count == 0) {
        return false;
    }

    VkLayerProperties *available_layers =
        CLARITY_MALLOC(sizeof(VkLayerProperties) * layer_count);
    if (!available_layers) {
        return false;
    }

    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for (u32 i = 0; i < validation_layer_count; i++) {
        bool layer_found = false;
        for (u32 j = 0; j < layer_count; j++) {
            if (strcmp(validation_layers[i], available_layers[j].layerName) ==
                0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            CLARITY_FREE(available_layers);
            return false;
        }
    }

    CLARITY_FREE(available_layers);
    return true;
}
#endif

static VkApplicationInfo vk_context_init_app_info(const VnlConfig *config) {
    return (VkApplicationInfo){
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = config->title,
        .applicationVersion =
            VK_MAKE_VERSION(config->version.major, config->version.minor,
                            config->version.patch),
        .pEngineName = "Vanilla",
        .engineVersion =
            VK_MAKE_VERSION(VNL_ENGINE_VERSION_MAJOR, VNL_ENGINE_VERSION_MINOR,
                            VNL_ENGINE_VERSION_PATCH),
        .apiVersion = VK_API_VERSION_1_0};
}

static VkInstanceCreateInfo
vk_context_init_instance_create_info(const VkApplicationInfo *app_info) {
    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = app_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL};

#ifdef MIRA_CLARITY_DEBUG
    if (vk_check_validation_layer_support()) {
        create_info.enabledLayerCount = validation_layer_count;
        create_info.ppEnabledLayerNames = validation_layers;
        CLARITY_LOG_INFO("Validation layers enabled.");
    } else {
        CLARITY_LOG_WARN("Validation layers requested, but not available.");
    }
#endif

    u32 glfw_extension_count = 0;
    const char **glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;

    return create_info;
}

static VnlStatus vk_context_init(const VnlConfig *config, VkContext *vkctx) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");

    CLARITY_LOG_INFO("Initializing Vulkan Instance.");

    u32 extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);

    VkApplicationInfo app_info = vk_context_init_app_info(config);
    VkInstanceCreateInfo instance_info =
        vk_context_init_instance_create_info(&app_info);

    VkInstance instance;
    VkResult result = vkCreateInstance(&instance_info, NULL, &instance);

    if (result != VK_SUCCESS) {
        CLARITY_LOG_WARN("Failed to initialise Vulkan Instance.");
        return VNL_ERROR_VULKAN_INSTANCE_CREATION_FAILED;
    }

    vkctx->instance = instance;
    CLARITY_LOG_INFO("Vulkan Instance created successfully.");
    return VNL_SUCCESS;
}

static VkQueueFamilyIndices vk_find_queue_families(VkPhysicalDevice device,
                                                   VkSurfaceKHR surface) {
    VkQueueFamilyIndices indices = {.has_graphics_family = false,
                                    .has_present_family = false,
                                    .graphics_family = 0,
                                    .present_family = 0};

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

    VkQueueFamilyProperties *queue_families =
        CLARITY_MALLOC(sizeof(VkQueueFamilyProperties) * queue_family_count);

    if (!queue_families) {
        return indices;
    }
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             queue_families);

    for (u32 i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
            indices.has_graphics_family = true;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                             &present_support);
        if (present_support) {
            indices.present_family = i;
            indices.has_present_family = true;
        }

        if (indices.has_graphics_family && indices.has_present_family)
            break;
    }

    CLARITY_FREE(queue_families);
    return indices;
}

static bool vk_check_ext_suppport(VkPhysicalDevice device) {
    u32 ext_count = 0;
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, NULL);

    if (ext_count == 0) {
        return false;
    }

    VkExtensionProperties *available_ext =
        CLARITY_MALLOC(sizeof(VkExtensionProperties) * ext_count);
    if (!available_ext) {
        return false;
    }

    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count,
                                         available_ext);

    bool found = false;
    for (u32 i = 0; i < ext_count; i++) {
        if (strcmp(available_ext[i].extensionName,
                   VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
            found = true;
            break;
        }
    }

    CLARITY_FREE(available_ext);
    return found;
}

static bool vk_is_device_suitable(VkPhysicalDevice device,
                                  VkSurfaceKHR surface) {
    VkQueueFamilyIndices indices = vk_find_queue_families(device, surface);
    bool ext_supported = vk_check_ext_suppport(device);
    return indices.has_graphics_family && indices.has_present_family &&
           ext_supported;
}

static VnlStatus vk_pick_physical_device(VkContext *vkctx) {
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");
    CLARITY_ASSERT(vkctx->instance != VK_NULL_HANDLE,
                   "Vulkan Instance cannot be NULL.");

    CLARITY_LOG_INFO("Picking Vulkan Physical Device.");

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(vkctx->instance, &device_count, NULL);

    if (device_count == 0) {
        CLARITY_LOG_WARN("Failed to find a GPU with Vulkan support.");
        return VNL_ERROR_PHYSICAL_DEVICE_NOT_FOUND;
    }

    VkPhysicalDevice *devices =
        CLARITY_MALLOC(sizeof(VkPhysicalDevice) * device_count);

    if (!devices) {
        return VNL_ERROR_OUT_OF_MEMORY;
    }

    vkEnumeratePhysicalDevices(vkctx->instance, &device_count, devices);

    for (u32 i = 0; i < device_count; i++) {
        if (vk_is_device_suitable(devices[i], vkctx->surface)) {
            physical_device = devices[i];
            break;
        }
    }

    CLARITY_FREE(devices);

    if (physical_device == VK_NULL_HANDLE) {
        CLARITY_LOG_WARN("Failed to find a GPU with Vulkan support.");
        return VNL_ERROR_PHYSICAL_DEVICE_NOT_FOUND;
    }

    vkctx->physical_device = physical_device;
    CLARITY_LOG_INFO("Vulkan Physical Device picked successfully.");
    return VNL_SUCCESS;
}

static VnlStatus vk_create_logical_device(VkContext *vkctx) {
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");
    CLARITY_ASSERT(vkctx->physical_device != VK_NULL_HANDLE,
                   "Physical device cannot be NULL.");

    CLARITY_LOG_INFO("Creating Vulkan Logical Device.");

    VkPhysicalDeviceFeatures device_features = {0};
    VkQueueFamilyIndices indices =
        vk_find_queue_families(vkctx->physical_device, vkctx->surface);

    /**
     * The current approach only allows for two queues, since
     * queue_create_infos is a simple fixed-size array with
     * information to create the graphics and present queues.
     */

    f32 queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_infos[2];
    u32 queue_create_info_count = 0;

    queue_create_infos[queue_create_info_count++] = (VkDeviceQueueCreateInfo){
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = indices.graphics_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority};

    if (indices.graphics_family != indices.present_family) {
        queue_create_infos[queue_create_info_count++] =
            (VkDeviceQueueCreateInfo){
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = indices.present_family,
                .queueCount = 1,
                .pQueuePriorities = &queue_priority};
    }

    const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueCreateInfoCount = queue_create_info_count,
        .pQueueCreateInfos = queue_create_infos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = device_extensions,
        .pEnabledFeatures = &device_features};

#ifdef MIRA_CLARITY_DEBUG
    if (vk_check_validation_layer_support()) {
        create_info.enabledLayerCount = validation_layer_count;
        create_info.ppEnabledLayerNames = validation_layers;
    }
#endif

    VkResult result = vkCreateDevice(vkctx->physical_device, &create_info, NULL,
                                     &vkctx->device);

    if (result != VK_SUCCESS) {
        CLARITY_LOG_WARN("Failed to create logical device.");
        return VNL_ERROR_LOGICAL_DEVICE_CREATION_FAILED;
    }

    vkGetDeviceQueue(vkctx->device, indices.graphics_family, 0,
                     &vkctx->graphics_queue);
    vkGetDeviceQueue(vkctx->device, indices.present_family, 0,
                     &vkctx->present_queue);

    CLARITY_LOG_INFO("Vulkan Logical Device created successfully.");
    return VNL_SUCCESS;
}

static VnlStatus vk_create_surface(VkContext *vkctx, GLFWwindow *window) {
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");
    CLARITY_ASSERT(window != NULL, "GLFW Window cannot be NULL.");

    CLARITY_LOG_INFO("Creating Vulkan Window Surface.");

    VkResult result =
        glfwCreateWindowSurface(vkctx->instance, window, NULL, &vkctx->surface);

    if (result != VK_SUCCESS) {
        CLARITY_LOG_WARN("Failed to create Vulkan window surface.");
        return VNL_ERROR_SURFACE_CREATION_FAILED;
    }

    CLARITY_LOG_INFO("Vulkan Window Surface created successfully.");
    return VNL_SUCCESS;
}

VnlStatus vulkan_init(const VnlConfig *config, GLFWwindow *window,
                      VkContext **out_ctx) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");
    CLARITY_ASSERT(window != NULL, "GLFW Window cannot be NULL.");
    CLARITY_ASSERT(out_ctx != NULL, "out_ctx pointer cannot be NULL.");

    CLARITY_LOG_INFO("Initializing Vulkan Context.");

    VkContext *vkctx = CLARITY_MALLOC(sizeof(VkContext));
    if (vkctx) {
        memset(vkctx, 0, sizeof(VkContext));
    }

    if (!vkctx)
        return VNL_ERROR_OUT_OF_MEMORY;

    VnlStatus status;

    status = vk_context_init(config, vkctx);
    if (status != VNL_SUCCESS)
        goto cleanup;

    status = vk_create_surface(vkctx, window);
    if (status != VNL_SUCCESS)
        goto cleanup;

    status = vk_pick_physical_device(vkctx);
    if (status != VNL_SUCCESS)
        goto cleanup;

    status = vk_create_logical_device(vkctx);
    if (status != VNL_SUCCESS)
        goto cleanup;

    *out_ctx = vkctx;
    CLARITY_LOG_INFO("Vulkan Context initialized successfully.");
    return VNL_SUCCESS;

cleanup:
    CLARITY_LOG_ERROR("Vulkan Context initialization failed. Cleaning up...");
    vulkan_shutdown(vkctx);
    return status;
}

void vulkan_shutdown(VkContext *vkctx) {
    if (vkctx) {
        CLARITY_LOG_INFO("Shutting down Vulkan Context.");

        if (vkctx->device != VK_NULL_HANDLE) {
            vkDestroyDevice(vkctx->device, NULL);
        }
        if (vkctx->instance != VK_NULL_HANDLE &&
            vkctx->surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(vkctx->instance, vkctx->surface, NULL);
        }
        if (vkctx->instance != VK_NULL_HANDLE) {
            vkDestroyInstance(vkctx->instance, NULL);
        }
        CLARITY_FREE(vkctx);

        CLARITY_LOG_INFO("Vulkan Context shut down successfully.");
    }
}