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
#include <vulkan/vkcommandpool.h>
#include <vulkan/vkframebuffer.h>
#include <vulkan/vkimageview.h>
#include <vulkan/vkpipeline.h>
#include <vulkan/vkqueue.h>
#include <vulkan/vkrenderpass.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vulkan.h>

typedef struct VkContext {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSurfaceKHR surface;
    VkSwapchainInstance swapchain;
    DARRAY(VkImageView) image_views;
    VkPipelineInstance pipeline;
    VkRenderPass render_pass;
    DARRAY(VkFramebuffer) framebuffers;
    VkCommandPool command_pool;
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

static DARRAY(const char *) vk_get_required_ext() {
    u32 ext_count = 0;
    const char **req_glfw_ext;
    req_glfw_ext = glfwGetRequiredInstanceExtensions(&ext_count);

    DARRAY(const char *) req_ext = NULL;

    for (u32 i = 0; i < ext_count; i++) {
        DARRAY_PUSH(req_ext, req_glfw_ext[i]);
    }

#ifdef MIRA_CLARITY_DEBUG
    DARRAY_PUSH(req_ext, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return req_ext;
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

    DARRAY(const char *) req_ext = vk_get_required_ext();

    create_info.enabledExtensionCount = (u32)DARRAY_SIZE(req_ext);
    create_info.ppEnabledExtensionNames = req_ext;

    return create_info;
}

static VnlStatus vk_context_init(const VnlConfig *config, VkContext *vkctx) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");

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
    return VNL_SUCCESS;
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
    bool adeq_swapchain = false;

    if (ext_supported) {
        VkSwapchainInfo sc_info = vk_swapchain_query_support(device, surface);
        adeq_swapchain = DARRAY_SIZE(sc_info.formats) != 0 &&
                         DARRAY_SIZE(sc_info.present_modes) != 0;
    }

    return indices.has_graphics_family && indices.has_present_family &&
           ext_supported && adeq_swapchain;
}

static VnlStatus vk_pick_physical_device(VkContext *vkctx) {
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");
    CLARITY_ASSERT(vkctx->instance != VK_NULL_HANDLE,
                   "Vulkan Instance cannot be NULL.");

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
    return VNL_SUCCESS;
}

static VnlStatus vk_create_logical_device(VkContext *vkctx) {
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");
    CLARITY_ASSERT(vkctx->physical_device != VK_NULL_HANDLE,
                   "Physical device cannot be NULL.");

    VkPhysicalDeviceFeatures device_features = {0};
    VkQueueFamilyIndices indices =
        vk_find_queue_families(vkctx->physical_device, vkctx->surface);

    /**
     * The current approach only allows for two queues, since
     * queue_create_infos is a simple fixed-size array with
     * information to create the graphics and present queues.
     */

    f32 queue_priority = 1.0f;
    DARRAY(VkDeviceQueueCreateInfo) queue_create_infos = NULL;

    VkDeviceQueueCreateInfo queue_create_info = (VkDeviceQueueCreateInfo){
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = indices.graphics_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority};

    DARRAY_PUSH(queue_create_infos, queue_create_info);

    if (indices.graphics_family != indices.present_family) {
        queue_create_info = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .queueFamilyIndex = indices.present_family,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority};

        DARRAY_PUSH(queue_create_infos, queue_create_info);
    }

    DARRAY(const char *) device_ext = NULL;
    DARRAY_PUSH(device_ext, VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueCreateInfoCount = DARRAY_SIZE(queue_create_infos),
        .pQueueCreateInfos = queue_create_infos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = DARRAY_SIZE(device_ext),
        .ppEnabledExtensionNames = device_ext,
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

    return VNL_SUCCESS;
}

static VnlStatus vk_create_surface(VkContext *vkctx, GLFWwindow *window) {
    CLARITY_ASSERT(vkctx != NULL, "VkContext cannot be NULL.");
    CLARITY_ASSERT(window != NULL, "GLFW Window cannot be NULL.");

    VkResult result =
        glfwCreateWindowSurface(vkctx->instance, window, NULL, &vkctx->surface);

    if (result != VK_SUCCESS) {
        CLARITY_LOG_WARN("Failed to create Vulkan window surface.");
        return VNL_ERROR_SURFACE_CREATION_FAILED;
    }

    return VNL_SUCCESS;
}

VnlStatus vulkan_init(const VnlConfig *config, GLFWwindow *window,
                      VkContext **out_ctx) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");
    CLARITY_ASSERT(window != NULL, "GLFW Window cannot be NULL.");
    CLARITY_ASSERT(out_ctx != NULL, "out_ctx pointer cannot be NULL.");

    VkContext *vkctx = CLARITY_MALLOC(sizeof(VkContext));
    if (vkctx) {
        memset(vkctx, 0, sizeof(VkContext));
    }

    if (!vkctx)
        return VNL_ERROR_OUT_OF_MEMORY;

    /*
    typedef struct VkContext {
        VkInstance instance;
        VkPhysicalDevice physical_device;
        VkDevice device;
        VkQueue graphics_queue;
        VkQueue present_queue;
        VkSurfaceKHR surface;
        VkSwapchainInstance swapchain;
        DARRAY(VkImageView) views;
        VkPipelineLayout pipeline_layout;
        VkRenderPass render_pass;
    } VkContext;
    */

    vkctx->instance = VK_NULL_HANDLE;
    vkctx->physical_device = VK_NULL_HANDLE;
    vkctx->device = VK_NULL_HANDLE;
    vkctx->graphics_queue = VK_NULL_HANDLE;
    vkctx->present_queue = VK_NULL_HANDLE;
    vkctx->surface = VK_NULL_HANDLE;
    vkctx->swapchain.swapchain = VK_NULL_HANDLE;
    vkctx->swapchain.format = VK_FORMAT_UNDEFINED;
    vkctx->swapchain.extent = (VkExtent2D){0, 0};
    vkctx->swapchain.images = NULL;
    vkctx->image_views = NULL;
    vkctx->render_pass = VK_NULL_HANDLE;
    vkctx->pipeline.pipeline = VK_NULL_HANDLE;
    vkctx->pipeline.layout = VK_NULL_HANDLE;
    vkctx->framebuffers = NULL;
    vkctx->command_pool = VK_NULL_HANDLE;

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

    VkSwapchainDesc swapchain_desc = {
        .physical_device = vkctx->physical_device,
        .device = vkctx->device,
        .surface = vkctx->surface,
        .window = window,
    };
    status = vk_swapchain_create(&swapchain_desc, &vkctx->swapchain);
    if (status != VNL_SUCCESS)
        goto cleanup;

    VkImageViewDesc image_view_desc = {
        .device = vkctx->device,
        .images = vkctx->swapchain.images,
        .format = vkctx->swapchain.format,
    };
    status = vk_image_view_create(&image_view_desc, &vkctx->image_views);
    if (status != VNL_SUCCESS)
        goto cleanup;

    VkRenderPassDesc render_pass_desc = {
        .device = vkctx->device,
        .format = vkctx->swapchain.format,
    };
    status = vk_render_pass_create(&render_pass_desc, &vkctx->render_pass);
    if (status != VNL_SUCCESS)
        goto cleanup;

    VkPipelineDesc pipeline_desc = {
        .device = vkctx->device,
        .render_pass = vkctx->render_pass,
        .extent = vkctx->swapchain.extent,
    };
    status = vk_pipeline_create(&pipeline_desc, &vkctx->pipeline);
    if (status != VNL_SUCCESS)
        goto cleanup;

    VkFramebufferDesc framebuffer_desc = {.device = vkctx->device,
                                          .image_views = vkctx->image_views,
                                          .render_pass = vkctx->render_pass,
                                          .extent = vkctx->swapchain.extent};
    status = vk_framebuffers_create(&framebuffer_desc, &vkctx->framebuffers);
    if (status != VNL_SUCCESS)
        goto cleanup;

    VkCommandPoolDesc command_pool_desc = {.device = vkctx->device,
                                           .physical_device =
                                               vkctx->physical_device,
                                           .surface = vkctx->surface};
    status = vk_command_pool_create(&command_pool_desc, &vkctx->command_pool);
    if (status != VNL_SUCCESS)
        goto cleanup;

    *out_ctx = vkctx;
    return VNL_SUCCESS;

cleanup:
    CLARITY_LOG_ERROR("Vulkan Context initialization failed. Cleaning up...");
    vulkan_shutdown(vkctx);
    return status;
}

