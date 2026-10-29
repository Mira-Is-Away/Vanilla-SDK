/**
 * @file vnl_renderer.h
 *
 * Defines the abstract interface for the rendering subsystem.
 *
 * @author Henry R
 * @date 2026-09-08
 */

#ifndef VANILLA_RENDERER_VNL_RENDERER_H_
#define VANILLA_RENDERER_VNL_RENDERER_H_

#include <mira/vnl_status.h>
#include <mira/vnl_types.h>

typedef struct VnlWinMan   VnlWinMan;
typedef struct VnlRenderer VnlRenderer;

/**
 * @brief Initialises the rendering subsystem.
 * @param[in] config The configuration for the engine.
 * @param[in] winman The window manager handle to bind the renderer to.
 * @param[out] out_renderer Pointer to store the created renderer handle.
 * @retval VNL_SUCCESS If initialization was successful.
 */
VnlStatus vnl_renderer_init(const VnlConfig *config, VnlWinMan *winman,
                            VnlRenderer **out_renderer);

/**
 * @brief Clears the current swapchain image with the configured clear colour.
 * @param[in] renderer The renderer handle.
 */
void vnl_renderer_clear_image(VnlRenderer *renderer);

/**
 * @brief Prepares the renderer and begins command recording for the current
 * frame.
 * @param[in] renderer The renderer handle.
 * @retval VNL_SUCCESS If the rendering stage was started successfully.
 * @retval VNL_FAILURE If acquiring the next swapchain image failed.
 * @retval VNL_ERROR_COMMAND_BUFFER_BEGIN_RECORDING_FAILED If beginning command
 * buffer recording failed.
 */
VnlStatus vnl_renderer_begin_rendering(VnlRenderer *renderer);

/**
 * @brief Records draw commands for the active frame.
 * @param[in] renderer The renderer handle.
 */
void vnl_renderer_draw(VnlRenderer *renderer);

/**
 * @brief Concludes command recording, submits commands, and presents the frame.
 * @param[in] renderer The renderer handle.
 * @retval VNL_SUCCESS If the frame was successfully ended and presented.
 * @retval VNL_FAILURE If queue submission or presentation failed.
 * @retval VNL_ERROR_COMMAND_BUFFER_END_RECORDING_FAILED If ending command
 * buffer recording failed.
 */
VnlStatus vnl_renderer_end_rendering(VnlRenderer *renderer);

/**
 * @brief Shuts down the rendering subsystem and frees all associated resources.
 * @param[in] renderer The renderer handle to destroy.
 */
void vnl_renderer_shutdown(VnlRenderer *renderer);

#endif
