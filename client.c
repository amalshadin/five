#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9002
#define MAX 100

// Send exactly n bytes
int send_all(int socket, void *buffer, int n)
{
    int total = 0;
    int bytes;

    while (total < n)
    {
        bytes = send(socket, (char *)buffer + total, n - total, 0);

        if (bytes <= 0)
            return -1;

        total += bytes;
    }

    return total;
}

int main()
{
    int sock;
    struct sockaddr_in server_addr;

    int n;
    int matrix[MAX][MAX];

    char result[50];

    // Create TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        exit(1);
    }

    printf("Connected to server.\n");

    // Read matrix order
    printf("Enter the order of the matrix: ");
    scanf("%d", &n);

    if (n <= 0 || n > MAX)
    {
        printf("Invalid matrix order.\n");
        close(sock);
        return 1;
    }

    // Read matrix elements
    printf("Enter the matrix elements:\n");

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &matrix[i][j]);
        }
    }

    // Display matrix
    printf("\nEntered Matrix:\n");

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    // Send n
    if (send_all(sock, &n, sizeof(n)) < 0)
    {
        perror("Failed to send n");
        close(sock);
        exit(1);
    }

    // Send complete matrix
    if (send_all(sock, matrix, sizeof(int) * n * n) < 0)
    {
        perror("Failed to send matrix");
        close(sock);
        exit(1);
    }

    // Receive result
    if (recv(sock, result, sizeof(result), 0) <= 0)
    {
        perror("Failed to receive result");
        close(sock);
        exit(1);
    }

    printf("\nMatrix Type: %s\n", result);

    // Close socket
    close(sock);

    return 0;
}
