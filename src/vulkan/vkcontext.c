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

#include "misc/vnl_macros.h"
#include "misc/vnl_status.h"
#include "misc/vnl_types.h"
#include "vnl_ds/vnl_list.h"

typedef struct VkQueueFamilyIndices {
    bool has_graphics_family;
    bool has_present_family;
    u32 graphics_family;
    u32 present_family;
} VkQueueFamilyIndices;

typedef struct VkContext {
    VkInstance instance;
    VkApplicationInfo app_info;
    VkInstanceCreateInfo create_info;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSurfaceKHR surface;
} VkContext;

static void vk_context_init_app_info(const VnlConfig *config, VkContext *vkctx) {
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = config->title,
        .applicationVersion =
            VK_MAKE_VERSION(config->version.major, config->version.minor, config->version.patch),
        .pEngineName = "Vanilla",
        .engineVersion = VK_MAKE_VERSION(VNL_ENGINE_VERSION_MAJOR, VNL_ENGINE_VERSION_MINOR,
                                         VNL_ENGINE_VERSION_PATCH),
        .apiVersion = VK_API_VERSION_1_0};

    vkctx->app_info = app_info;
}

static void vk_context_init_instance_create_info(VkContext *vkctx) {
    VkInstanceCreateInfo create_info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                        .pNext = NULL,
                                        .flags = 0,
                                        .pApplicationInfo = &vkctx->app_info,
                                        .enabledLayerCount = 0,
                                        .ppEnabledLayerNames = 0};

    u32 glfw_extension_count = 0;
    const char **glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;

    vkctx->create_info = create_info;
}

static VnlStatus vk_context_init(const VnlConfig *config, VkContext *vkctx) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");

    CLARITY_LOG_INFO("Initializing Vulkan Instance.");

    u32 extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);

    VkInstance instance;

    vk_context_init_app_info(config, vkctx);
    vk_context_init_instance_create_info(vkctx);

    VkResult result = vkCreateInstance(&vkctx->create_info, NULL, &instance);

    CLARITY_ASSERT(result == VK_SUCCESS, "vkCreateInstance failed.");

    if (result != VK_SUCCESS) {
        CLARITY_LOG_WARN("Failed to create Vulkan context.");
        return VNL_ERROR_VULKAN_INSTANCE_CREATION_FAILED;
    }

    vkctx->instance = instance;
    CLARITY_LOG_INFO("Vulkan Instance created successfully.");
    return VNL_SUCCESS;
}

static VkQueueFamilyIndices vk_find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkQueueFamilyIndices indices = {.has_graphics_family = false, .graphics_family = 0};

    // Get the amount of queue families available
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

    /*
    Allocate the necessary memory for the queue families found
    and get the queue families proper
    */
    VkQueueFamilyProperties *queue_families =
        malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);

    CLARITY_ASSERT(queue_families != NULL, "Memory allocation failed for queue_families.");

    if (!queue_families) {
        return indices;
    }
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    for (u32 i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
            indices.has_graphics_family = true;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support) {
            indices.present_family = i;
            indices.has_present_family = true;
        }

        if (indices.has_graphics_family && indices.has_present_family)
            break;
    }

    free(queue_families);
    return indices;
}

static bool vk_is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkQueueFamilyIndices indices = vk_find_queue_families(device, surface);
    return indices.has_graphics_family && indices.has_present_family;
}

