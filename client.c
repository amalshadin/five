#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#define PORT 5000
#define BUFFER_SIZE 1024


// Thread responsible for receiving messages
void *receive_messages(void *arg)
{
    int socket = *(int *)arg;

    char buffer[BUFFER_SIZE];

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        int bytes_received = recv(
            socket,
            buffer,
            BUFFER_SIZE - 1,
            0
        );

        if (bytes_received <= 0)
        {
            printf("\nDisconnected from server.\n");
            break;
        }

        buffer[bytes_received] = '\0';

        printf("\n%s", buffer);
        printf("You: ");
        fflush(stdout);
    }

    return NULL;
}


int main()
{
    int client_socket;

    struct sockaddr_in server_address;


    // Create TCP socket
    client_socket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (client_socket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }


    // Configure server address
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Server is running on localhost
    inet_pton(
        AF_INET,
        "127.0.0.1",
        &server_address.sin_addr
    );


    // Connect to server
    if (connect(
        client_socket,
        (struct sockaddr *)&server_address,
        sizeof(server_address)
    ) < 0)
    {
        perror("connect");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server!\n");


    // Create receiving thread
    pthread_t receive_thread;

    pthread_create(
        &receive_thread,
        NULL,
        receive_messages,
        &client_socket
    );


    // Main thread handles sending
    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("You: ");
        fflush(stdout);

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            break;
        }

        send(
            client_socket,
            buffer,
            strlen(buffer),
            0
        );
    }


    close(client_socket);

    return 0;
}
