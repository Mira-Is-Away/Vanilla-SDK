/**
 * @file vkcommandpool.h
 *
 * Creation and management of Vulkan command pools.
 *
 * @author Henry R
 * @date 2026-09-14
 */

#ifndef VNL_VULKAN_VKCOMMANDPOOL_H_
#define VNL_VULKAN_VKCOMMANDPOOL_H_

#include <core/vnl_status.h>
#include <vulkan/vulkan.h>

/**
 * @struct VkCommandPoolDesc
 * @brief Descriptor for creating a Vulkan command pool.
 */
typedef struct {
    VkDevice         device;
    VkPhysicalDevice physical_device;
    VkSurfaceKHR     surface;
} VkCommandPoolDesc;

/**
 * @brief Creates a Vulkan command pool configured for graphics command buffers.
 * @param[in] desc Pointer to the command pool descriptor.
 * @param[out] out_command_pool Pointer to store the created command pool handle.
 * @retval VNL_SUCCESS If command pool creation was successful.
 * @retval VNL_ERROR_COMMAND_POOL_CREATION_FAILED If command pool creation failed.
 */
VnlStatus vk_command_pool_create(const VkCommandPoolDesc *desc,
                                 VkCommandPool           *out_command_pool);

#endif