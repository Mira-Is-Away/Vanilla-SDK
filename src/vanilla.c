#include <vanilla.h>

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#define MIRA_CLARITY_IMPL
#include <mira/clarity.h>

#include <glfw/window.h>
#include <misc/vnl_types.h>
#include <vulkan/vkcontext.h>

struct VnlEngine {
    const VnlConfig *config;
    GLFWwindow *window;
    VkContext *vkctx;
};

static VnlStatus vnl_init_glfw() {
    CLARITY_LOG_INFO("Initializing GLFW.");

    bool glfw_status = glfwInit();

    CLARITY_ASSERT(glfw_status == true, "Failed to init glfw.");

    if (!glfw_status) {
        return VNL_ERROR_GLFW_INIT_FAILED;
    }

    CLARITY_LOG_INFO("GLFW has initialised successfully.");
    return VNL_SUCCESS;
}

VnlStatus vnl_init(const VnlConfig *config, VnlEngine **out_engine) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");
    CLARITY_ASSERT(out_engine != NULL, "Output engine pointer cannot be NULL.");

    CLARITY_LOG_INFO("Initializing Vanilla SDK Components.");

    VnlStatus status;

    status = vnl_init_glfw();
    CLARITY_ASSERT(status == VNL_SUCCESS, "vnl_init_glfw failed.");
    if (status != VNL_SUCCESS)
        return status;

    VnlEngine *engine = CLARITY_MALLOC(sizeof(VnlEngine));
    CLARITY_ASSERT(engine != NULL, "Failed to allocate memory for VnlEngine.");
    if (!engine)
        return VNL_ERROR_OUT_OF_MEMORY;

    engine->config = config;

    status = vnl_window_create(config, &engine->window);
    CLARITY_ASSERT(status == VNL_SUCCESS, "vnl_window_create failed.");
    if (status != VNL_SUCCESS) {
        free(engine);
        return status;
    }

    status = vulkan_init(config, engine->window, &engine->vkctx);
    CLARITY_ASSERT(status == VNL_SUCCESS, "vulkan_init failed.");
    if (status != VNL_SUCCESS) {
        vnl_window_destroy(engine->window);
        free(engine);
        return status;
    }

    *out_engine = engine;
    CLARITY_LOG_INFO("Vanilla has initialised successfully.");
    return VNL_SUCCESS;
}

void vnl_run(VnlEngine *engine) {
    CLARITY_ASSERT(engine != NULL, "Engine pointer is NULL.");
    CLARITY_ASSERT(engine->window != NULL, "Engine window pointer is NULL.");
    if (!engine || !engine->window)
        return;

    /*
    while(!glfwWindowShouldClose(engine->window)) {
        glfwPollEvents();
    }*/

    CLARITY_LOG_WARN("vnl_run() has been called. This function is currently a "
                     "dummy; Shutting down...");
}

void vnl_shutdown(VnlEngine *engine) {
    CLARITY_ASSERT(engine != NULL, "Engine pointer is NULL.");
    if (!engine)
        return;

    CLARITY_LOG_INFO("Shutting down Vanilla Engine.");

    vulkan_shutdown(engine->vkctx);
    vnl_window_destroy(engine->window);
    glfwTerminate();
    CLARITY_FREE(engine);

    CLARITY_MEM_REPORT();

    CLARITY_LOG_INFO("Vanilla has shut down successfully.");
}