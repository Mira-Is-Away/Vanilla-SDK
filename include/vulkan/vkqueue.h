/**
 * @file vkqueue.h
 *
 * Queue families and queue management functions for Vulkan.
 *
 * @author Henry R
 * @date 2026-07-29
 */

#ifndef VANILLA_VULKAN_VKQUEUE_H_
#define VANILLA_VULKAN_VKQUEUE_H_

#include "core/vnl_types.h"
#include <stdbool.h>
#include <vulkan/vulkan.h>

typedef struct VkQueueFamilyIndices {
    bool has_graphics_family;
    bool has_present_family;
    u32 graphics_family;
    u32 present_family;
} VkQueueFamilyIndices;

/**
 * @brief Finds the queue family indices for graphics and present operations.
 * @param device The physical device to query.
 * @param surface The surface to check presentation support against.
 * @return VkQueueFamilyIndices structure with family indices and availability
 * flags.
 */
VkQueueFamilyIndices vk_find_queue_families(VkPhysicalDevice device,
                                            VkSurfaceKHR surface);

#endif
