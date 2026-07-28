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

#include "core/vnl_macros.h"
#include "core/vnl_status.h"
#include "core/vnl_types.h"

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

typedef struct VnlEngine VnlEngine;

/**
 * @brief Initialises the engine and its components.
 * @param[in] config The configuration for the engine.
 * @param[out] out_engine A pointer to store the created engine instance.
 * @retval VNL_SUCCESS If initialisation was successful.
 */
VnlStatus vnl_init(const VnlConfig *config, VnlEngine **out_engine);

/**
 * @brief Runs the engine's main loop.
 * @param[in] engine The engine instance to run.
 */
void vnl_run(VnlEngine *engine);

/**
 * @brief Shuts down the engine, deallocating all used memory.
 * @param[in] engine The engine instance to shut down.
 */
void vnl_shutdown(VnlEngine *engine);

#endif