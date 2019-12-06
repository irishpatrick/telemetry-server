#ifndef SERVER_H
#define SERVER_H

#define MAX_CLIENTS 10
#define BUF_SIZE 1024
void* server_proc(void*);
static void handle_signal(int);
#endif /* SERVER_H */
