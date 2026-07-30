#include <vulkan/vkshadermodules.h>

#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SHADER_DIR
#define SHADER_DIR "./shaders/" // Fallback for manual compilation
#endif

char *vk_shader_read(const char *filename, size_t *out_size) {
    char fullpath[4096];

    snprintf(fullpath, sizeof(fullpath), "%s%s", SHADER_DIR, filename);

    FILE *f = fopen(fullpath, "rb");
    if (!f) {
        CLARITY_LOG_ERROR("Failed to open %s", fullpath);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long int fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize <= 0) {
        fclose(f);
        return NULL;
    }

    char *buffer = (char *)malloc(fsize);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, fsize, f);
    fclose(f);

    if (out_size) {
        *out_size = bytes_read;
    }

    return buffer;
}