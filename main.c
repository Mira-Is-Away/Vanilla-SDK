#include <stdlib.h>
#include <stdio.h>

#include <mira/vanilla.h>

int main(void) {
    /*
    User creates and owns the config.
    There is a VNL_DEFAULT_CONFIG macro that populates a 
    basic but valid config struct. 
    */
    VnlConfig config = VNL_DEFAULT_CONFIG;
    config.title = "Vanilla SDK v1.0 --- Testing sandbox";
    config.version = VNL_MAKE_VERSION(1, 0, 0);
    config.window = VNL_MAKE_WINDOW_SIZE(800, 600);
    config.clear_colour = VNL_MAKE_COLOUR(160, 160, 160);
    
    VnlEngine *engine = NULL;

    VnlStatus status = vnl_init(&config, &engine);

    if(status != VNL_SUCCESS) {
        printf("Failed to initialise Vanilla SDK (Error Code: %d). Terminating program...\n", status);
        exit(EXIT_FAILURE);
    }

    vnl_run(engine);

    vnl_shutdown(engine);

    return 0;
    
}