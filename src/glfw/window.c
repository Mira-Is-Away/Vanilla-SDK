#include "glfw/window.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>
#include <stdbool.h>

#include "misc/vnl_types.h"

VnlStatus vnl_window_create(const VnlConfig *config, GLFWwindow **out_window) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");
    CLARITY_ASSERT(out_window != NULL, "Output window pointer cannot be NULL.");

    CLARITY_LOG_INFO("Creating GLFW window: %s (%dx%d)", config->title,
                     config->window.width, config->window.height);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(
        config->window.width, config->window.height, config->title, NULL, NULL);

    CLARITY_ASSERT(window != NULL, "Failed to create GLFW window.");

    if (!window) {
        CLARITY_LOG_WARN("Failed to create GLFW window.");
        return VNL_ERROR_WINDOW_CREATION_FAILED;
    }

    *out_window = window;

    CLARITY_LOG_INFO("GLFW window created successfully.");
    return VNL_SUCCESS;
}

void vnl_window_destroy(GLFWwindow *window) {
    if (window) {
        CLARITY_LOG_INFO("Destroying GLFW window.");
        glfwDestroyWindow(window);
    }
}