/**
 * @file vkimageview.h
 *
 * Creation and management of Vulkan image views.
 *
 * @author Henry R
 * @date 2026-09-14
 */

#ifndef VNL_VULKAN_VKIMAGEVIEW_H_
#define VNL_VULKAN_VKIMAGEVIEW_H_

#include <core/vnl_status.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

/**
 * @struct VkImageViewDesc
 * @brief Descriptor containing parameters for creating image views for a set of
 * images.
 */
typedef struct {
    VkDevice              device;
    const DARRAY(VkImage) images;
    VkFormat              format;
} VkImageViewDesc;

/**
 * @brief Creates Vulkan 2D image views for each image in the descriptor.
 * @param[in] desc Pointer to the image view descriptor.
 * @param[out] out_views Pointer to a dynamic array to store the created image
 * views.
 * @retval VNL_SUCCESS If image views were created successfully.
 * @retval VNL_ERROR_IMAGE_VIEW_CREATION_FAILED If image view creation failed.
 */
VnlStatus vk_image_view_create(const VkImageViewDesc *desc,
                               DARRAY(VkImageView)   *out_views);

#endif