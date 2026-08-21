#ifndef VNL_VULKAN_VKRENDERPASS_H_
#define VNL_VULKAN_VKRENDERPASS_H_

#include <core/vnl_status.h>
#include <vulkan/vkswapchain.h>

VnlStatus vk_render_pass_create(VkDevice device, VkSwapchainInstance sc,
                                VkRenderPass *out_rp);

#endif