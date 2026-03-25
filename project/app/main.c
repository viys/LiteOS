#include <windows.h>
#include "my_config.h"
#include "os.h"

int main(int argc, char *argv[]) {
    printf("Application : %s\n", CONFIG_APP_NAME);
    printf("Version     : %d.%d.%d\n",
           CONFIG_APP_VERSION_MAJOR,
           CONFIG_APP_VERSION_MINOR,
           CONFIG_APP_VERSION_PATCH);

#if CONFIG_DEBUG_ENABLE
    printf("[DEBUG] Debug mode enabled (level %d)\n", CONFIG_DEBUG_LEVEL);
#endif

    while(1) {
        // 时间戳更新
        // 任务轮询
        // DWORD ms = GetTickCount();
        // printf("%d\r\n", ms);
        os_while();
    }

    return 0;
}
