#include <vulkan/vkcommandbuffers.h>

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

VnlStatus vk_command_buffers_create(const VkCommandBufferDesc *desc,
                                    DARRAY(VkCommandBuffer) * out_buffers) {

    VkCommandBufferAllocateInfo alloc_info = (VkCommandBufferAllocateInfo){
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = desc->pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = desc->command_buffer_count};

    VkCommandBuffer *command_buffers =
        CLARITY_MALLOC(desc->command_buffer_count * sizeof(VkCommandBuffer));
    if (vkAllocateCommandBuffers(desc->device, &alloc_info, command_buffers) !=
        VK_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to create Vulkan command buffers.");
        return VNL_ERROR_COMMAND_BUFFER_CREATION_FAILED;
    }

    for (u32 i = 0; i < desc->command_buffer_count; i++) {
        DARRAY_PUSH(*out_buffers, command_buffers[i]);
    }

    CLARITY_FREE(command_buffers);

    return VNL_SUCCESS;
}

/*
VnlStatus vk_command_buffer_record(VkCommandBuffer buffer, u32 image_index) {

}*/
