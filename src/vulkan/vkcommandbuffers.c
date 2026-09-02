#include <vulkan/vkcommandbuffers.h>

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>
#include <vulkan/vulkan.h>

VnlStatus vk_command_buffers_create(const VkCommandBufferDesc *desc,
                                    VkCommandBuffer *out_buffers) {

    VkCommandBufferAllocateInfo alloc_info = (VkCommandBufferAllocateInfo){
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = desc->pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = desc->command_buffer_count
    };

    VkCommandBuffer command_buffer;
    if (vkAllocateCommandBuffers(desc->device, &alloc_info, &command_buffer) !=
        VK_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to create Vulkan command buffers.");
        return VNL_ERROR_COMMAND_BUFFER_CREATION_FAILED;
    }

    *out_buffers = command_buffer;

    return VNL_SUCCESS;
}

