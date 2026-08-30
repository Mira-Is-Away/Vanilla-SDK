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
    VNL_SUCCESS                               = 0,
    VNL_FAILURE                               = 1,
    VNL_ERROR_OUT_OF_MEMORY                   = 2,
    VNL_ERROR_GLFW_INIT_FAILED                = 3,
    VNL_ERROR_WINDOW_CREATION_FAILED          = 4,
    VNL_ERROR_VULKAN_NOT_SUPPORTED            = 5,
    VNL_ERROR_VULKAN_INSTANCE_CREATION_FAILED = 6,
    VNL_ERROR_PHYSICAL_DEVICE_NOT_FOUND       = 7,
    VNL_ERROR_LOGICAL_DEVICE_CREATION_FAILED  = 8,
    VNL_ERROR_SURFACE_CREATION_FAILED         = 9,
    VNL_ERROR_SWAPCHAIN_CREATION_FAILED       = 10,
    VNL_ERROR_IMAGE_VIEW_CREATION_FAILED      = 11,
    VNL_ERROR_SHADER_CREATION_FAILED          = 12,
    VNL_ERROR_PIPELINE_CREATION_FAILED        = 13,
    VNL_ERROR_RENDER_PASS_CREATION_FAILED     = 14,
    VNL_ERROR_FRAMEBUFFER_CREATION_FAILED     = 15,
    VNL_ERROR_COMMAND_POOL_CREATION_FAILED    = 16
} VnlStatus;

#endif