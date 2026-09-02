#ifndef VNL_VULKAN_VKSWAPCHAIN_H_
#define VNL_VULKAN_VKSWAPCHAIN_H_

#include <GLFW/glfw3.h>
#include <core/vnl_status.h>
#include <mira/darray.h>
#include <vulkan/vkqueue.h>
#include <vulkan/vulkan.h>

typedef struct {
    VkSurfaceCapabilitiesKHR   cap;
    DARRAY(VkSurfaceFormatKHR) formats;
    DARRAY(VkPresentModeKHR)   present_modes;
} VkSwapchainInfo;

typedef struct {
    VkSwapchainKHR  swapchain;
    VkFormat        format;
    VkExtent2D      extent;
    DARRAY(VkImage) images;
} VkSwapchainInstance;

typedef struct {
    VkPhysicalDevice physical_device;
    VkDevice         device;
    VkSurfaceKHR     surface;
    GLFWwindow      *window;
} VkSwapchainDesc;

VkSwapchainInfo vk_swapchain_query_support(VkPhysicalDevice device,
                                           VkSurfaceKHR     surface);

VkSurfaceFormatKHR
vk_swapchain_choose_format(DARRAY(VkSurfaceFormatKHR) available_formats);

VkPresentModeKHR vk_swapchain_choose_present_mode(
    /*DARRAY(VkPresentModeKHR)
        available_present_modes*/
);

VkExtent2D vk_swapchain_extent(GLFWwindow               *window,
                               VkSurfaceCapabilitiesKHR *cap);

VnlStatus vk_swapchain_create(const VkSwapchainDesc *desc,
                              VkSwapchainInstance   *out_sc);

#endif