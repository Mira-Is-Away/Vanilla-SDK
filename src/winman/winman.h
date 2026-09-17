/**
 * @file winman.h
 *
 * Defines window management interface used by the engine internally..
 *
 * @date 17-03-26
 * @author Henry R
 */

#ifndef VANILLA_WINMAN_WINMAN_H_
#define VANILLA_WINMAN_WINMAN_H_

#include <stdbool.h>

#include <mira/vnl_status.h>
#include <mira/vnl_types.h>
#include <vulkan/vulkan.h>

typedef struct GLFWwindow GLFWwindow;
typedef struct VnlWinMan  VnlWinMan;

/**
 * @brief Initialises the window manager and creates a window.
 * @param[in] config The configuration struct provided by the user.
 * @param[out] out_winman A double pointer to store the created window manager
 * handle.
 * @retval VNL_SUCCESS If window manager initialisation was successful.
 */
VnlStatus vnl_winman_init(const VnlConfig *config, VnlWinMan **out_winman);

/**
 * @brief Returns the underlying GLFW window pointer (for low-level access if
 * needed).
 * @param[in] winman The window manager handle.
 * @return Pointer to the GLFWwindow.
 */
GLFWwindow *vnl_winman_get_window(VnlWinMan *winman);

/**
 * @brief Checks if the window has received a close request.
 * @param[in] winman The window manager handle.
 * @return True if the window should close, false otherwise.
 */
bool vnl_winman_should_close(VnlWinMan *winman);

/**
 * @brief Polls window and input events.
 * @param[in] winman The window manager handle.
 */
void vnl_winman_poll_events(VnlWinMan *winman);

/**
 * @brief Gets required Vulkan instance extensions for the windowing system.
 * @param[out] out_count Pointer to store the number of extensions.
 * @return Array of extension name strings.
 */
const char **vnl_winman_get_required_extensions(u32 *out_count);

/**
 * @brief Creates a Vulkan surface for the managed window.
 * @param[in] winman The window manager handle.
 * @param[in] instance The Vulkan instance.
 * @param[in] allocator Optional Vulkan allocation callbacks.
 * @param[out] out_surface Pointer to store the created surface.
 * @retval VNL_SUCCESS If surface creation was successful.
 * @retval VNL_ERROR_SURFACE_CREATION_FAILED If surface creation failed.
 */
VnlStatus
vnl_winman_create_vulkan_surface(VnlWinMan *winman, VkInstance instance,
                                 const VkAllocationCallbacks *allocator,
                                 VkSurfaceKHR                *out_surface);

/**
 * @brief Retrieves the current framebuffer size in pixels.
 * @param[in] winman The window manager handle.
 * @param[out] out_width Pointer to store width.
 * @param[out] out_height Pointer to store height.
 */
void vnl_winman_get_framebuffer_size(VnlWinMan *winman, int *out_width,
                                     int *out_height);

/**
 * @brief Shuts down the window manager and terminates the windowing subsystem.
 * @param winman The window manager to shut down.
 */
void vnl_winman_shutdown(VnlWinMan *winman);

#endif