#include <vulkan/vkcommandbuffers.h>

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>
#include <mira/darray.h>
#include <vulkan/vkrenderpass.h>
#include <vulkan/vulkan.h>

VnlStatus vk_command_buffers_create(const VkCommandBufferDesc *desc,
                                    DARRAY(VkCommandBuffer)   *out_buffers) {

    CLARITY_ASSERT(desc != NULL, "VkCommandBufferDesc cannot be NULL.");

    VkCommandBufferAllocateInfo alloc_info = (VkCommandBufferAllocateInfo){
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = desc->pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = desc->command_buffer_count,
    };

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

VnlStatus vk_command_buffer_record(const VkCommandBufferRecordDesc *desc) {

    VkCommandBufferBeginInfo begin_info = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags            = 0,
        .pInheritanceInfo = NULL,
    };

    if (vkBeginCommandBuffer(desc->command_buffer, &begin_info) != VK_SUCCESS) {
        CLARITY_LOG_ERROR(
            "Failed to begin recording into Vulkan command buffer!");
        return VNL_ERROR_COMMAND_BUFFER_BEGIN_RECORDING_FAILED;
    }

    VkRenderPassBeginDesc render_pass_desc = {
        .command_buffer = desc->command_buffer,
        .render_pass    = desc->render_pass,
        .framebuffers   = desc->framebuffers,
        .image_index    = desc->image_index,
        .offset         = {0, 0},
        .extent         = desc->extent,
        .clear_colour   = {1.0f, 1.0f, 1.0f, 1.0f},
    };
    vk_render_pass_begin(&render_pass_desc);

    vkCmdBindPipeline(desc->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      desc->pipeline);

    return VNL_SUCCESS;
}