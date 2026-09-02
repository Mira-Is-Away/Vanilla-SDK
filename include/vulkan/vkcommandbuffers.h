#ifndef VNL_VULKAN_VKCOMMANDBUFFERS_H_
#define VNL_VULKAN_VKCOMMANDBUFFERS_H_

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <mira/darray.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vulkan.h>

typedef struct {
    VkDevice      device;
    VkCommandPool pool;
    u32           command_buffer_count;
} VkCommandBufferDesc;

VnlStatus vk_command_buffers_create(const VkCommandBufferDesc *desc,
                                    DARRAY(VkCommandBuffer)   *out_buffers);

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

VnlStatus vk_command_buffer_record(const VkCommandBufferRecordDesc *desc);

#endif