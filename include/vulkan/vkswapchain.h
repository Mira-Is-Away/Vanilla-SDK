/**
 * @file vkswapchain.h
 *
 * Creation and configuration of Vulkan swapchains and surface capabilities query.
 *
 * @author Henry R
 * @date 2026-09-14
 */

#ifndef VNL_VULKAN_VKSWAPCHAIN_H_
#define VNL_VULKAN_VKSWAPCHAIN_H_

#include <GLFW/glfw3.h>
#include <core/vnl_status.h>
#include <mira/darray.h>
#include <vulkan/vkqueue.h>
#include <vulkan/vulkan.h>

/**
 * @struct VkSwapchainInfo
 * @brief Contains surface capabilities, supported formats, and present modes.
 */
typedef struct {
    VkSurfaceCapabilitiesKHR   cap;
    DARRAY(VkSurfaceFormatKHR) formats;
    DARRAY(VkPresentModeKHR)   present_modes;
} VkSwapchainInfo;

/**
 * @struct VkSwapchainInstance
 * @brief Represents an active Vulkan swapchain with its format, extent, and images.
 */
typedef struct {
    VkSwapchainKHR  swapchain;
    VkFormat        format;
    VkExtent2D      extent;
    DARRAY(VkImage) images;
} VkSwapchainInstance;

/**
 * @struct VkSwapchainDesc
 * @brief Descriptor containing parameters for swapchain creation.
 */
typedef struct {
    VkPhysicalDevice physical_device;
    VkDevice         device;
    VkSurfaceKHR     surface;
    GLFWwindow      *window;
} VkSwapchainDesc;

/**
 * @brief Queries swapchain support details for a physical device and surface.
 * @param[in] device The physical device to query.
 * @param[in] surface The window surface.
 * @return VkSwapchainInfo structure containing capabilities, formats, and present modes.
 */
VkSwapchainInfo vk_swapchain_query_support(VkPhysicalDevice device,
                                           VkSurfaceKHR     surface);

/**
 * @brief Selects an optimal surface format (preferring SRGB).
 * @param[in] available_formats Dynamic array of supported surface formats.
 * @return The chosen VkSurfaceFormatKHR.
 */
VkSurfaceFormatKHR
vk_swapchain_choose_format(DARRAY(VkSurfaceFormatKHR) available_formats);

/**
 * @brief Selects the presentation mode for the swapchain.
 * @return The chosen VkPresentModeKHR.
 */
VkPresentModeKHR vk_swapchain_choose_present_mode(
    /*DARRAY(VkPresentModeKHR)
        available_present_modes*/
);

/**
 * @brief Computes the swapchain 2D extent matching the window framebuffer dimensions.
 * @param[in] window The GLFW window handle.
 * @param[in] cap Pointer to the surface capabilities.
 * @return VkExtent2D with clamped dimensions.
 */
VkExtent2D vk_swapchain_extent(GLFWwindow               *window,
                               VkSurfaceCapabilitiesKHR *cap);

/**
 * @brief Creates a Vulkan swapchain and retrieves its images.
 * @param[in] desc Pointer to the swapchain creation descriptor.
 * @param[out] out_sc Pointer to store the created swapchain instance.
 * @retval VNL_SUCCESS If swapchain creation was successful.
 * @retval VNL_ERROR_SWAPCHAIN_CREATION_FAILED If creation failed.
 */
VnlStatus vk_swapchain_create(const VkSwapchainDesc *desc,
                              VkSwapchainInstance   *out_sc);

#endif