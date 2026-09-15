/**
 * @file vkpipeline.h
 *
 * Creation and configuration of Vulkan graphics pipelines.
 *
 * @author Henry R
 * @date 2026-09-14
 */

#ifndef VNL_VULKAN_VKPIPELINE_H_
#define VNL_VULKAN_VKPIPELINE_H_

#include <core/vnl_status.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vulkan.h>

/**
 * @struct VkPipelineInstance
 * @brief Holds a Vulkan graphics pipeline and its layout.
 */
typedef struct VkPipelineInstance {
    VkPipeline       pipeline;
    VkPipelineLayout layout;
} VkPipelineInstance;

/**
 * @struct VkPipelineDesc
 * @brief Descriptor containing parameters for graphics pipeline creation.
 */
typedef struct {
    VkDevice     device;
    VkRenderPass render_pass;
    VkExtent2D   extent;
} VkPipelineDesc;

/**
 * @brief Creates a Vulkan graphics pipeline and pipeline layout.
 * @param[in] desc Pointer to the pipeline descriptor.
 * @param[out] out_pipeline Pointer to store the created pipeline instance.
 * @retval VNL_SUCCESS If pipeline creation was successful.
 * @retval VNL_ERROR_SHADER_CREATION_FAILED If loading or creating shader modules failed.
 * @retval VNL_ERROR_PIPELINE_CREATION_FAILED If pipeline or pipeline layout creation failed.
 */
VnlStatus vk_pipeline_create(const VkPipelineDesc *desc,
                             VkPipelineInstance   *out_pipeline);

// void vk_pipeline_destroy();

#endif