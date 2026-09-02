#ifndef VNL_VULKAN_VKPIPELINE_H_
#define VNL_VULKAN_VKPIPELINE_H_

#include <core/vnl_status.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vulkan.h>

typedef struct VkPipelineInstance {
    VkPipeline       pipeline;
    VkPipelineLayout layout;
} VkPipelineInstance;

typedef struct {
    VkDevice     device;
    VkRenderPass render_pass;
    VkExtent2D   extent;
} VkPipelineDesc;

VnlStatus vk_pipeline_create(const VkPipelineDesc *desc,
                             VkPipelineInstance   *out_pipeline);

// void vk_pipeline_destroy();

#endif