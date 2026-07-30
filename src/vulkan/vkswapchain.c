#include <vulkan/vkswapchain.h>

#include <GLFW/glfw3.h>
#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <mira/clarity.h>

VkSwapChainInfo vk_swapchain_query_support(VkPhysicalDevice device,
                                           VkSurfaceKHR surface) {
    VkSwapChainInfo info = {0};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &info.cap);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);

    if (format_count > 0) {
        VkSurfaceFormatKHR *formats_arr =
            CLARITY_MALLOC(format_count * sizeof(VkSurfaceFormatKHR));
        if (formats_arr) {
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                                 formats_arr);

            for (u32 i = 0; i < format_count; i++)
                DARRAY_PUSH(info.formats, formats_arr[i]);

            CLARITY_FREE(formats_arr);
        }
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                              &present_mode_count, NULL);

    if (present_mode_count > 0) {
        VkPresentModeKHR *present_mode_arr =
            CLARITY_MALLOC(present_mode_count * sizeof(VkPresentModeKHR));
        if (present_mode_arr) {
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device, surface, &present_mode_count, present_mode_arr);

            for (u32 i = 0; i < present_mode_count; i++)
                DARRAY_PUSH(info.present_modes, present_mode_arr[i]);

            CLARITY_FREE(present_mode_arr);
        }
    }

    return info;
}

VkSurfaceFormatKHR vk_swapchain_choose_format(DARRAY(VkSurfaceFormatKHR)
                                                  available_formats) {
    DARRAY_FOREACH(VkSurfaceFormatKHR, format, available_formats) {
        if (format->format == VK_FORMAT_B8G8R8A8_SRGB &&
            format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return *format;
    }

    return available_formats[0];
}

VkPresentModeKHR vk_swapchain_choose_present_mode(/*DARRAY(VkPresentModeKHR)
                                                      available_present_modes*/) {
    /**
     * - VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application are
     * transferred to the screen right away, which may result in tearing.
     *
     * - VK_PRESENT_MODE_FIFO_KHR: The swap chain is a queue where the display
     * takes an image from the front of the queue when the display is refreshed
     * and the program inserts rendered images at the back of the queue. If the
     * queue is full then the program has to wait. This is most similar to
     * vertical sync as found in modern games. The moment that the display is
     * refreshed is known as "vertical blank".
     *
     * - VK_PRESENT_MODE_FIFO_RELAXED_KHR: This mode only differs from the
     * previous one if the application is late and the queue was empty at the
     * last vertical blank. Instead of waiting for the next vertical blank, the
     * image is transferred right away when it finally arrives. This may result
     * in visible tearing.
     *
     * - VK_PRESENT_MODE_MAILBOX_KHR: This is another variation of the second
     * mode. Instead of blocking the application when the queue is full, the
     * images that are already queued are simply replaced with the newer ones.
     * This mode can be used to render frames as fast as possible while still
     * avoiding tearing, resulting in fewer latency issues than standard
     * vertical sync. This is commonly known as "triple buffering", although the
     * existence of three buffers alone does not necessarily mean that the
     * framerate is unlocked.
     *
     * VK_PRESENT_MODE_FIFO_KHR is the only present mode guaranteed to be
     * available.
     *
     * VK_PRESENT_MODE_MAILBOX_KHR allows for great performance without screen
     * tearing, at the cost of higher power usage. In the future, there could be
     * a setting that allows Vanilla to pick between performance or energy
     * efficiency. Since the applications that are planned to use the SDK at
     * this stage are pretty lightweight, I'll be sticking to
     * VK_PRESENT_MODE_FIFO_KHR.
     *
     * - Henry
     *
     */

    /*
    However, in case the need for some extra performance is there, I'm defining
    the loop here now because I know I won't remember how to do it later.

    DARRAY_FOREACH(VkPresentModeKHR, present_mode, available_present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }
    */

    return VK_PRESENT_MODE_FIFO_KHR;
}

static inline u32 _u32_clamp(u32 val, u32 min, u32 max) {
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}

VkExtent2D vk_swapchain_extent(GLFWwindow *window,
                               VkSurfaceCapabilitiesKHR *cap) {
    if (cap->currentExtent.width != UINT32_MAX)
        return cap->currentExtent;

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    return (VkExtent2D){.width = _u32_clamp((u32)w, cap->minImageExtent.width,
                                            cap->maxImageExtent.width),
                        .height = _u32_clamp((u32)h, cap->minImageExtent.height,
                                             cap->maxImageExtent.height)};
}

VnlStatus vk_swapchain_create(VkPhysicalDevice physical_device, VkDevice device,
                              VkSurfaceKHR surface, GLFWwindow *window,
                              VkSwapchainKHR *out_sc) {

    CLARITY_LOG_INFO("Creating Vulkan Swapchain.");

    // Fetching information about the swapchain capabilities
    VkSwapChainInfo sc_info =
        vk_swapchain_query_support(physical_device, surface);
    VkSurfaceFormatKHR surface_format =
        vk_swapchain_choose_format(sc_info.formats);

    VkPresentModeKHR present_mode =
        vk_swapchain_choose_present_mode(/*sc_info.present_modes*/);

    VkExtent2D extent = vk_swapchain_extent(window, &sc_info.cap);

    u32 image_count = sc_info.cap.minImageCount + 1;

    if (sc_info.cap.maxImageCount > 0 &&
        image_count > sc_info.cap.maxImageCount)
        image_count = sc_info.cap.maxImageCount;

    // Configuring Swapchain creation
    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = sc_info.cap.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE};

    VkQueueFamilyIndices indices =
        vk_find_queue_families(physical_device, surface);

    u32 queue_family_indices[] = {indices.graphics_family,
                                  indices.present_family};

    if (indices.graphics_family != indices.present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = NULL;
    }

    // Swapchain creation
    VkSwapchainKHR sc;
    if (vkCreateSwapchainKHR(device, &create_info, NULL, &sc) != VK_SUCCESS) {
        CLARITY_LOG_ERROR("Vulkan Swapchain creation has failed.");
        return VNL_ERROR_SWAPCHAIN_CREATION_FAILED;
    }

    *out_sc = sc;
    return VNL_SUCCESS;
}