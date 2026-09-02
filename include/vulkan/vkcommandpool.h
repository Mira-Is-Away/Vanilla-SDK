#ifndef VNL_VULKAN_VKCOMMANDPOOL_H_
#define VNL_VULKAN_VKCOMMANDPOOL_H_

#include <core/vnl_status.h>
#include <vulkan/vulkan.h>

typedef struct {
    VkDevice         device;
    VkPhysicalDevice physical_device;
    VkSurfaceKHR     surface;
} VkCommandPoolDesc;

VnlStatus vk_command_pool_create(const VkCommandPoolDesc *desc,
                                 VkCommandPool           *out_command_pool);

#endif