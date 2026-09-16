/**
 * @file vkframebuffer.h
 *
 * Creation and management of Vulkan framebuffers.
 *
 * @author Henry R
 * @date 2026-09-14
 */

#ifndef VNL_VULKAN_VKFRAMEBUFFER_H_
#define VNL_VULKAN_VKFRAMEBUFFER_H_

#include <core/vnl_status.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

/**
 * @struct VkFramebufferDesc
 * @brief Descriptor containing parameters for creating framebuffers.
 */
typedef struct {
    VkDevice            device;
    DARRAY(VkImageView) image_views;
    VkRenderPass        render_pass;
    VkExtent2D          extent;
} VkFramebufferDesc;

/**
 * @brief Creates framebuffers for each image view in the descriptor.
 * @param[in] desc Pointer to the framebuffer descriptor.
 * @param[out] out_framebuffers Pointer to a dynamic array to store the created
 * framebuffers.
 * @retval VNL_SUCCESS If framebuffers were created successfully.
 * @retval VNL_ERROR_FRAMEBUFFER_CREATION_FAILED If framebuffer creation failed.
 */
VnlStatus vk_framebuffers_create(const VkFramebufferDesc *desc,
                                 DARRAY(VkFramebuffer)   *out_framebuffers);

#endif