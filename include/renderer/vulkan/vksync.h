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
#include <vulkan/vulkan.h>

/**
 * @def VNL_MAX_FRAMES_IN_FLIGHT
 * @brief Maximum number of frames that can be processed concurrently.
 */
#define VNL_MAX_FRAMES_IN_FLIGHT 2

/**
 * @struct VkSync
 * @brief Synchronization primitives per frame in flight.
 */
typedef struct {
    VkDevice    device;
    VkSemaphore image_available;
    VkSemaphore render_finished;
    VkFence     in_flight;
} VkSync;

/**
 * @struct VkSyncDesc
 * @brief Descriptor containing parameters for creating synchronization objects.
 */
typedef struct {
    VkDevice device;
} VkSyncDesc;

/**
 * @brief Creates synchronization objects (semaphores and fence) for frame
 * rendering.
 * @param[in] desc Pointer to the synchronization descriptor.
 * @param[out] out_sync Pointer to store the created synchronization primitives.
 * @retval VNL_SUCCESS If synchronization objects were created successfully.
 * @retval VNL_ERROR_SYNC_OBJECTS_CREATION_FAILED If object creation failed.
 */
VnlStatus vk_sync_create(const VkSyncDesc *desc, VkSync *out_sync);

/**
 * @brief Destroys synchronization primitives (semaphores and fences).
 * @param[in] sync The synchronization struct containing objects to destroy.
 */
void vk_sync_destroy(VkSync sync);

/**
 * @brief Blocks the host until the in-flight frame fence is signaled.
 * @param[in] sync Pointer to the synchronization primitives.
 */
void vk_wait_in_flight(const VkSync *sync);

/**
 * @brief Resets the in-flight frame fence to the unsignaled state.
 * @param[in,out] sync Pointer to the synchronization primitives.
 */
void vk_reset_in_flight(VkSync *sync);

#endif