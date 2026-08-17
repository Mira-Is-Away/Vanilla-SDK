#ifndef VNL_VULKAN_VKSHADERMODULES_H_
#define VNL_VULKAN_VKSHADERMODULES_H_

#include <core/vnl_status.h>
#include <stddef.h>
#include <vulkan/vulkan.h>

VnlStatus vk_shader_module_create(VkDevice device, const char *shader_src,
                                  size_t src_size, VkShaderModule *out_module);

char *vk_shader_read(const char *filename, size_t *out_size);

#endif