/**
 * @file vkcommandbuffers.h
 *
 * Creation, recording, and management of Vulkan command buffers.
 *
 * @author Henry R
 * @date 2026-09-14
 */

#ifndef VNL_VULKAN_VKCOMMANDBUFFERS_H_
#define VNL_VULKAN_VKCOMMANDBUFFERS_H_

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <mira/darray.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vulkan.h>

/**
 * @struct VkCommandBufferDesc
 * @brief Descriptor for allocating Vulkan command buffers from a command pool.
 */
typedef struct {
    VkDevice      device;
    VkCommandPool pool;
    u32           command_buffer_count;
} VkCommandBufferDesc;

/**
 * @brief Allocates command buffers from the specified command pool.
 * @param[in] desc Pointer to the command buffer allocation descriptor.
 * @param[out] out_buffers Pointer to a dynamic array to store the allocated
 * command buffers.
 * @retval VNL_SUCCESS If command buffer allocation was successful.
 * @retval VNL_ERROR_COMMAND_BUFFER_CREATION_FAILED If allocation failed.
 */
VnlStatus vk_command_buffers_create(const VkCommandBufferDesc *desc,
                                    DARRAY(VkCommandBuffer)   *out_buffers);

/**
 * @struct VkCommandBufferRecordDesc
 * @brief Descriptor containing parameters for recording drawing commands into a
 * command buffer.
 */
typedef struct {
    VkCommandBuffer       command_buffer;
    VkRenderPass          render_pass;
    DARRAY(VkFramebuffer) framebuffers;
    u32                   image_index;
    VnlVec2               offset;
    VkExtent2D            extent;
    VnlColour             clear_colour;
    VkPipeline            pipeline;
} VkCommandBufferRecordDesc;

/**
 * @brief Records standard render pass and drawing commands into a command
 * buffer.
 * @param[in] desc Pointer to the command buffer recording descriptor.
 * @retval VNL_SUCCESS If recording was successful.
 * @retval VNL_ERROR_COMMAND_BUFFER_BEGIN_RECORDING_FAILED If starting the
 * command buffer failed.
 * @retval VNL_ERROR_COMMAND_BUFFER_END_RECORDING_FAILED If finishing the
 * command buffer failed.
 */
VnlStatus vk_command_buffer_record(const VkCommandBufferRecordDesc *desc);

/**
 * @brief Resets a command buffer to the initial state.
 * @param[in] cmd_buffer The command buffer handle to reset.
 */
void vk_command_buffer_reset(VkCommandBuffer cmd_buffer);

#endif