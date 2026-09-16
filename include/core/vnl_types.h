/**
 * @file vnl_types.h
 *
 * Common type definitions and primitive types used throughout the engine.
 *
 * @author Henry R
 * @date 2026-09-14
 */

#ifndef VANILLA_CORE_VNL_TYPES_H_
#define VANILLA_CORE_VNL_TYPES_H_

#include <stdint.h>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

/**
 * @struct VnlVec2
 * @brief Two-dimensional integer vector.
 */
typedef struct VnlVec2 {
    i32 x;
    i32 y;
} VnlVec2;

/**
 * @struct VnlColour
 * @brief RGBA colour structure with normalized floating-point components (0.0f
 * - 1.0f).
 */
typedef struct VnlColour {
    f32 r;
    f32 g;
    f32 b;
    f32 a;
} VnlColour;

/**
 * @struct VnlWindow
 * @brief Window dimensions in pixels.
 */
typedef struct VnlWindowConfig {
    u16 width;
    u16 height;
} VnlWindowConfig;

/**
 * @struct VnlGameVersion
 * @brief Semantic version structure (major, minor, patch).
 */
typedef struct VnlGameVersion {
    u16 major;
    u16 minor;
    u16 patch;
} VnlGameVersion;

/**
 * @struct VnlConfig
 * @brief Configuration settings for initialising the engine.
 */
typedef struct VnlConfig {
    VnlWindowConfig window;
    VnlGameVersion  version;
    const char     *title;
    f32             target_fps;
    VnlColour       clear_colour;
} VnlConfig;

#endif