#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define MAX_CONNECTIONS 30

void handle_client(int client_socket);
void* client_thread(void* arg);
void sigint_handler(int sig);

int server_socket;

int main(int argc, char *argv[]) {
    
    if(argc != 2){
        fprintf(stderr, "Usage: %s port \n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    signal(SIGINT, sigint_handler);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror( "Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Bind failed");
        close(server_socket);
        return 1;
    } 

    if (listen(server_socket, MAX_CONNECTIONS) < 0) {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }

    printf("Server listening on port %d\n", port);

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0){
            perror("Accept failed");
            continue;
        }

        pthread_t thread_id;
        if(pthread_create(&thread_id, NULL, client_thread, (void*)&client_socket) != 0){
            perror("Thread creation failed");
            close(client_socket);
        }
    }
    
    close(server_socket);

    return 0;
}

void* client_thread(void* arg) {
    int client_socket = *((int*)arg);
    handle_client(client_socket);
    close(client_socket);
    pthread_exit(NULL);
}

void handle_client(int client_socket){
    char buffer[1024];
    int bytes_read;

    while((bytes_read = read(client_socket, buffer, sizeof(buffer) -1)) > 0) {
        buffer[bytes_read] = '\0';
        if(strcmp(buffer, "getInfo") == 0){
            write(client_socket, "Sam service, v1.0\n", 19);
        } else if(strcmp(buffer, "getNumberOfPartitions") == 0){
            FILE *fp = popen("lsblk -l | grep part | wc -l", "r");
            if ( fp == NULL){
                write(client_socket, "Error\n", 6);
                continue;
            }
            fgets(buffer, sizeof(buffer), fp);
            pclose(fp);
            write(client_socket, buffer, strlen(buffer));
        } else if (strcmp(buffer, "getCurrentKernelVersion") == 0){
            FILE *fp = popen("uname -r", "r");
            if (fp == NULL){
                write(client_socket, "Error\n", 6);
                continue;
            }
            fgets(buffer, sizeof(buffer), fp);
            pclose(fp);
            write(client_socket, buffer, strlen(buffer));
        } else if (strcmp(buffer, "sshdRunning") == 0){
            FILE *fp = popen("ps ax | grep sshd | grep -v grep", "r");
            if (fp == NULL) {
                write(client_socket, "false\n", 6);
                continue;
            }
            if (fgets(buffer, sizeof(buffer), fp) != NULL) {
                write(client_socket, "true\n", 5);
            } else {
                write(client_socket, "false\n", 6);
            }
            pclose(fp);
        } else {
            write(client_socket, "Invalid command\n", 17);
        }
    }
}


void sigint_handler(int sig) {
    printf("Caught signal %d, terminating server...\n", sig);
    close(server_socket);
    exit(EXIT_SUCCESS);
}