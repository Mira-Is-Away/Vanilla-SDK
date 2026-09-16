#include <renderer/vulkan/vksync.h>

#include <stdint.h>
#include <string.h>

#include <core/vnl_status.h>
#include <mira/clarity.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

VnlStatus vk_sync_create(const VkSyncDesc *desc, VkSync *out_sync) {
    CLARITY_ASSERT(desc != NULL, "Sync desc cannot be NULL.");
    CLARITY_ASSERT(desc->device != VK_NULL_HANDLE, "Device cannot be NULL.");
    CLARITY_ASSERT(desc->swapchain_image_count > 0,
                   "Swapchain image count must be greater than 0.");
    CLARITY_ASSERT(out_sync != NULL, "out_sync pointer cannot be NULL.");

    memset(out_sync, 0, sizeof(VkSync));
    out_sync->device = desc->device;

    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (u32 i = 0; i < VNL_MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(desc->device, &semaphore_info, NULL,
                              &out_sync->frames[i].image_available) !=
            VK_SUCCESS) {
            CLARITY_LOG_ERROR("Failed to create image available semaphore.");
            vk_sync_destroy(out_sync);
            return VNL_ERROR_SYNC_OBJECTS_CREATION_FAILED;
        }

        if (vkCreateFence(desc->device, &fence_info, NULL,
                          &out_sync->frames[i].in_flight) != VK_SUCCESS) {
            CLARITY_LOG_ERROR("Failed to create in-flight fence.");
            vk_sync_destroy(out_sync);
            return VNL_ERROR_SYNC_OBJECTS_CREATION_FAILED;
        }
    }

    for (u32 i = 0; i < desc->swapchain_image_count; i++) {
        VkSemaphore sem = VK_NULL_HANDLE;
        if (vkCreateSemaphore(desc->device, &semaphore_info, NULL, &sem) !=
            VK_SUCCESS) {
            CLARITY_LOG_ERROR("Failed to create render finished semaphore.");
            vk_sync_destroy(out_sync);
            return VNL_ERROR_SYNC_OBJECTS_CREATION_FAILED;
        }
        DARRAY_PUSH(out_sync->render_finished, sem);
    }

    return VNL_SUCCESS;
}

void vk_sync_destroy(VkSync *sync) {
    if (!sync || sync->device == VK_NULL_HANDLE) {
        return;
    }

    for (u32 i = 0; i < VNL_MAX_FRAMES_IN_FLIGHT; i++) {
        if (sync->frames[i].image_available != VK_NULL_HANDLE) {
            vkDestroySemaphore(sync->device, sync->frames[i].image_available,
                               NULL);
            sync->frames[i].image_available = VK_NULL_HANDLE;
        }
        if (sync->frames[i].in_flight != VK_NULL_HANDLE) {
            vkDestroyFence(sync->device, sync->frames[i].in_flight, NULL);
            sync->frames[i].in_flight = VK_NULL_HANDLE;
        }
    }

    if (sync->render_finished != NULL) {
        for (u32 i = 0; i < (u32)DARRAY_SIZE(sync->render_finished); i++) {
            if (sync->render_finished[i] != VK_NULL_HANDLE) {
                vkDestroySemaphore(sync->device, sync->render_finished[i],
                                   NULL);
            }
        }
        DARRAY_FREE(sync->render_finished);
        sync->render_finished = NULL;
    }

    sync->device = VK_NULL_HANDLE;
}

void vk_wait_in_flight(const VkSync *sync, u32 frame_index) {
    CLARITY_ASSERT(sync != NULL, "Sync pointer cannot be NULL.");
    CLARITY_ASSERT(frame_index < VNL_MAX_FRAMES_IN_FLIGHT,
                   "Frame index out of bounds.");
    vkWaitForFences(sync->device, 1, &sync->frames[frame_index].in_flight,
                    VK_TRUE, UINT64_MAX);
}

void vk_reset_in_flight(VkSync *sync, u32 frame_index) {
    CLARITY_ASSERT(sync != NULL, "Sync pointer cannot be NULL.");
    CLARITY_ASSERT(frame_index < VNL_MAX_FRAMES_IN_FLIGHT,
                   "Frame index out of bounds.");
    vkResetFences(sync->device, 1, &sync->frames[frame_index].in_flight);
}