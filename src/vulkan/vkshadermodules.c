#include <vulkan/vkshadermodules.h>

#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <mira/clarity.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

#ifndef SHADER_DIR
#define SHADER_DIR "./shaders/" // Fallback for manual compilation
#endif

VnlStatus vk_shader_module_create(VkDevice device, const char *shader_src,
                                  size_t src_size, VkShaderModule *out_module) {
    CLARITY_ASSERT(device != VK_NULL_HANDLE, "Logical device cannot be NULL.");
    CLARITY_ASSERT(shader_src != NULL, "Shader source cannot be NULL.");
    CLARITY_ASSERT(out_module != NULL,
                   "Output shader module pointer cannot be NULL.");

    VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = src_size,
        .pCode = (const u32 *)shader_src};

    VkShaderModule shader_module;
    if (vkCreateShaderModule(device, &create_info, NULL, &shader_module) !=
        VK_SUCCESS) {
        CLARITY_LOG_ERROR("Failed to create shader module.");
        return VNL_ERROR_SHADER_CREATION_FAILED;
    }

    *out_module = shader_module;

    return VNL_SUCCESS;
}

char *vk_shader_read(const char *filename, size_t *out_size) {
    char fullpath[4096];

    snprintf(fullpath, sizeof(fullpath), "%s%s", SHADER_DIR, filename);

    FILE *f = fopen(fullpath, "rb");
    if (!f) {
        CLARITY_LOG_ERROR("Failed to open %s", fullpath);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long int fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize <= 0) {
        fclose(f);
        return NULL;
    }

    char *buffer = (char *)malloc(fsize);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, fsize, f);
    fclose(f);

    if (out_size) {
        *out_size = bytes_read;
    }

    return buffer;
}