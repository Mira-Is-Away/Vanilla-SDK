/**
 * @file vkrenderpass.h
 *
 * Creation, configuration, and execution of Vulkan render passes.
 *
 * @author Henry R
 * @date 2026-09-14
 */

#ifndef VNL_VULKAN_VKRENDERPASS_H_
#define VNL_VULKAN_VKRENDERPASS_H_

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

/**
 * @struct VkRenderPassDesc
 * @brief Descriptor containing parameters for render pass creation.
 */
typedef struct {
    VkDevice device;
    VkFormat format;
} VkRenderPassDesc;

/**
 * @brief Creates a single-subpass color render pass.
 * @param[in] desc Pointer to the render pass descriptor.
 * @param[out] out_rp Pointer to store the created render pass handle.
 * @retval VNL_SUCCESS If render pass creation was successful.
 * @retval VNL_ERROR_RENDER_PASS_CREATION_FAILED If render pass creation failed.
 */
VnlStatus vk_render_pass_create(const VkRenderPassDesc *desc,
                                VkRenderPass           *out_rp);

/**
 * @struct VkRenderPassBeginDesc
 * @brief Descriptor containing parameters for beginning a render pass execution.
 */
typedef struct {
    VkCommandBuffer       command_buffer;
    VkRenderPass          render_pass;
    DARRAY(VkFramebuffer) framebuffers;
    u32                   image_index;
    VnlVec2               offset;
    VkExtent2D            extent;
    VnlColour             clear_colour;
} VkRenderPassBeginDesc;

/**
 * @brief Begins the specified render pass on a command buffer.
 * @param[in] desc Pointer to the render pass begin descriptor.
 */
void vk_render_pass_begin(const VkRenderPassBeginDesc *desc);

/**
 * @brief Ends the active render pass on the given command buffer.
 * @param[in] command_buffer The command buffer recording the render pass.
 */
void vk_render_pass_end(VkCommandBuffer command_buffer);

#endif