#ifndef VNL_VULKAN_VKCOMMANDBUFFERS_H_
#define VNL_VULKAN_VKCOMMANDBUFFERS_H_

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

typedef struct {
    VkDevice device;
    VkCommandPool pool;
    u32 command_buffer_count;
} VkCommandBufferDesc;

VnlStatus vk_command_buffers_create(const VkCommandBufferDesc *desc,
                                    DARRAY(VkCommandBuffer) *out_buffers);

#endif