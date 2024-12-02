#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define SERVER_PORT "58065"
#define SERVER_ADDR "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct addrinfo hints, *server_info, *p;
    char send_buffer[BUFFER_SIZE] = "Hello, Server!";
    ssize_t bytes_sent;

    // Set up hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP

    // Get address info
    int status = getaddrinfo(SERVER_ADDR, SERVER_PORT, &hints, &server_info);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return EXIT_FAILURE;
    }

    // Debugging: Print resolved addresses
    for (p = server_info; p != NULL; p = p->ai_next) {
        char ip_str[INET_ADDRSTRLEN];
        struct sockaddr_in *addr = (struct sockaddr_in *)p->ai_addr;
        inet_ntop(p->ai_family, &(addr->sin_addr), ip_str, sizeof(ip_str));
        printf("Resolved server address: %s\n", ip_str);
    }

    // Create socket
    for (p = server_info; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to create socket\n");
        freeaddrinfo(server_info);
        return EXIT_FAILURE;
    }

    printf("Socket created successfully.\n");

    // Send message to the server
    bytes_sent = sendto(sockfd, send_buffer, strlen(send_buffer), 0, p->ai_addr, p->ai_addrlen);
    if (bytes_sent == -1) {
        perror("sendto");
        close(sockfd);
        freeaddrinfo(server_info);
        return EXIT_FAILURE;
    }

    printf("Message sent to server: %s\n", send_buffer);

    // Clean up
    close(sockfd);
    freeaddrinfo(server_info);
    return EXIT_SUCCESS;
}
