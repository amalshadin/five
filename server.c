#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9002
#define MAX 100

// Receive exactly n bytes
int recv_all(int socket, void *buffer, int n)
{
    int total = 0;
    int bytes;

    while (total < n)
    {
        bytes = recv(socket, (char *)buffer + total, n - total, 0);

        if (bytes <= 0)
            return -1;

        total += bytes;
    }

    return total;
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int n;
    int matrix[MAX][MAX];

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

    // Listen
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client
    client_fd = accept(server_fd,
                        (struct sockaddr *)&client_addr,
                        &addr_len);

    if (client_fd < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(1);
    }

    printf("Client connected.\n");

    // Receive n
    if (recv_all(client_fd, &n, sizeof(n)) < 0)
    {
        perror("Failed to receive n");
        close(client_fd);
        close(server_fd);
        exit(1);
    }

    // Receive complete matrix
    if (recv_all(client_fd, matrix, sizeof(int) * n * n) < 0)
    {
        perror("Failed to receive matrix");
        close(client_fd);
        close(server_fd);
        exit(1);
    }

    // Display received matrix
    printf("Received matrix of order %d:\n", n);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    // Check matrix type
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            // Elements below principal diagonal
            if (i > j && matrix[i][j] != 0)
                upper = 0;

            // Elements above principal diagonal
            if (i < j && matrix[i][j] != 0)
                lower = 0;

            // All non-diagonal elements
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

    // Send result
    send(client_fd, result, strlen(result) + 1, 0);

    // Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}            if (i != j && matrix[i][j] != 0)
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
