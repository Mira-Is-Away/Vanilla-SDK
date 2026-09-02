#include <vulkan/vkrenderpass.h>

#include <core/vnl_status.h>
#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>

VnlStatus vk_render_pass_create(const VkRenderPassDesc *desc,
                                VkRenderPass           *out_rp) {
    CLARITY_ASSERT(desc != NULL, "Render pass descriptor cannot be NULL.");
    CLARITY_ASSERT(desc->device != VK_NULL_HANDLE,
                   "Logical device cannot be NULL.");
    CLARITY_ASSERT(out_rp != NULL,
                   "Output render pass pointer cannot be NULL.");

    VkAttachmentDescription colour_attachment = (VkAttachmentDescription){
        .format         = desc->format,
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};

    /**
     * This functionality has been superseded by Vulkan 1.2
     * (VkAttachmentReference2) and Vulkan 1.4.
     */
    VkAttachmentReference attach_ref = (VkAttachmentReference){
        .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpass = (VkSubpassDescription){
        .flags                   = 0,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount    = 0,
        .pInputAttachments       = NULL,
        .colorAttachmentCount    = 1,
        .pColorAttachments       = &attach_ref,
        .pResolveAttachments     = NULL,
        .pDepthStencilAttachment = NULL,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments    = NULL};

    VkRenderPassCreateInfo render_pass_info = (VkRenderPassCreateInfo){
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments    = &colour_attachment,
        .subpassCount    = 1,
        .pSubpasses      = &subpass};

    /**
     * This functionality has been superseded by Vulkan 1.4
     */
    VkRenderPass render_pass;
    if (vkCreateRenderPass(desc->device, &render_pass_info, NULL,
                           &render_pass) != VK_SUCCESS) {
        CLARITY_LOG_ERROR("Failure to create Vulkan render pass.");
        return VNL_ERROR_RENDER_PASS_CREATION_FAILED;
    }

    *out_rp = render_pass;

    return VNL_SUCCESS;
}