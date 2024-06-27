#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 3131

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char command[1024];
    char buffer[1024];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Enter commands:\n");
    while (1) {
        printf("> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        if (send(client_socket, command, strlen(command), 0) < 0) {
            perror("Send failed");
            break;
        }

        if (strcmp(command, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            perror("Receive failed");
            break;
        } else if (bytes_received == 0) {
            printf("Server disconnected.\n");
            break;
        } else {
            buffer[bytes_received] = '\0';
            printf("Server response: %s\n", buffer);
        }
    }

    close(client_socket);
    return 0;
}
