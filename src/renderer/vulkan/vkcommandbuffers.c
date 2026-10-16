#include <renderer/vulkan/vkcommandbuffers.h>

#include <mira/vnl_status.h>
#include <mira/vnl_types.h>
#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>
#include <mira/darray.h>
#include <renderer/vulkan/vkrenderpass.h>
#include <vulkan/vulkan.h>

static void _transition_image_layout(VkCommandBuffer cmd_buffer, VkImage image,
                                     VkImageLayout         old_layout,
                                     VkImageLayout         new_layout,
                                     VkAccessFlags2        src_access_mask,
                                     VkAccessFlags2        dst_access_mask,
                                     VkPipelineStageFlags2 src_stage_mask,
                                     VkPipelineStageFlags2 dst_stage_mask) {
    VkImageMemoryBarrier2 barrier = (VkImageMemoryBarrier2){
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext               = VK_NULL_HANDLE,
        .srcStageMask        = src_stage_mask,
        .srcAccessMask       = src_access_mask,
        .dstStageMask        = dst_stage_mask,
        .dstAccessMask       = dst_access_mask,
        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = image,
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    VkDependencyInfo dependency_info = {
        .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .dependencyFlags         = 0,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers    = &barrier,
    };

    vkCmdPipelineBarrier2(cmd_buffer, &dependency_info);
}

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

    _transition_image_layout(
        desc->command_buffer, desc->image, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);

    VkRenderingAttachmentInfo rendering_attachment_info = {
        .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext       = VK_NULL_HANDLE,
        .imageView   = desc->image_view,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue  = *(const VkClearValue *)&desc->clear_colour,
    };

    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea =
            {
                .offset = {0, 0},
                .extent = desc->extent,
            },
        .layerCount           = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &rendering_attachment_info,
    };

    vkCmdBeginRendering(desc->command_buffer, &rendering_info);

    vkCmdBindPipeline(desc->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      desc->pipeline);

    VkViewport viewport = {
        .x        = 0.0f,
        .y        = 0.0f,
        .width    = (f32)desc->extent.width,
        .height   = (f32)desc->extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    vkCmdSetViewport(desc->command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = desc->extent,
    };

    vkCmdSetScissor(desc->command_buffer, 0, 1, &scissor);

    vkCmdDraw(desc->command_buffer, 3, 1, 0, 0);

    vkCmdEndRendering(desc->command_buffer);

    _transition_image_layout(
        desc->command_buffer, desc->image,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        0, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

    if (vkEndCommandBuffer(desc->command_buffer) != VK_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to record Vulkan command buffer.");
        return VNL_ERROR_COMMAND_BUFFER_END_RECORDING_FAILED;
    }

    return VNL_SUCCESS;
}

void vk_command_buffer_reset(VkCommandBuffer cmd_buffer) {
    vkResetCommandBuffer(cmd_buffer, 0);
}