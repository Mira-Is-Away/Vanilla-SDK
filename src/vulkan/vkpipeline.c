#include <vulkan/vkpipeline.h>

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>
#include <mira/darray.h>
#include <vulkan/vkshadermodules.h>
#include <vulkan/vkswapchain.h>

static VkPipelineDynamicStateCreateInfo _create_dynamic_pipeline_state() {
    DARRAY(VkDynamicState) dynamic_state_list = NULL;
    DARRAY_PUSH(dynamic_state_list, VK_DYNAMIC_STATE_VIEWPORT);
    DARRAY_PUSH(dynamic_state_list, VK_DYNAMIC_STATE_SCISSOR);

    return (VkPipelineDynamicStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = DARRAY_SIZE(dynamic_state_list),
        .pDynamicStates = dynamic_state_list};
}

VnlStatus vk_pipeline_create(VkDevice device, VkSwapchainInstance sc,
                             VkRenderPass render_pass,
                             VkPipelineInstance *out_pipeline) {
    CLARITY_ASSERT(device != VK_NULL_HANDLE, "Logical device cannot be NULL.");
    CLARITY_ASSERT(out_pipeline != NULL,
                   "Output pipeline layout pointer cannot be NULL.");

    size_t vert_s, frag_s;
    const char *vert = vk_shader_read("vertex.vert.spv", &vert_s);
    const char *frag = vk_shader_read("fragment.frag.spv", &frag_s);
    sc = sc; // This is here temporarily to avoid "unused variable" compiler
             // warnings.

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
        .module = frag_m,
        .pName = "main",
        .pSpecializationInfo = NULL};

    DARRAY(VkPipelineShaderStageCreateInfo) shader_stages = NULL;
    DARRAY_PUSH(shader_stages, v_create_info);
    DARRAY_PUSH(shader_stages, f_create_info);

    /**
     * The entire next session is full of currently unused structs, but that
     * will be used later. The next section is commented out to avoid "unused
     * variable" warnings.
     */
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
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

    VkPipelineViewportStateCreateInfo viewport_state =
        (VkPipelineViewportStateCreateInfo){
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = &(VkViewport){.x = 0.0f,
                                        .y = 0.0f,
                                        .width = (float)sc.extent.width,
                                        .height = (float)sc.extent.height,
                                        .minDepth = 0.0f,
                                        .maxDepth = 1.0f},
            .scissorCount = 1,
            .pScissors = &(VkRect2D){.offset = {0, 0}, .extent = sc.extent}};

    VkPipelineRasterizationStateCreateInfo rast_info =
        (VkPipelineRasterizationStateCreateInfo){
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .lineWidth = 1.0f,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f};

    VkPipelineMultisampleStateCreateInfo multisample_info =
        (VkPipelineMultisampleStateCreateInfo){
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .sampleShadingEnable = VK_FALSE,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .minSampleShading = 1.0f,
            .pSampleMask = NULL,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE};

    /**
     * Alpha blending is currently disabled, but I'll leave the optional fields
     * of the colour blending struct commented out for easier refactor later.
     */
    VkPipelineColorBlendAttachmentState color_blend =
        (VkPipelineColorBlendAttachmentState){
            .colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            .blendEnable = VK_FALSE
            //.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            //.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            //.colorBlendOp = VK_BLEND_OP_ADD,
            //.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            //.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            //.alphaBlendOp = VK_BLEND_OP_ADD
        };

    VkPipelineColorBlendStateCreateInfo colour_blend_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &color_blend,
        .blendConstants[0] = 0.0f,
        .blendConstants[1] = 0.0f,
        .blendConstants[2] = 0.0f,
        .blendConstants[3] = 0.0f};

    VkPipelineDynamicStateCreateInfo dynamic_state =
        _create_dynamic_pipeline_state();

    VkPipelineLayout pipeline_layout;
    VkPipelineLayoutCreateInfo layout_info = (VkPipelineLayoutCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = NULL,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL};

    if (vkCreatePipelineLayout(device, &layout_info, NULL, &pipeline_layout) !=
        VK_SUCCESS) {
        return VNL_ERROR_PIPELINE_CREATION_FAILED;
    }

    out_pipeline->layout = pipeline_layout;

    VkGraphicsPipelineCreateInfo pipeline_info = (VkGraphicsPipelineCreateInfo){
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &in_asm_info,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rast_info,
        .pMultisampleState = &multisample_info,
        .pDepthStencilState = NULL,
        .pColorBlendState = &colour_blend_info,
        .pDynamicState = &dynamic_state,
        .layout = pipeline_layout,
        .renderPass = render_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1};

    VkPipeline pipeline;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info,
                                  NULL, &pipeline) != VK_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to create graphics pipeline.");
        return VNL_ERROR_PIPELINE_CREATION_FAILED;
    }

    out_pipeline->pipeline = pipeline;

    // Won't need the shader modules after they've been uploaded to the GPU
    vkDestroyShaderModule(device, vert_m, NULL);
    vkDestroyShaderModule(device, frag_m, NULL);

    return VNL_SUCCESS;
}