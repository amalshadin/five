#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9002

int main()
{
    int sock;
    struct sockaddr_in server_addr;

    int n;
    int matrix[100][100];

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

    if (n <= 0 || n > 100)
    {
        printf("Invalid matrix order.\n");
        close(sock);
        return 1;
    }

    // Seed random number generator
    srand(time(NULL));

    // Generate matrix
    printf("\nGenerated Matrix:\n");

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matrix[i][j] = (rand() % 50) + 1;
            printf("%4d ", matrix[i][j]);
        }
        printf("\n");
    }

    // Send matrix order
    if (send(sock, &n, sizeof(n), 0) < 0)
    {
        perror("Failed to send n");
        close(sock);
        exit(1);
    }

    // Send matrix
    if (send(sock, matrix, sizeof(int) * n * n, 0) < 0)
    {
        perror("Failed to send matrix");
        close(sock);
        exit(1);
    }

    // Receive matrix type
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
