#include <mira/vanilla.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#define MIRA_CLARITY_IMPL
#include <mira/clarity.h>
#define MIRA_DARRAY_IMPL
#include <mira/darray.h>
#include <mira/vnl_types.h>
#include <renderer/vnl_renderer.h>
#include <winman/winman.h>

struct VnlEngine {
    const VnlConfig *config;
    VnlWinMan       *winman;
    VnlRenderer     *renderer;
};

static VnlEngine *g_current_context = NULL;

VnlStatus vnl_init(const VnlConfig *config) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");

    VnlStatus status;

    VnlEngine *engine = CLARITY_MALLOC(sizeof(VnlEngine));
    if (!engine) {
        CLARITY_LOG_ERROR("Vanilla has run out of available memory!");
        return VNL_ERROR_OUT_OF_MEMORY;
    }

    engine->config = config;

    status = vnl_winman_init(config, &engine->winman);
    if (status != VNL_SUCCESS) {
        CLARITY_FREE(engine);
        return status;
    }

    status = vnl_renderer_init(config, engine->winman, &engine->renderer);
    if (status != VNL_SUCCESS) {
        vnl_winman_shutdown(engine->winman);
        CLARITY_FREE(engine);
        return status;
    }

    vnl_set_context(engine);
    return VNL_SUCCESS;
}

void vnl_set_context(VnlEngine *engine) {
    g_current_context = engine;
}

VnlEngine *vnl_get_context() {
    return g_current_context;
}

bool vnl_is_running() {
    CLARITY_ASSERT(g_current_context != NULL,
                   "Vanilla's current context is NULL.");
    CLARITY_ASSERT(g_current_context->winman != NULL,
                   "Engine window manager handle is NULL.");

    return !vnl_winman_should_close(g_current_context->winman);
}

void vnl_begin_frame() {
    CLARITY_ASSERT(g_current_context != NULL,
                   "Vanilla's current context is NULL.");
    CLARITY_ASSERT(g_current_context->winman != NULL,
                   "Engine window manager handle is NULL.");
    CLARITY_ASSERT(g_current_context->renderer != NULL,
                   "Engine renderer handle is NULL.");

    vnl_winman_poll_events(g_current_context->winman);

    VnlStatus status;
    if ((status = vnl_renderer_begin_rendering(g_current_context->renderer)) !=
        VNL_SUCCESS) {
        exit(status);
    }
}

void vnl_draw() {
    CLARITY_ASSERT(g_current_context != NULL,
                   "Vanilla's current context is NULL.");
    CLARITY_ASSERT(g_current_context->renderer != NULL,
                   "Engine renderer handle is NULL.");

    vnl_renderer_draw(g_current_context->renderer);
}

void vnl_end_frame() {
    CLARITY_ASSERT(g_current_context != NULL,
                   "Vanilla's current context is NULL.");
    CLARITY_ASSERT(g_current_context->renderer != NULL,
                   "Engine renderer handle is NULL.");

    vnl_renderer_end_rendering(g_current_context->renderer);
}

void vnl_run() {
    while (vnl_is_running()) {
        vnl_begin_frame();
        vnl_draw();
        vnl_end_frame();
    }
}

void vnl_shutdown() {
    if (g_current_context) {
        vnl_renderer_shutdown(g_current_context->renderer);
        vnl_winman_shutdown(g_current_context->winman);
        CLARITY_FREE(g_current_context);
        vnl_set_context(NULL);
    }

    CLARITY_MEM_REPORT();
}
