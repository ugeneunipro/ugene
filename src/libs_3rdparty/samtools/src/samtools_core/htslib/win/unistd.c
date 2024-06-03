#include "unistd.h"
#include <Windows.h>

int usleep(unsigned int useconds)
{
    if (useconds == 0)
        return 0;

    if (useconds >= 1000000)
        return EINVAL;

    Sleep((useconds + 999) / 1000);

    return 0;
}

