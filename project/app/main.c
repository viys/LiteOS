#include <stdio.h>
#include "my_config.h"

int main(int argc, char *argv[]) {
    printf("Application : %s\n", CONFIG_APP_NAME);
    printf("Version     : %d.%d.%d\n",
           CONFIG_APP_VERSION_MAJOR,
           CONFIG_APP_VERSION_MINOR,
           CONFIG_APP_VERSION_PATCH);

#if CONFIG_DEBUG_ENABLE
    printf("[DEBUG] Debug mode enabled (level %d)\n", CONFIG_DEBUG_LEVEL);
#endif

    return 0;
}
