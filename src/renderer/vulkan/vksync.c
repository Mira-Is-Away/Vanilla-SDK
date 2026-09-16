#include <renderer/vulkan/vksync.h>

#include <stdint.h>

#include <core/vnl_status.h>
#include <mira/clarity.h>
#include <vulkan/vulkan.h>

VnlStatus vk_sync_create(const VkSyncDesc *desc, VkSync *out_sync) {
    VkSemaphoreCreateInfo semaphore_info = (VkSemaphoreCreateInfo){
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkFenceCreateInfo fence_info = (VkFenceCreateInfo){
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    VkResult status = VK_SUCCESS;

    status = vkCreateSemaphore(desc->device, &semaphore_info, NULL,
                               &out_sync->image_available);
    status = vkCreateSemaphore(desc->device, &semaphore_info, NULL,
                               &out_sync->render_finished);
    status =
        vkCreateFence(desc->device, &fence_info, NULL, &out_sync->in_flight);

    if (status != VK_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to create Vulkan synchronisation objects.");
        return VNL_ERROR_SYNC_OBJECTS_CREATION_FAILED;
    }

    out_sync->device = desc->device;

    return VNL_SUCCESS;
}

void vk_sync_destroy(VkSync sync) {
    vkDestroySemaphore(sync.device, sync.image_available, NULL);
    vkDestroySemaphore(sync.device, sync.render_finished, NULL);
    vkDestroyFence(sync.device, sync.in_flight, NULL);
}

void vk_wait_in_flight(const VkSync *sync) {
    vkWaitForFences(sync->device, 1, &sync->in_flight, VK_TRUE, UINT64_MAX);
}

void vk_reset_in_flight(VkSync *sync) {
    vkResetFences(sync->device, 1, &sync->in_flight);
}