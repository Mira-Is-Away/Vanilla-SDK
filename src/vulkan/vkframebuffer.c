#include <vulkan/vkframebuffer.h>

#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif

#include <core/vnl_status.h>
#include <mira/clarity.h>

VnlStatus vk_framebuffers_create() {

    CLARITY_LOG_WARN("Dummy framebuffer creation");

    return VNL_SUCCESS;
}