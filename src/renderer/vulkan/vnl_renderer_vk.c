/**
 * @file vnl_renderer_vk.c
 *
 * Vulkan implementation of the Vanilla renderer subsystem.
 *
 * @author Henry R
 * @date 2026-09-08
 */

#include <renderer/vnl_renderer.h>

#include <stdint.h>

#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>
#include <mira/darray.h>

#include <mira/vnl_status.h>
#include <mira/vnl_types.h>
#include <renderer/vulkan/vkcommandbuffers.h>
#include <renderer/vulkan/vkcontext.h>
#include <renderer/vulkan/vksync.h>
#include <vulkan/vulkan.h>

struct VnlRenderer {
    VkContext                *vkctx;
    u32                       swapchain_image_index;
    VnlColour                 clear_colour;
    VkCommandBufferRecordDesc record_desc;
};

VnlStatus vnl_renderer_init(const VnlConfig *config, VnlWinMan *winman,
                            VnlRenderer **out_renderer) {
    CLARITY_ASSERT(config != NULL, "Config handle cannot be NULL.");
    CLARITY_ASSERT(winman != NULL, "Window manager handle cannot be NULL.");
    CLARITY_ASSERT(out_renderer != NULL,
                   "out_renderer pointer cannot be NULL.");

    VnlRenderer *renderer = CLARITY_MALLOC(sizeof(VnlRenderer));
    if (!renderer) {
        return VNL_ERROR_OUT_OF_MEMORY;
    }

    VnlStatus status = vulkan_init(config, winman, &renderer->vkctx);
    if (status != VNL_SUCCESS) {
        CLARITY_FREE(renderer);
        return status;
    }

    renderer->clear_colour          = config->clear_colour;
    renderer->swapchain_image_index = 0;

    *out_renderer = renderer;
    return VNL_SUCCESS;
}

void vnl_renderer_clear_image(VnlRenderer *renderer) {
    CLARITY_ASSERT(renderer != NULL, "Renderer handle cannot be NULL.");
    VkContext      *vk            = renderer->vkctx;
    u32             current_frame = vk->current_frame;
    VkCommandBuffer cmd_buffer    = vk->command_buffers[current_frame];
    VkImage image = vk->swapchain.images[renderer->swapchain_image_index];

    vkCmdClearColorImage(cmd_buffer, image,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                         (VkClearColorValue *)&renderer->clear_colour, 0, NULL);
}

VnlStatus vnl_renderer_begin_rendering(VnlRenderer *renderer) {
    CLARITY_ASSERT(renderer != NULL, "Renderer handle cannot be NULL.");
    CLARITY_ASSERT(renderer->vkctx != NULL,
                   "Vulkan Context handle cannot be NULL.");

    VkContext *vk            = renderer->vkctx;
    u32        current_frame = vk->current_frame;

    vk_wait_in_flight(&vk->sync, current_frame);

    if (vkAcquireNextImageKHR(vk->device, vk->swapchain.swapchain, UINT64_MAX,
                              vk->sync.frames[current_frame].image_available,
                              VK_NULL_HANDLE,
                              &renderer->swapchain_image_index)) {
        CLARITY_LOG_ERROR("Failed to acquire next swapchain image.");
        return VNL_FAILURE;
    }

    vk_reset_in_flight(&vk->sync, current_frame);

    vk_command_buffer_reset(vk->command_buffers[current_frame]);

    renderer->record_desc = (VkCommandBufferRecordDesc){
        .command_buffer = vk->command_buffers[current_frame],
        .image          = vk->swapchain.images[renderer->swapchain_image_index],
        .image_view     = vk->image_views[renderer->swapchain_image_index],
        .offset         = (VnlVec2){0, 0},
        .extent         = vk->swapchain.extent,
        .pipeline       = vk->pipeline.pipeline,
        .clear_colour   = renderer->clear_colour,
    };

    VnlStatus status;
    if ((status = vk_command_buffer_begin_recording(&renderer->record_desc)) !=
        VNL_SUCCESS) {
        return status;
    }

    return VNL_SUCCESS;
}

void vnl_renderer_draw(VnlRenderer *renderer) {
    CLARITY_ASSERT(renderer != NULL, "Renderer handle cannot be NULL.");
    CLARITY_ASSERT(renderer->vkctx != NULL,
                   "Vulkan Context handle cannot be NULL.");

    vk_command_buffer_draw_batch(&renderer->record_desc);
}

