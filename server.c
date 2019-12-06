#include "server.h"
#include "config.h"
#include "serial.h"
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

static pthread_t clients[MAX_CLIENTS];
static int client_map[MAX_CLIENTS];
static int running;
static int sockfd, connfd, len;

void* client_proc(void* ptr)
{
    int* info = (int*)ptr;
    int id = info[1];
    int connfd = info[0];

    char buffer[BUF_SIZE];

    while (1)
    {
        memset(buffer, 0, BUF_SIZE);

        strcpy(buffer, "ack");
        write(connfd, buffer, BUF_SIZE);

        memset(buffer, 0, BUF_SIZE);

        size_t n = read(connfd, buffer, BUF_SIZE);
        printf("from client: %s\n", buffer);
    }

    client_map[id] = 0;
    free(ptr);
}

void* server_proc(void* ptr)
{
    struct sigaction sa;
    sa.sa_handler = &handle_signal;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        // TODO handle error
    }
    
    running = 1;
    memset(client_map, -1, sizeof(int) * MAX_CLIENTS);

    Config* cfg = (Config*)ptr;
    pthread_mutex_t* lock = &cfg->lock;

    pthread_mutex_lock(lock);
    printf("server: starting\n");
    pthread_mutex_unlock(lock);

    struct sockaddr_in servaddr, client;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("server: cannot create socket\n");
        return NULL;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(cfg->port);

    int err;
    err = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (err != 0)
    {
        printf("server: cannot bind port");
        return NULL;
    }

    err = listen(sockfd, 5);
    if (err != 0)
    {
        printf("server: cannot open port");
        return NULL;
    }

    while (running)
    {
        len = sizeof(client);
        printf("listen\n");
        connfd = accept(sockfd, (struct sockaddr*)&client, &len);
        if (connfd == -1)
        {
            printf("bad conn\n");
            continue;
        }

        int i;
        pthread_t* worker = NULL;
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_map[i] <= 0)
            {
                worker = &clients[i];
                client_map[i] = 1;
                break;
            }
        }
        if (worker == NULL)
        {
            continue;
        }

        int* info = malloc(2 * sizeof(int));
        info[0] = connfd;
        info[1] = i;
        pthread_create(worker, NULL, client_proc, (void*)info);
    }
    
    close(sockfd);

    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_map[i] != -1)
        {
            pthread_join(clients[i], NULL);
        }
    }

    printf("server: shutting down\n");

    return NULL;
}

static void handle_signal(int signum)
{
    switch(signum)
    {
        case SIGINT:
            close(sockfd);

            int i;
            for (i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_map[i] != -1)
                {
                    pthread_kill(clients[i], SIGINT);
                }
            }
            exit(0);
    }
}
