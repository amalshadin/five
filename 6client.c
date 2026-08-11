#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000
#define SIZE 4096

int main()
{
    int sockfd;
    char buffer[SIZE];

    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Server address
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Input sentence
    printf("Enter new-generation English sentence:\n");

    fgets(buffer, SIZE, stdin);

    buffer[strcspn(buffer, "\n")] = '\0';

    // Send to server
    sendto(sockfd, buffer, strlen(buffer) + 1, 0,
           (struct sockaddr *)&server,
           server_len);

    // Receive translated sentence
    memset(buffer, 0, SIZE);

    recvfrom(sockfd, buffer, SIZE, 0,
             (struct sockaddr *)&server,
             &server_len);

    printf("\nFormal English:\n");
    printf("%s\n", buffer);

    close(sockfd);

    return 0;
}
