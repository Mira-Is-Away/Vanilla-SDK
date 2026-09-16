#include "winman/winman.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <core/vnl_types.h>

#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>

typedef struct VnlWinMan {
    int         width;
    int         height;
    GLFWwindow *window;
} VnlWinMan;

VnlStatus vnl_winman_init(const VnlConfig *config, VnlWinMan **out_winman) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");
    CLARITY_ASSERT(out_winman != NULL,
                   "Output window manager pointer cannot be NULL.");

    CLARITY_LOG_INFO("Initialising window manager: %s (%dx%d)", config->title,
                     config->window.width, config->window.height);

    if (glfwInit() == false) {
        return VNL_ERROR_GLFW_INIT_FAILED;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *new_window = glfwCreateWindow(
        config->window.width, config->window.height, config->title, NULL, NULL);

    if (!new_window) {
        CLARITY_LOG_ERROR("Failed to create GLFW window.");
        return VNL_ERROR_WINDOW_CREATION_FAILED;
    }

    VnlWinMan *winman = (VnlWinMan *)CLARITY_MALLOC(sizeof(VnlWinMan));

    if (!winman) {
        CLARITY_LOG_ERROR("Vanilla has run out of available memory!");
        glfwDestroyWindow(new_window);
        return VNL_ERROR_OUT_OF_MEMORY;
    }

    winman->width  = config->window.width;
    winman->height = config->window.height;
    winman->window = new_window;

    *out_winman = winman;

    return VNL_SUCCESS;
}

GLFWwindow *vnl_winman_get_window(VnlWinMan *winman) {
    CLARITY_ASSERT(winman != NULL, "winman cannot be NULL.");
    return winman ? winman->window : NULL;
}

bool vnl_winman_should_close(VnlWinMan *winman) {
    CLARITY_ASSERT(winman != NULL, "winman cannot be NULL.");
    return winman && winman->window ? glfwWindowShouldClose(winman->window)
                                    : true;
}

void vnl_winman_poll_events(VnlWinMan *winman) {
    (void)winman;
    glfwPollEvents();
}

const char **vnl_winman_get_required_extensions(u32 *out_count) {
    CLARITY_ASSERT(out_count != NULL, "out_count cannot be NULL.");
    return glfwGetRequiredInstanceExtensions(out_count);
}

VnlStatus
vnl_winman_create_vulkan_surface(VnlWinMan *winman, VkInstance instance,
                                 const VkAllocationCallbacks *allocator,
                                 VkSurfaceKHR                *out_surface) {
    CLARITY_ASSERT(winman != NULL, "winman cannot be NULL.");
    CLARITY_ASSERT(winman->window != NULL, "winman->window cannot be NULL.");
    CLARITY_ASSERT(instance != VK_NULL_HANDLE, "VkInstance cannot be NULL.");
    CLARITY_ASSERT(out_surface != NULL, "out_surface cannot be NULL.");

    VkResult result = glfwCreateWindowSurface(instance, winman->window,
                                              allocator, out_surface);
    if (result != VK_SUCCESS) {
        CLARITY_LOG_WARN("Failed to create Vulkan window surface.");
        return VNL_ERROR_SURFACE_CREATION_FAILED;
    }

    return VNL_SUCCESS;
}

void vnl_winman_get_framebuffer_size(VnlWinMan *winman, int *out_width,
                                     int *out_height) {
    CLARITY_ASSERT(winman != NULL, "winman cannot be NULL.");
    CLARITY_ASSERT(winman->window != NULL, "winman->window cannot be NULL.");
    glfwGetFramebufferSize(winman->window, out_width, out_height);
}

void vnl_winman_shutdown(VnlWinMan *winman) {
    CLARITY_ASSERT(winman != NULL, "Attempted to free a null pointer.");

    if (winman) {
        if (winman->window) {
            glfwDestroyWindow(winman->window);
            winman->window = NULL;
        }
        CLARITY_FREE(winman);
    }

    glfwTerminate();
}