static VnlStatus vk_pick_physical_device(VkContext *vkctx) {
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");
    CLARITY_ASSERT(vkctx->instance != VK_NULL_HANDLE, "Vulkan Instance cannot be NULL.");

    CLARITY_LOG_INFO("Picking Vulkan Physical Device.");

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(vkctx->instance, &device_count, NULL);

    CLARITY_ASSERT(device_count > 0, "No GPU with Vulkan support found.");

    if (device_count == 0) {
        CLARITY_LOG_WARN("Failed to find a GPU with Vulkan support.");
        return VNL_ERROR_PHYSICAL_DEVICE_NOT_FOUND;
    }

    VkPhysicalDevice *devices = malloc(sizeof(VkPhysicalDevice) * device_count);

    CLARITY_ASSERT(devices != NULL, "Memory allocation failed for physical devices list.");

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

    free(devices);

    CLARITY_ASSERT(physical_device != VK_NULL_HANDLE, "No suitable GPU found.");

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
    CLARITY_ASSERT(vkctx->physical_device != VK_NULL_HANDLE, "Physical device cannot be NULL.");

    CLARITY_LOG_INFO("Creating Vulkan Logical Device.");

    VkPhysicalDeviceFeatures device_features = {0};
    VkQueueFamilyIndices indices = vk_find_queue_families(vkctx->physical_device, vkctx->surface);

    f32 queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info = {.sType =
                                                     VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                 .pNext = NULL,
                                                 .flags = 0,
                                                 .queueFamilyIndex = indices.graphics_family,
                                                 .queueCount = 1,
                                                 .pQueuePriorities = &queue_priority};

    VkDeviceCreateInfo create_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                      .pNext = NULL,
                                      .flags = 0,
                                      .queueCreateInfoCount = 1,
                                      .pQueueCreateInfos = &queue_create_info,
                                      .enabledLayerCount = 0,
                                      .ppEnabledLayerNames = NULL,
                                      .enabledExtensionCount = 0,
                                      .ppEnabledExtensionNames = NULL,
                                      .pEnabledFeatures = &device_features};

    VkResult result = vkCreateDevice(vkctx->physical_device, &create_info, NULL, &vkctx->device);

    CLARITY_ASSERT(result == VK_SUCCESS, "vkCreateDevice failed.");

    if (result != VK_SUCCESS) {
        CLARITY_LOG_WARN("Failed to create logical device.");
        return VNL_ERROR_LOGICAL_DEVICE_CREATION_FAILED;
    }

    vkGetDeviceQueue(vkctx->device, indices.graphics_family, 0, &vkctx->graphics_queue);

    CLARITY_LOG_INFO("Vulkan Logical Device created successfully.");
    return VNL_SUCCESS;
}

static VnlStatus vk_create_surface(VkContext *vkctx, GLFWwindow *window) {
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");
    CLARITY_ASSERT(window != NULL, "GLFW Window cannot be NULL.");

    CLARITY_LOG_INFO("Creating Vulkan Window Surface.");

    VkResult result = glfwCreateWindowSurface(vkctx->instance, window, NULL, &vkctx->surface);

    CLARITY_ASSERT(result == VK_SUCCESS, "glfwCreateWindowSurface failed.");

    if (result != VK_SUCCESS) {
        CLARITY_LOG_WARN("Failed to create Vulkan window surface.");
        return VNL_ERROR_SURFACE_CREATION_FAILED;
    }

    CLARITY_LOG_INFO("Vulkan Window Surface created successfully.");
    return VNL_SUCCESS;
}

VnlStatus vulkan_init(const VnlConfig *config, GLFWwindow *window, VkContext **out_ctx) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");
    CLARITY_ASSERT(window != NULL, "GLFW Window cannot be NULL.");
    CLARITY_ASSERT(out_ctx != NULL, "out_ctx pointer cannot be NULL.");

    CLARITY_LOG_INFO("Initializing Vulkan Context.");

    VkContext *vkctx = calloc(1, sizeof(VkContext));

    CLARITY_ASSERT(vkctx != NULL, "Memory allocation failed for VkContext.");

    if (!vkctx)
        return VNL_ERROR_OUT_OF_MEMORY;

    VnlStatus status;

    status = vk_context_init(config, vkctx);
    CLARITY_ASSERT(status == VNL_SUCCESS, "vk_context_init failed.");
    if (status != VNL_SUCCESS)
        goto cleanup;

    status = vk_create_surface(vkctx, window);
    CLARITY_ASSERT(status == VNL_SUCCESS, "vk_create_surface failed.");
    if (status != VNL_SUCCESS)
        goto cleanup;

    status = vk_pick_physical_device(vkctx);
    CLARITY_ASSERT(status == VNL_SUCCESS, "vk_pick_physical_device failed.");
    if (status != VNL_SUCCESS)
        goto cleanup;

    status = vk_create_logical_device(vkctx);
    CLARITY_ASSERT(status == VNL_SUCCESS, "vk_create_logical_device failed.");
    if (status != VNL_SUCCESS)
        goto cleanup;

    *out_ctx = vkctx;
    CLARITY_LOG_INFO("Vulkan Context initialized successfully.");
    return VNL_SUCCESS;

cleanup:
    CLARITY_LOG_WARN("Vulkan Context initialization failed. Cleaning up...");
    vulkan_shutdown(vkctx);
    return status;
}

void vulkan_shutdown(VkContext *vkctx) {
    if (vkctx) {
        CLARITY_LOG_INFO("Shutting down Vulkan Context.");

        if (vkctx->device != VK_NULL_HANDLE) {
            vkDestroyDevice(vkctx->device, NULL);
        }
        if (vkctx->instance != VK_NULL_HANDLE && vkctx->surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(vkctx->instance, vkctx->surface, NULL);
        }
        if (vkctx->instance != VK_NULL_HANDLE) {
            vkDestroyInstance(vkctx->instance, NULL);
        }
        free(vkctx);

        CLARITY_LOG_INFO("Vulkan Context shut down successfully.");
    }
}