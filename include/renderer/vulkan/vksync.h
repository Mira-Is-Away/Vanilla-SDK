/**
 * @file vksync.h
 *
 * Synchronization primitives (semaphores and fences) for frame rendering.
 *
 * @author Henry R
 * @date 2026-09-14
 */

#ifndef VNL_VULKAN_VKSYNC_H_
#define VNL_VULKAN_VKSYNC_H_

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <mira/darray.h>
#include <vulkan/vulkan.h>

/**
 * @def VNL_MAX_FRAMES_IN_FLIGHT
 * @brief Maximum number of frames that can be processed concurrently.
 */
#define VNL_MAX_FRAMES_IN_FLIGHT 2

/**
 * @struct VkFrameSync
 * @brief Synchronization primitives per frame in flight.
 */
typedef struct {
    VkSemaphore image_available;
    VkFence     in_flight;
} VkFrameSync;

/**
 * @struct VkSync
 * @brief Top-level Vulkan synchronization manager.
 */
typedef struct VkSync {
    VkDevice            device;
    VkFrameSync         frames[VNL_MAX_FRAMES_IN_FLIGHT];
    DARRAY(VkSemaphore) render_finished;
} VkSync;

/**
 * @struct VkSyncDesc
 * @brief Descriptor containing parameters for creating synchronization objects.
 */
typedef struct {
    VkDevice device;
    u32      swapchain_image_count;
} VkSyncDesc;

/**
 * @brief Creates synchronization objects for frames in flight and swapchain
 * render completion.
 * @param[in] desc Pointer to the synchronization descriptor.
 * @param[out] out_sync Pointer to store the created synchronization manager.
 * @retval VNL_SUCCESS If synchronization objects were created successfully.
 * @retval VNL_ERROR_SYNC_OBJECTS_CREATION_FAILED If object creation failed.
 */
VnlStatus vk_sync_create(const VkSyncDesc *desc, VkSync *out_sync);

/**
 * @brief Destroys all synchronization primitives in VkSync.
 * @param[in,out] sync Pointer to the synchronization struct to destroy.
 */
void vk_sync_destroy(VkSync *sync);

/**
 * @brief Blocks the host until the specified in-flight frame fence is signaled.
 * @param[in] sync Pointer to the synchronization primitives.
 * @param[in] frame_index Index of the frame in flight.
 */
void vk_wait_in_flight(const VkSync *sync, u32 frame_index);

/**
 * @brief Resets the specified in-flight frame fence to the unsignaled state.
 * @param[in,out] sync Pointer to the synchronization primitives.
 * @param[in] frame_index Index of the frame in flight.
 */
void vk_reset_in_flight(VkSync *sync, u32 frame_index);

#endif