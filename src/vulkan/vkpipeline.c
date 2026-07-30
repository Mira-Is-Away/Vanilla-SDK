#include <vulkan/vkpipeline.h>

#include <core/vnl_status.h>
#include <vulkan/vkshadermodules.h>

typedef struct {
    size_t vert;
    size_t frag;
} VkShaderSizes;

VnlStatus vk_pipeline_create() {
    VkShaderSizes sizes;
    const char *vert_s = vk_shader_read("vertex.vert.spv", &sizes.vert);
    const char *frag_s = vk_shader_read("fragment.frag.spv", &sizes.frag);

    if (!vert_s || !frag_s)
        return VNL_FAILURE;

    return VNL_SUCCESS;
}