/**
 * @file vanilla.h
 *
 * The entry point to interact with the Vanilla engine.
 *
 * @author Henry R
 * @date 2025-12-09
 */

#ifndef VANILLA_VANILLA_H_
#define VANILLA_VANILLA_H_

#include <stdbool.h>

#include <mira/vnl_macros.h>
#include <mira/vnl_status.h>
#include <mira/vnl_types.h>

typedef struct VnlEngine VnlEngine;

/**
 * @brief Initialises the engine and its components.
 * @param[in] config The configuration for the engine.
 * @retval VNL_SUCCESS If initialisation was successful.
 */
VNL_API VnlStatus vnl_init(const VnlConfig *config);

/**
 * @brief Sets the current engine context used by the library.
 * @param[in] engine A pointer to the desired context.
 */
VNL_API void vnl_set_context(VnlEngine *engine);

/**
 * @brief Retrieves a pointer to the current engine context being used.
 * @return A pointer to the current context.
 */
VNL_API VnlEngine *vnl_get_context();

/**
 * @brief Checks whether the engine is currently running.
 * @return True if the engine is running and the window should remain open,
 * false otherwise.
 */
VNL_API bool vnl_is_running();

/**
 * @brief Begins a new frame by polling window events and starting renderer
 * recording.
 */
VNL_API void vnl_begin_frame();

/**
 * @brief Records and emits draw commands for the active frame.
 */
VNL_API void vnl_draw();

/**
 * @brief Concludes the frame by submitting queued commands and presenting the
 * swapchain image.
 */
VNL_API void vnl_end_frame();

/**
 * @brief Runs the engine's main loop.
 */
VNL_API void vnl_run();

/**
 * @brief Shuts down the engine, deallocating all used memory.
 */
VNL_API void vnl_shutdown();

#endif