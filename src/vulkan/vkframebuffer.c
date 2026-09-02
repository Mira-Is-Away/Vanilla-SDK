#include <vulkan/vkframebuffer.h>

#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif

#include <core/vnl_status.h>
#include <mira/clarity.h>
#include <mira/darray.h>

VnlStatus vk_framebuffers_create(const VkFramebufferDesc *desc,
                                 DARRAY(VkFramebuffer) *out_framebuffers) {
    DARRAY_FOREACH(VkImageView, image_view, desc->image_views) {
        VkFramebufferCreateInfo framebuffer_info = (VkFramebufferCreateInfo){
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = desc->render_pass,
            .attachmentCount = 1,
            .pAttachments = image_view,
            .width = desc->extent.width,
            .height = desc->extent.height,
            .layers = 1};

        VkFramebuffer framebuffer;
        if (vkCreateFramebuffer(desc->device, &framebuffer_info, NULL,
                                &framebuffer) != VK_SUCCESS) {
            CLARITY_LOG_ERROR("Failed to create Vulkan Framebuffer.");
            return VNL_ERROR_FRAMEBUFFER_CREATION_FAILED;
        }

        DARRAY_PUSH(*out_framebuffers, framebuffer);
    }

    return VNL_SUCCESS;
}