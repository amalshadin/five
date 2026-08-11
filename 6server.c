#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000
#define SIZE 4096

void translate(char *sentence)
{
    char result[SIZE] = "";
    char *word;

    word = strtok(sentence, " ");

    while (word != NULL)
    {
        if (strcmp(word, "tbh") == 0)
            strcat(result, "to be honest");

        else if (strcmp(word, "ig") == 0)
            strcat(result, "I guess");

        else if (strcmp(word, "tbf") == 0)
            strcat(result, "to be fair");

        else if (strcmp(word, "atm") == 0)
            strcat(result, "at the moment");

        else if (strcmp(word, "irl") == 0)
            strcat(result, "in real life");

        else if (strcmp(word, "lol") == 0)
            strcat(result, "laughing out loud");

        else if (strcmp(word, "asap") == 0)
            strcat(result, "as soon as possible");

        else if (strcmp(word, "omg") == 0)
            strcat(result, "oh my god");

        else if (strcmp(word, "ttyl") == 0)
            strcat(result, "talk to you later");

        else if (strcmp(word, "idk") == 0)
            strcat(result, "I don't know");

        else if (strcmp(word, "nvm") == 0)
            strcat(result, "never mind");

        else
            strcat(result, word);

        strcat(result, " ");

        word = strtok(NULL, " ");
    }

    strcpy(sentence, result);
}

int main()
{
    int sockfd;
    char buffer[SIZE];

    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (struct sockaddr *)&server,
             sizeof(server)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        return 1;
    }

    printf("UDP Server running on port %d...\n", PORT);

    while (1)
    {
        memset(buffer, 0, SIZE);

        // Receive sentence
        recvfrom(sockfd, buffer, SIZE, 0,
                 (struct sockaddr *)&client,
                 &client_len);

        printf("Received: %s\n", buffer);

        // Translate
        translate(buffer);

        printf("Translated: %s\n", buffer);

        // Send translated sentence
        sendto(sockfd, buffer, strlen(buffer) + 1, 0,
               (struct sockaddr *)&client,
               client_len);
    }

    close(sockfd);

    return 0;
}
