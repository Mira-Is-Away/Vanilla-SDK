/**
 * @file vkcontext.h
 *
 * The structure and functions dedicated to storing
 * Vulkan context information.
 *
 * @author Henry R
 * @date 2025-12-09
 */

#ifndef VANILLA_VULKAN_VKCONTEXT_H_
#define VANILLA_VULKAN_VKCONTEXT_H_

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <core/vnl_status.h>
#include <vulkan/vulkan.h>

typedef struct VkQueueFamilyIndices VkQueueFamilyIndices;
typedef struct VnlConfig            VnlConfig;
typedef struct VnlContext           VnlContext;

/**
 * @struct VkContext
 * @brief Holds pointers to Vulkan context information.
 */
typedef struct VkContext VkContext;

/**
 * @brief Initialises a Vulkan context instance.
 * @return Returns a pointer to valid VkContext.
 * @retval NULL If context initialisation fails.
 */
VnlStatus vulkan_init(const VnlConfig *config, GLFWwindow *window,
                      VkContext **out_ctx);
void      vulkan_shutdown(VkContext *vkctx);

#endif