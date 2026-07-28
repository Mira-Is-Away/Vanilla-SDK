/**
 * @file vnl_status.h
 *
 * Definition of the status codes used throughout the engine.
 *
 * @author Henry R
 * @date 26-02-28
 */

#ifndef VANILLA_CORE_VNL_STATUS_H_
#define VANILLA_CORE_VNL_STATUS_H_

typedef enum VnlStatus {
    VNL_SUCCESS = 0,
    VNL_FAILURE = 1,
    VNL_ERROR_GLFW_INIT_FAILED = 2,
    VNL_ERROR_WINDOW_CREATION_FAILED = 3,
    VNL_ERROR_VULKAN_NOT_SUPPORTED = 4,
    VNL_ERROR_VULKAN_INSTANCE_CREATION_FAILED = 5,
    VNL_ERROR_PHYSICAL_DEVICE_NOT_FOUND = 6,
    VNL_ERROR_LOGICAL_DEVICE_CREATION_FAILED = 7,
    VNL_ERROR_SURFACE_CREATION_FAILED = 8,
    VNL_ERROR_OUT_OF_MEMORY = 9
} VnlStatus;

#endif