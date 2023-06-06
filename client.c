#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>

#define MAX_RETRY 10
#define BASE_INTERVAL 500
#define MAX_WAIT_INTERVAL 8000

void exponentialBackoffSleep(int nRetry) {
    double waitTime = fmin(MAX_WAIT_INTERVAL, BASE_INTERVAL * pow(2, nRetry));
    unsigned int nSleepTime = waitTime * 1000;
    usleep(nSleepTime);
}

int validateIP(const char *ip) {
    struct sockaddr_in sa;
    int iRet = inet_pton(AF_INET, ip, &(sa.sin_addr));
    if (iRet == 0) {
        printf("Invalid IP address. Please enter a valid IP address : ");
        return 0;
    } else if (iRet < 0) {
        perror("Failed to validate IP address. Please enter IP address again : ");
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
	int nPort, nRetry = 0, sockfd;
    char szMsg[2002], szIP[16], szPort[6];

    printf("Please Enter the message sending to server (maximum length 2000) : ");
    fgets(szMsg, sizeof(szMsg), stdin);
    szMsg[strcspn(szMsg, "\n")] = '\0';

    while (1) {
        printf("Please enter the server IP : ");
        fgets(szIP, sizeof(szIP), stdin);
		szIP[strcspn(szIP, "\n")] = '\0';
        if (validateIP(szIP)) break;
    }

	printf("Please enter the port : ");
    while (1) {
        fgets(szPort, sizeof(szPort), stdin);
		szPort[strcspn(szPort, "\n")] = '\0';
		nPort = atoi(szPort);
		if (nPort >= 1 && nPort <= 65535) break;
		printf("Invalid port. Please enter a valid port (1-65535) : ");
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

	// preparing to connect
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(szIP);
    serverAddr.sin_port = htons(nPort);
    
    while (nRetry <= MAX_RETRY) {
        if (sendto(sockfd, szMsg, strlen(szMsg), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("function sendto() failed.");
            exit(EXIT_FAILURE);
        }

        char szBuffer[2001] = {0};
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        ssize_t numBytesReceived = recvfrom(sockfd, szBuffer, sizeof(szBuffer), 0, (struct sockaddr *)&clientAddr, &clientLen);
        if (numBytesReceived >= 0) {
            printf("Response from server: %s\n", szBuffer);
            break;
        } else {
			printf("The server did not respond...\n");
		}

		if (nRetry >= MAX_RETRY) {
			exit(EXIT_FAILURE);
		}
		
		exponentialBackoffSleep(nRetry);
		nRetry++;
    }

    close(sockfd);
    return 0;
}