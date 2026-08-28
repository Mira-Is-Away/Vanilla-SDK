#ifndef VNL_VULKAN_VKIMAGEVIEW_H_
#define VNL_VULKAN_VKIMAGEVIEW_H_

#include <core/vnl_status.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

typedef struct {
    VkDevice device;
    const DARRAY(VkImage) images;
    VkFormat format;
} VkImageViewDesc;

VnlStatus vk_image_view_create(const VkImageViewDesc *desc,
                               DARRAY(VkImageView) * out_views);

#endif