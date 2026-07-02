/**
 * @file window.h
 *
 * Defines utility functions to handle windows within the engine.
 *
 * @date 17-03-26
 * @author Henry R
 */

#ifndef VANILLA_GLFW_WINDOW_H_
#define VANILLA_GLFW_WINDOW_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "misc/vnl_status.h"

typedef struct VnlConfig VnlConfig;
typedef struct VnlWindow VnlWindow;

/**
 * @brief Creates a GLFW window.
 * @param[in] config The configuration for the window.
 * @param[out] out_window A pointer to store the created window handle.
 * @retval VNL_SUCCESS If window creation was successful.
 */
VnlStatus vnl_window_create(const VnlConfig *config, GLFWwindow **out_window);

/**
 * @brief Destroys a GLFW window.
 * @param window The window handle to destroy.
 */
void vnl_window_destroy(GLFWwindow *window);

#endif