void vulkan_shutdown(VkContext *vkctx) {
    if (vkctx) {
        if (vkctx->command_pool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(vkctx->device, vkctx->command_pool, NULL);
        }
        if (vkctx->framebuffers != NULL) {
            DARRAY_FOREACH(VkFramebuffer, framebuffer, vkctx->framebuffers) {
                vkDestroyFramebuffer(vkctx->device, *framebuffer, NULL);
            }
        }
        if (vkctx->pipeline.pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(vkctx->device, vkctx->pipeline.pipeline, NULL);
            vkctx->pipeline.pipeline = VK_NULL_HANDLE;
        }
        if (vkctx->pipeline.layout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(vkctx->device, vkctx->pipeline.layout,
                                    NULL);
            vkctx->pipeline.layout = VK_NULL_HANDLE;
        }
        if (vkctx->render_pass != VK_NULL_HANDLE) {
            vkDestroyRenderPass(vkctx->device, vkctx->render_pass, NULL);
            vkctx->render_pass = VK_NULL_HANDLE;
        }
        if (vkctx->image_views != NULL) {
            DARRAY_FOREACH(VkImageView, view, vkctx->image_views) {
                if (*view != VK_NULL_HANDLE) {
                    vkDestroyImageView(vkctx->device, *view, NULL);
                }
            }
            DARRAY_FREE(vkctx->image_views);
        }
        if (vkctx->swapchain.images != NULL) {
            DARRAY_FREE(vkctx->swapchain.images);
        }
        if (vkctx->swapchain.swapchain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(vkctx->device, vkctx->swapchain.swapchain,
                                  NULL);
            vkctx->swapchain.swapchain = VK_NULL_HANDLE;
        }
        if (vkctx->device != VK_NULL_HANDLE) {
            vkDestroyDevice(vkctx->device, NULL);
            vkctx->device = VK_NULL_HANDLE;
        }
        if (vkctx->instance != VK_NULL_HANDLE &&
            vkctx->surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(vkctx->instance, vkctx->surface, NULL);
            vkctx->surface = VK_NULL_HANDLE;
        }
        if (vkctx->instance != VK_NULL_HANDLE) {
            vkDestroyInstance(vkctx->instance, NULL);
            vkctx->instance = VK_NULL_HANDLE;
        }
        CLARITY_FREE(vkctx);
    }
}