#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#define PORT 5000
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

// Store connected clients
int clients[MAX_CLIENTS];
int client_count = 0;

// Mutex to protect the clients array
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


// Add a client to the clients array
void add_client(int client_socket)
{
    pthread_mutex_lock(&clients_mutex);

    if (client_count < MAX_CLIENTS)
    {
        clients[client_count] = client_socket;
        client_count++;
    }

    pthread_mutex_unlock(&clients_mutex);
}


// Remove a client from the clients array
void remove_client(int client_socket)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < client_count; i++)
    {
        if (clients[i] == client_socket)
        {
            // Shift everything after this client one position left
            for (int j = i; j < client_count - 1; j++)
            {
                clients[j] = clients[j + 1];
            }

            client_count--;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}


// Send a message to every connected client except the sender
void broadcast_message(char *message, int sender_socket)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < client_count; i++)
    {
        if (clients[i] != sender_socket)
        {
            send(clients[i], message, strlen(message), 0);
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}


// Function executed by each client thread
void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;

    // We don't need the dynamically allocated argument anymore
    free(arg);

    char buffer[BUFFER_SIZE];

    printf("Client connected: socket %d\n", client_socket);

    while (1)
    {
        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);

        // Receive message from client
        int bytes_received = recv(
            client_socket,
            buffer,
            BUFFER_SIZE - 1,
            0
        );

        // Client disconnected or error occurred
        if (bytes_received <= 0)
        {
            printf("Client disconnected: socket %d\n", client_socket);
            break;
        }

        buffer[bytes_received] = '\0';

        printf("Client %d: %s", client_socket, buffer);

        // Send message to all other clients
        broadcast_message(buffer, client_socket);
    }

    // Remove client from shared list
    remove_client(client_socket);

    // Close client's socket
    close(client_socket);

    return NULL;
}


int main()
{
    int server_socket;

    struct sockaddr_in server_address;

    // --------------------------------------------------
    // 1. Create TCP socket
    // --------------------------------------------------

    server_socket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (server_socket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("Server socket created successfully.\n");


    // --------------------------------------------------
    // 2. Configure server address
    // --------------------------------------------------

    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;

    // Accept connections from any IPv4 interface
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Convert port number to network byte order
    server_address.sin_port = htons(PORT);


    // --------------------------------------------------
    // 3. Bind socket
    // --------------------------------------------------

    if (bind(
        server_socket,
        (struct sockaddr *)&server_address,
        sizeof(server_address)
    ) < 0)
    {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Socket bound to port %d.\n", PORT);


    // --------------------------------------------------
    // 4. Listen for clients
    // --------------------------------------------------

    if (listen(server_socket, 10) < 0)
    {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening...\n");


    // --------------------------------------------------
    // 5. Accept clients continuously
    // --------------------------------------------------

    while (1)
    {
        struct sockaddr_in client_address;
        socklen_t client_address_length =
            sizeof(client_address);

        int client_socket = accept(
            server_socket,
            (struct sockaddr *)&client_address,
            &client_address_length
        );

        if (client_socket < 0)
        {
            perror("accept");
            continue;
        }


        // Print client information
        printf(
            "New connection from %s:%d\n",
            inet_ntoa(client_address.sin_addr),
            ntohs(client_address.sin_port)
        );


        // --------------------------------------------------
        // Add client to shared client list
        // --------------------------------------------------

        add_client(client_socket);


        // --------------------------------------------------
        // Create a thread for this client
        // --------------------------------------------------

        pthread_t thread;

        // Allocate memory for the socket argument
        int *client_socket_ptr =
            malloc(sizeof(int));

        if (client_socket_ptr == NULL)
        {
            perror("malloc");

            remove_client(client_socket);
            close(client_socket);

            continue;
        }

        *client_socket_ptr = client_socket;


        if (pthread_create(
            &thread,
            NULL,
            handle_client,
            client_socket_ptr
        ) != 0)
        {
            perror("pthread_create");

            free(client_socket_ptr);
            remove_client(client_socket);
            close(client_socket);

            continue;
        }


        // We don't need to wait for this thread.
        // It can clean itself up when it finishes.
        pthread_detach(thread);
    }


    // --------------------------------------------------
    // Cleanup
    // --------------------------------------------------

    close(server_socket);

    pthread_mutex_destroy(&clients_mutex);

    return 0;
}
