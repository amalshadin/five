#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9002

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int n;
    int matrix[100][100];

    int upper = 1;
    int lower = 1;
    int diagonal = 1;

    char result[50];

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    // Listen for clients
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                        &addr_len);

    if (client_fd < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(1);
    }

    printf("Client connected.\n");

    // Receive matrix order
    if (recv(client_fd, &n, sizeof(n), 0) <= 0)
    {
        perror("Failed to receive n");
        close(client_fd);
        close(server_fd);
        exit(1);
    }

    // Receive matrix
    if (recv(client_fd, matrix, sizeof(int) * n * n, 0) <= 0)
    {
        perror("Failed to receive matrix");
        close(client_fd);
        close(server_fd);
        exit(1);
    }

    printf("Received matrix of order %d:\n", n);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%4d ", matrix[i][j]);
        }
        printf("\n");
    }

    // Check matrix type
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i > j && matrix[i][j] != 0)
                upper = 0;

            if (i < j && matrix[i][j] != 0)
                lower = 0;

            if (i != j && matrix[i][j] != 0)
                diagonal = 0;
        }
    }

    // Determine matrix type
    if (diagonal)
        strcpy(result, "Diagonal Matrix");
    else if (upper)
        strcpy(result, "Upper Triangular Matrix");
    else if (lower)
        strcpy(result, "Lower Triangular Matrix");
    else
        strcpy(result, "Normal Matrix");

    printf("Matrix type: %s\n", result);

    // Send result to client
    send(client_fd, result, strlen(result) + 1, 0);

    // Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
