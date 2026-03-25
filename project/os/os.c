#include "os.h"

bool os_while(void)
{
    os_event_t evt;

    os_time_run();

    if (os_event_get(&evt)) {
        os_event_dispatch(&evt);
    }

}
