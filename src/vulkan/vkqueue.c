#include "vulkan/vkqueue.h"

#include <stdlib.h>
#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>

VkQueueFamilyIndices vk_find_queue_families(VkPhysicalDevice device,
                                            VkSurfaceKHR     surface) {
    VkQueueFamilyIndices indices = {.has_graphics_family = false,
                                    .has_present_family  = false,
                                    .graphics_family     = 0,
                                    .present_family      = 0};

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

    VkQueueFamilyProperties *queue_families =
        CLARITY_MALLOC(sizeof(VkQueueFamilyProperties) * queue_family_count);

    if (!queue_families) {
        return indices;
    }
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             queue_families);

    for (u32 i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family     = i;
            indices.has_graphics_family = true;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                             &present_support);
        if (present_support) {
            indices.present_family     = i;
            indices.has_present_family = true;
        }

        if (indices.has_graphics_family && indices.has_present_family)
            break;
    }

    CLARITY_FREE(queue_families);
    return indices;
}
