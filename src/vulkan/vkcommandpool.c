#include <vulkan/vkcommandpool.h>

#include <core/vnl_status.h>
#include <mira/clarity.h>
#include <vulkan/vkqueue.h>

VnlStatus vk_command_pool_create(const VkCommandPoolDesc *desc,
                                 VkCommandPool           *out_command_pool) {
    VkQueueFamilyIndices queue_family_indices =
        vk_find_queue_families(desc->physical_device, desc->surface);

    VkCommandPoolCreateInfo command_pool_info = (VkCommandPoolCreateInfo){
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queue_family_indices.graphics_family,
    };

    VkCommandPool command_pool;
    if (vkCreateCommandPool(desc->device, &command_pool_info, NULL,
                            &command_pool) != VK_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to create Vulkan command pool.");
        return VNL_ERROR_COMMAND_POOL_CREATION_FAILED;
    }

    *out_command_pool = command_pool;

    return VNL_SUCCESS;
}