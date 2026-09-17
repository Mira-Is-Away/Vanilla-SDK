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

VnlStatus vnl_init(const VnlConfig *config, VnlEngine **out_engine) {
    CLARITY_ASSERT(config != NULL, "Config cannot be NULL.");
    CLARITY_ASSERT(out_engine != NULL, "**out_engine must not be NULL.");

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

    *out_engine = engine;
    return VNL_SUCCESS;
}

void vnl_run(VnlEngine *engine) {
    CLARITY_ASSERT(engine != NULL, "Engine handle is NULL.");
    CLARITY_ASSERT(engine->winman != NULL,
                   "Engine window manager handle is NULL.");
    CLARITY_ASSERT(engine->renderer != NULL, "Engine renderer handle is NULL.");
    if (!engine || !engine->winman || !engine->renderer)
        return;

    while (!vnl_winman_should_close(engine->winman)) {
        vnl_winman_poll_events(engine->winman);
        vnl_renderer_draw(engine->renderer);
    }
}

void vnl_shutdown(VnlEngine *engine) {
    if (!engine)
        return;

    vnl_renderer_shutdown(engine->renderer);
    vnl_winman_shutdown(engine->winman);
    CLARITY_FREE(engine);

    CLARITY_MEM_REPORT();
}
