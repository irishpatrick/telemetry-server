#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "serial.h"
#include "server.h"
#include "config.h"

static pthread_t serial_thread;
static pthread_t server_thread;

static void handle_signal(int signum)
{
    switch (signum)
    {
        case SIGINT:
            printf("\nshutdown\n");
            pthread_kill(serial_thread, SIGINT);
            pthread_kill(server_thread, SIGINT);
            exit(0);
    }
}

int main(int argc, char** argv)
{
    struct sigaction sa;
    sa.sa_handler = &handle_signal;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        // TODO handle error
    }

    Config cfg;
    cfg.port = 8080;
    pthread_mutex_init(&cfg.lock, NULL);

    pthread_create(&serial_thread, NULL, serial_proc, (void*)(&cfg));
    pthread_create(&server_thread, NULL, server_proc, (void*)(&cfg));

    pthread_join(serial_thread, NULL);
    pthread_join(server_thread, NULL);

    return 0;
}
