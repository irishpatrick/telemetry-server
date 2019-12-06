#ifndef CONFIG_H
#define CONFIG_H

#include <pthread.h>

typedef struct _Config
{
    // common stuff
    pthread_mutex_t lock;
    // serial stuff
    char fn[100];
    // server stuff
    long port;
} Config;

#endif /* CONFIG_H */
