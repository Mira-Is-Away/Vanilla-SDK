#ifndef VNL_VULKAN_VKFRAMEBUFFER_H_
#define VNL_VULKAN_VKFRAMEBUFFER_H_

#include <core/vnl_status.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

typedef struct {
    VkDevice device;
    DARRAY(VkImageView) image_views;
    VkRenderPass render_pass;
    VkExtent2D extent;
} VkFramebufferDesc;

VnlStatus vk_framebuffers_create(const VkFramebufferDesc *desc,
                                 DARRAY(VkFramebuffer) * out_framebuffers);

#endif