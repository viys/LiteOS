#include "os.h"

bool os_while(uint32_t timestamp)
{
    static os_event_t evt;

    os_time_run(timestamp);

    if (os_event_get(&evt)) {
        os_event_dispatch(&evt);
    }

}
