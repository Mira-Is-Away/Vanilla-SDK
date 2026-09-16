/**
 * @file vkshadermodules.h
 *
 * Loading and creation of Vulkan shader modules from SPIR-V bytecode.
 *
 * @author Henry R
 * @date 2026-09-14
 */

#ifndef VNL_VULKAN_VKSHADERMODULES_H_
#define VNL_VULKAN_VKSHADERMODULES_H_

#include <mira/vnl_status.h>
#include <stddef.h>
#include <vulkan/vulkan.h>

/**
 * @brief Creates a Vulkan shader module from binary shader bytecode.
 * @param[in] device The logical device handle.
 * @param[in] shader_src Pointer to the shader binary bytecode.
 * @param[in] src_size Size of the bytecode in bytes.
 * @param[out] out_module Pointer to store the created VkShaderModule handle.
 * @retval VNL_SUCCESS If shader module creation was successful.
 * @retval VNL_ERROR_SHADER_CREATION_FAILED If creation failed.
 */
VnlStatus vk_shader_module_create(VkDevice device, const char *shader_src,
                                  size_t src_size, VkShaderModule *out_module);

/**
 * @brief Reads a shader file from the shader directory into a heap-allocated
 * buffer.
 * @param[in] filename Relative file path of the shader file.
 * @param[out] out_size Optional pointer to store the number of bytes read.
 * @return Pointer to heap-allocated buffer containing the file content, or NULL
 * on failure.
 */
char *vk_shader_read(const char *filename, size_t *out_size);

#endif