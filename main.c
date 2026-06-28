#include <stdlib.h>
#include <stdio.h>

#include "vanilla.h"

#include "mira/clarity.h"

// TODO: Add Vulkan validation layers

int main() {
    /*
    User creates and owns the config.
    There is a VNL_DEFAULT_CONFIG macro that populates a 
    basic but valid config struct. 
    */
    VnlConfig config = VNL_DEFAULT_CONFIG;
    config.title = "Test Sandbox";
    config.version.minor = 1;

    VnlEngine* engine = NULL;

    VnlStatus status = vnl_init(&config, &engine);

    if(status != VNL_SUCCESS) {
        printf("Failed to initialise Vanilla SDK (Error Code: %d). Terminating program...\n", status);
        exit(EXIT_FAILURE);
    }

    vnl_run(engine);

    vnl_shutdown(engine);

    return 0;
    
}