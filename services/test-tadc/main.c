#include <fnode/service.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

void handler(uint32_t cmd, char const * data, uint32_t size)
{
    char text[size + 1];
    memcpy(text, data, size);
    text[size] = 0;
    printf("Got data from controller: %s\n", text);
}

int main(int argc, char ** argv)
{
    srand(time(0));

    if (argc < 2)
    {
        fprintf(stderr, "Usage: test-cocl SERIAL-NUMBER\n");
        return -1;
    }

    fnode_service_t * service = fnode_service_create(argv[1], "TADC");
    if ( !service )
    {
        fprintf(stderr, "Failed to create COCL service\n");
        return -1;
    }

    fnode_service_reg_handler(service, handler);

    time_t last_time = 0;
    size_t i = 0;

    for(;;)
    {
        fnode_service_update(service);

        time_t time_now = time(0);
        if (time_now - last_time > 1)
        {
            char data[256];
            snprintf(data, sizeof data, "%d%d",
                    0,
                    (int)(30 * sin(i * 0.1f)));
            fnode_service_notify_state(service, data, strlen(data));
            last_time = time_now;
            ++i;
        }
    }

    fnode_service_release(service);
    return 0;
}
