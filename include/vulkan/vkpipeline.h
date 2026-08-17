#ifndef VNL_VULKAN_VKPIPELINE_H_
#define VNL_VULKAN_VKPIPELINE_H_

#include <core/vnl_status.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vulkan.h>

VnlStatus vk_pipeline_create(VkDevice device, VkSwapchainInstance sc,
                             VkPipelineLayout *out_layout);

// void vk_pipeline_destroy();

#endif