VnlStatus vnl_renderer_end_rendering(VnlRenderer *renderer) {
    CLARITY_ASSERT(renderer != NULL, "Renderer handle cannot be NULL.");
    CLARITY_ASSERT(renderer->vkctx != NULL,
                   "Vulkan Context handle cannot be NULL.");

    VkContext *vk            = renderer->vkctx;
    u32        current_frame = vk->current_frame;
    u32        image_index   = renderer->swapchain_image_index;

    VnlStatus status;
    if ((status = vk_command_buffer_end_recording(&renderer->record_desc)) !=
        VNL_SUCCESS) {
        return status;
    }

    VkSemaphore wait_semaphores[] = {
        vk->sync.frames[current_frame].image_available};
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signal_semaphores[] = {vk->sync.render_finished[image_index]};

    VkSubmitInfo submit_info = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = wait_semaphores,
        .pWaitDstStageMask    = wait_stages,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &vk->command_buffers[current_frame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = signal_semaphores,
    };

    if (vkQueueSubmit(vk->graphics_queue, 1, &submit_info,
                      vk->sync.frames[current_frame].in_flight) != VK_SUCCESS) {
        CLARITY_LOG_ERROR(
            "Failed to submit draw command buffer to graphics queue.");
        return VNL_FAILURE;
    }

    VkSwapchainKHR swapchains[] = {vk->swapchain.swapchain};

    VkPresentInfoKHR present_info = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = signal_semaphores,
        .swapchainCount     = 1,
        .pSwapchains        = swapchains,
        .pImageIndices      = &image_index,
        .pResults           = NULL,
    };

    if (vkQueuePresentKHR(vk->present_queue, &present_info) != VK_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to present swapchain image ().");
        return VNL_FAILURE;
    }

    vk->current_frame = (vk->current_frame + 1) % VNL_MAX_FRAMES_IN_FLIGHT;

    return VNL_SUCCESS;
}

/*
VnlStatus vnl_renderer_draw(VnlRenderer *renderer) {
    CLARITY_ASSERT(renderer != NULL, "Renderer handle cannot be NULL.");
    CLARITY_ASSERT(renderer->vkctx != NULL, "VkContext cannot be NULL.");

    VkContext *vk            = renderer->vkctx;
    u32        current_frame = vk->current_frame;
    u32        image_index   = 0;

    vk_wait_in_flight(&vk->sync, current_frame);

    if (vkAcquireNextImageKHR(vk->device, vk->swapchain.swapchain, UINT64_MAX,
                              vk->sync.frames[current_frame].image_available,
                              VK_NULL_HANDLE, &image_index)) {
        CLARITY_LOG_ERROR("Failed to acquire next swapchain image.");
        return VNL_FAILURE;
    }

    vk_reset_in_flight(&vk->sync, current_frame);

    vk_command_buffer_reset(vk->command_buffers[current_frame]);

    VkCommandBufferRecordDesc record_desc = {
        .command_buffer = vk->command_buffers[current_frame],
        .image          = vk->swapchain.images[image_index],
        .image_view     = vk->image_views[image_index],
        .offset         = (VnlVec2){0, 0},
        .extent         = vk->swapchain.extent,
        .pipeline       = vk->pipeline.pipeline,
        .clear_colour   = renderer->clear_colour,
    };

    VnlStatus status;
    if ((status = vk_command_buffer_begin_recording(&record_desc)) !=
        VNL_SUCCESS) {
        return status;
    }

    vk_command_buffer_draw_batch(&record_desc);

    if ((status = vk_command_buffer_end_recording(&record_desc)) !=
        VNL_SUCCESS) {
        return status;
    }

    VkSemaphore wait_semaphores[] = {
        vk->sync.frames[current_frame].image_available};
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signal_semaphores[] = {vk->sync.render_finished[image_index]};

    VkSubmitInfo submit_info = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = wait_semaphores,
        .pWaitDstStageMask    = wait_stages,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &vk->command_buffers[current_frame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = signal_semaphores,
    };

    if (vkQueueSubmit(vk->graphics_queue, 1, &submit_info,
                      vk->sync.frames[current_frame].in_flight) != VK_SUCCESS) {
        CLARITY_LOG_ERROR(
            "Failed to submit draw command buffer to graphics queue.");
        return VNL_FAILURE;
    }

    VkSwapchainKHR swapchains[] = {vk->swapchain.swapchain};

    VkPresentInfoKHR present_info = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = signal_semaphores,
        .swapchainCount     = 1,
        .pSwapchains        = swapchains,
        .pImageIndices      = &image_index,
        .pResults           = NULL,
    };

    if (vkQueuePresentKHR(vk->present_queue, &present_info) != VK_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to present swapchain image.");
        return VNL_FAILURE;
    }

    vk->current_frame = (vk->current_frame + 1) % VNL_MAX_FRAMES_IN_FLIGHT;

    return VNL_SUCCESS;
}
*/

void vnl_renderer_shutdown(VnlRenderer *renderer) {
    if (!renderer) {
        return;
    }

    if (renderer->vkctx) {
        if (renderer->vkctx->device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(renderer->vkctx->device);
        }
        vulkan_shutdown(renderer->vkctx);
        renderer->vkctx = NULL;
    }

    CLARITY_FREE(renderer);
}
