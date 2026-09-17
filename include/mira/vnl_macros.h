/**
 * @file vnl_macros.h
 *
 * Useful general-purpose macros defined and used by the engine.
 *
 * @author Henry R
 * @date 2026-02-21
 */

#ifndef VANILLA_CORE_VANILLA_MACROS_H_
#define VANILLA_CORE_VANILLA_MACROS_H_

// Do not change this
#define VNL_ENGINE_VERSION_MAJOR 0
#define VNL_ENGINE_VERSION_MINOR 1
#define VNL_ENGINE_VERSION_PATCH 0

#if defined(_WIN32)
#if defined(VNL_BUILD_SHARED)
#define VNL_API __declspec(dllexport)
#elif defined(VNL_USE_SHARED)
#define VNL_API __declspec(dllimport)
#else
#define VNL_API
#endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#define VNL_API __attribute__((visibility("default")))
#else
#define VNL_API
#endif

/** @brief Macro with the engine's default configuration data.
 *
 * Usage:
 * VnlConfig config = VNL_DEFAULT_CONFIG;
 */
#define VNL_DEFAULT_CONFIG                                                     \
    {                                                                          \
        .window       = {1280, 720},                                           \
        .version      = {0, 0, 0},                                             \
        .title        = "Untitled Vanilla Project",                            \
        .target_fps   = 60.0f,                                                 \
        .clear_colour = {0.7f, 0.7f, 0.7f, 1.0f},                              \
    }

/** @brief Helper macro to format the game's version into a VnlGameVersion
 * struct
 *
 * Usage:
 * config.version = VNL_MAKE_VERSION(1, 0, 0);
 */
#define VNL_MAKE_VERSION(major, minor, patch)                                  \
    (VnlGameVersion) {                                                         \
        major, minor, patch                                                    \
    }

/** @brief Helper macro to format the game's window size into a VnlWindow struct
 *
 * Usage:
 * config.window = VNL_MAKE_WINDOW_SIZE(1, 0, 0);
 */
#define VNL_MAKE_WINDOW_SIZE(width, height)                                    \
    (VnlWindowConfig) {                                                        \
        width, height                                                          \
    }

/** @brief Helper macro to format RGB colour values into a VnlColour struct
 *
 * Usage:
 * config.clear_colour = VNL_MAKE_COLOUR(255, 255, 255);
 */
#define VNL_MAKE_COLOUR(r, g, b)                                               \
    (VnlColour){(float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f}

#endif