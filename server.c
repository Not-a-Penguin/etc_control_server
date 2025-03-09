#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "server.h"
#include "controller_ back_end.h"

#define PORT 1990
#define LISTEN_BACKLOG 3

int create_server(FILE* file_fd){

    char client_ip[INET_ADDRSTRLEN];

    // Create socket

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(sock_fd == -1){

        perror("[socket]\t Failed to create socket");
        exit(0);
    }
    else{
        printf("[socket]\t Socket created successfully.\n");
    }

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    // Define socket address options

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind 

    if(bind(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1){
        perror("[socket]\t Error binding socket");
        exit(0);
    }

    printf("[socket]\t Socket bound successfully.\n");

    // Listen

    if(listen(sock_fd, LISTEN_BACKLOG) == -1){
        perror("[socket]\t Socket listening failed");
    }
    else printf("[socket]\t Server listening on port %d.\n", PORT);

    // Accept

    struct sockaddr_in peer_addr;
    socklen_t peer_addr_size = sizeof(peer_addr);
    int connection_fd = accept(sock_fd, (struct sockaddr *)&peer_addr, &peer_addr_size);

    if(connection_fd < 0){
        perror("[socket] Server failed to accept connection");
    }
    else{
        inet_ntop(AF_INET, &(peer_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("[socket]\t Client %s connected to server. Send '<close>' to cleanly exit.\n", client_ip);
    }

    // Thread for new connection
    pthread_t connectionThread;

    thread_arg_struct* args;

    args = malloc(sizeof(thread_arg_struct) * 1);

    args->socket_fd = connection_fd;
    args->file_fd = file_fd;

    int thread_ret = pthread_create(&connectionThread, NULL, process_input, args);

    pthread_join(connectionThread, NULL);
    fprintf(stdout, "[socket]\t Closing socket.\n");
    close(connection_fd);
    close(sock_fd);

    fprintf(stdout, "[server]\t Closing file.\n");

    if(fclose(file_fd) != 0){
        perror("[server]\t File failed to close");
    };

    return 0;
}