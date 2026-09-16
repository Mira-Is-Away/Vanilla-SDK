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

#include <core/vnl_status.h>
#include <core/vnl_types.h>

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
 * @brief Draws a single frame using the active rendering backend.
 * @param[in] renderer The renderer handle.
 * @retval VNL_SUCCESS If the frame was rendered successfully.
 */
VnlStatus vnl_renderer_draw(VnlRenderer *renderer);

/**
 * @brief Shuts down the rendering subsystem and frees all associated resources.
 * @param[in] renderer The renderer handle to destroy.
 */
void vnl_renderer_shutdown(VnlRenderer *renderer);

#endif
