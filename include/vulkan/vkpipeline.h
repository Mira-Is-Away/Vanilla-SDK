#ifndef VNL_VULKAN_VKPIPELINE_H_
#define VNL_VULKAN_VKPIPELINE_H_

#include <core/vnl_status.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vulkan.h>

typedef struct VkPipelineInstance {
    VkPipeline pipeline;
    VkPipelineLayout layout;
} VkPipelineInstance;

VnlStatus vk_pipeline_create(VkDevice device, VkSwapchainInstance sc,
                             VkRenderPass render_pass,
                             VkPipelineInstance *out_pipeline);

// void vk_pipeline_destroy();

#endif