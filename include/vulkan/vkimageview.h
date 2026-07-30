#ifndef VNL_VULKAN_VKIMAGEVIEW_H_
#define VNL_VULKAN_VKIMAGEVIEW_H_

#include <core/vnl_status.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

VnlStatus vk_image_view_create(VkDevice device, const DARRAY(VkImage) images,
                               VkFormat format, DARRAY(VkImageView) * views);

#endif