#include "serial.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

void* serial_proc(void* ptr)
{
    Config* cfg = (Config*)ptr;
    pthread_mutex_t* lock = &cfg->lock;

    pthread_mutex_lock(lock);
    printf("serial: starting\n");
    pthread_mutex_unlock(lock);
}
