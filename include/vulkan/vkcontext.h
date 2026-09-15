/**
 * @file vkcontext.h
 *
 * The structure and functions dedicated to storing
 * Vulkan context information.
 *
 * @author Henry R
 * @date 2025-12-09
 */

#ifndef VANILLA_VULKAN_VKCONTEXT_H_
#define VANILLA_VULKAN_VKCONTEXT_H_

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <core/vnl_status.h>
#include <vulkan/vulkan.h>

#include <mira/darray.h>
#include <vulkan/vkpipeline.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vksync.h>

typedef struct VkQueueFamilyIndices VkQueueFamilyIndices;
typedef struct VnlConfig            VnlConfig;

/**
 * @struct VkContext
 * @brief Holds pointers and handles to Vulkan context information.
 */
typedef struct VkContext {
    VkInstance              instance;
    VkPhysicalDevice        physical_device;
    VkDevice                device;
    VkQueue                 graphics_queue;
    VkQueue                 present_queue;
    VkSurfaceKHR            surface;
    VkSwapchainInstance     swapchain;
    DARRAY(VkImageView)     image_views;
    VkPipelineInstance      pipeline;
    VkRenderPass            render_pass;
    DARRAY(VkFramebuffer)   framebuffers;
    VkCommandPool           command_pool;
    DARRAY(VkCommandBuffer) command_buffers;
    VkSync                  sync[VNL_MAX_FRAMES_IN_FLIGHT];
    u32                     current_frame;
} VkContext;

/**
 * @brief Initialises the Vulkan context and creates core Vulkan objects.
 * @param[in] config The engine configuration settings.
 * @param[in] window The GLFW window handle to create the surface for.
 * @param[out] out_ctx Pointer to store the created VkContext.
 * @retval VNL_SUCCESS If context initialisation was successful.
 */
VnlStatus vulkan_init(const VnlConfig *config, GLFWwindow *window,
                      VkContext **out_ctx);

/**
 * @brief Shuts down the Vulkan context and destroys all associated Vulkan objects.
 * @param[in] vkctx The Vulkan context to destroy.
 */
void vulkan_shutdown(VkContext *vkctx);

#endif