#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#define PORT        8888
#define IPV4_ADDR   "172.17.0.2"

void *clientHandler(void *arg) {
    int sockfd = *(int *)arg;

    while (1) {
        char szBuffer[2001] = {0};
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        ssize_t nReceived = recvfrom(sockfd, szBuffer, sizeof(szBuffer), 0, (struct sockaddr *)&clientAddr, &clientLen);
        if (0 > nReceived) {
            perror("recvfrom failed");
            close(sockfd);
            pthread_exit(NULL);
        }

        ssize_t nSent = sendto(sockfd, szBuffer, nReceived, 0, (struct sockaddr *)&clientAddr, clientLen);
        if (0 > nSent) {
            perror("sendto failed");
            close(sockfd);
            pthread_exit(NULL);
        }
    }

    close(sockfd);
    pthread_exit(NULL);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > sockfd) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(IPV4_ADDR);
    serverAddr.sin_port = htons(PORT);

    if (0 > bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    pthread_t threads[10];
    int i, rc;

    for (i = 0; i < 10; i++) {
        rc = pthread_create(&threads[i], NULL, clientHandler, (void *)&sockfd);
        if (rc) {
            perror("Failed to create thread");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    close(sockfd);
    return 0;
}