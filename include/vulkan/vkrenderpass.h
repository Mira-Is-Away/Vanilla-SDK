#ifndef VNL_VULKAN_VKRENDERPASS_H_
#define VNL_VULKAN_VKRENDERPASS_H_

#include <core/vnl_status.h>
#include <vulkan/vulkan.h>

typedef struct {
    VkDevice device;
    VkFormat format;
} VkRenderPassDesc;

VnlStatus vk_render_pass_create(const VkRenderPassDesc *desc,
                                VkRenderPass           *out_rp);

#endif