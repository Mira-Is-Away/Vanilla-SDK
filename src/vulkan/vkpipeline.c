#include <vulkan/vkpipeline.h>

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <mira/clarity.h>
#include <mira/darray.h>
#include <vulkan/vkshadermodules.h>
/*
static VkPipelineDynamicStateCreateInfo _pipeline_dynamic_state_create() {

    DARRAY(VkDynamicState) dyn_states = NULL;
    DARRAY_PUSH(dyn_states, VK_DYNAMIC_STATE_VIEWPORT);
    DARRAY_PUSH(dyn_states, VK_DYNAMIC_STATE_SCISSOR);

    VkPipelineDynamicStateCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .dynamicStateCount = (u32)DARRAY_SIZE(dyn_states),
        .pDynamicStates = dyn_states};

    return create_info;
}
*/

VnlStatus vk_pipeline_create(VkDevice device) {
    size_t vert_s, frag_s;
    const char *vert = vk_shader_read("vertex.vert.spv", &vert_s);
    const char *frag = vk_shader_read("fragment.frag.spv", &frag_s);

    if (!vert || !frag) {
        CLARITY_LOG_ERROR("Failed to fetch shader bytecode.");
        return VNL_ERROR_SHADER_CREATION_FAILED;
    }

    VkShaderModule vert_m, frag_m;
    if (vk_shader_module_create(device, vert, vert_s, &vert_m) != VNL_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to create vertex shader module.");
        return VNL_ERROR_SHADER_CREATION_FAILED;
    }

    if (vk_shader_module_create(device, frag, frag_s, &frag_m) != VNL_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to create fragment shader module.");
        return VNL_ERROR_SHADER_CREATION_FAILED;
    }

    VkPipelineShaderStageCreateInfo v_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vert_m,
        .pName = "main",
        .pSpecializationInfo = NULL};

    VkPipelineShaderStageCreateInfo f_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = vert_m,
        .pName = "main",
        .pSpecializationInfo = NULL};

    DARRAY(VkPipelineShaderStageCreateInfo) create_infos = NULL;
    DARRAY_PUSH(create_infos, v_create_info);
    DARRAY_PUSH(create_infos, f_create_info);

    /*

    VkPipelineDynamicStateCreateInfo dyn_info =
        _pipeline_dynamic_state_create();

    VkPipelineVertexInputStateCreateInfo vis_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = NULL,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = NULL};

    VkPipelineInputAssemblyStateCreateInfo in_asm_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE};

    vkDestroyShaderModule(device, vert_m, NULL);
    vkDestroyShaderModule(device, frag_m, NULL);

    */
    return VNL_SUCCESS;
}