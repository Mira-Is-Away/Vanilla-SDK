#include <vulkan/vkimageview.h>

#include <core/vnl_status.h>
#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

VnlStatus vk_image_view_create(const VkImageViewDesc *desc,
                               DARRAY(VkImageView)   *out_views) {
    CLARITY_ASSERT(desc != NULL, "ImageView descriptor cannot be NULL.");
    CLARITY_ASSERT(desc->device != VK_NULL_HANDLE,
                   "Logical device cannot be NULL.");
    CLARITY_ASSERT(desc->images != NULL, "Images array cannot be NULL.");
    CLARITY_ASSERT(out_views != NULL, "Output views pointer cannot be NULL.");

    for (size_t i = 0; i < DARRAY_SIZE(desc->images); i++) {
        VkImageViewCreateInfo create_info = {
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext    = NULL,
            .flags    = 0,
            .image    = desc->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = desc->format,
            .components =
                (VkComponentMapping){.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                     .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                     .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                     .a = VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = (VkImageSubresourceRange){
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1}};

        VkImageView view;
        if (vkCreateImageView(desc->device, &create_info, NULL, &view) !=
            VK_SUCCESS) {
            CLARITY_LOG_ERROR("Failed to create VkImageView (#%d).", i);
            return VNL_ERROR_IMAGE_VIEW_CREATION_FAILED;
        }

        DARRAY_PUSH(*out_views, view);
    }

    return VNL_SUCCESS;
}

// NEED TO DESTROY THESE OURSELVES, they were allocated by us, unlike the
// VkImages.