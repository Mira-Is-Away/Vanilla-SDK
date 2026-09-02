#ifndef VNL_VULKAN_VKRENDERPASS_H_
#define VNL_VULKAN_VKRENDERPASS_H_

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

typedef struct {
    VkDevice device;
    VkFormat format;
} VkRenderPassDesc;

VnlStatus vk_render_pass_create(const VkRenderPassDesc *desc,
                                VkRenderPass           *out_rp);

typedef struct {
    VkCommandBuffer       command_buffer;
    VkRenderPass          render_pass;
    DARRAY(VkFramebuffer) framebuffers;
    u32                   image_index;
    VnlVec2               offset;
    VkExtent2D            extent;
    VnlColour             clear_colour;
} VkRenderPassBeginDesc;

void vk_render_pass_begin(const VkRenderPassBeginDesc *desc);

#endif