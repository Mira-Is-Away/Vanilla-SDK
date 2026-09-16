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

#include <core/vnl_status.h>
#include <vulkan/vulkan.h>

#include <mira/darray.h>
#include <renderer/vulkan/vkcommandbuffers.h>
#include <renderer/vulkan/vkcommandpool.h>
#include <renderer/vulkan/vkframebuffer.h>
#include <renderer/vulkan/vkimageview.h>
#include <renderer/vulkan/vkpipeline.h>
#include <renderer/vulkan/vkqueue.h>
#include <renderer/vulkan/vkrenderpass.h>
#include <renderer/vulkan/vkswapchain.h>
#include <renderer/vulkan/vksync.h>

typedef struct VnlConfig VnlConfig;
typedef struct VnlWinMan VnlWinMan;

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
    VkSync                  sync;
    u32                     current_frame;
} VkContext;

/**
 * @brief Initialises the Vulkan context and creates core Vulkan objects.
 * @param[in] config The engine configuration settings.
 * @param[in] winman The window manager handle to create the surface and
 * swapchain from.
 * @param[out] out_ctx Pointer to store the created VkContext.
 * @retval VNL_SUCCESS If context initialisation was successful.
 */
VnlStatus vulkan_init(const VnlConfig *config, VnlWinMan *winman,
                      VkContext **out_ctx);

/**
 * @brief Shuts down the Vulkan context and destroys all associated Vulkan
 * objects.
 * @param[in] vkctx The Vulkan context to destroy.
 */
void vulkan_shutdown(VkContext *vkctx);

#endif