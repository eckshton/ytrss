#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "io.h"

sockaddr_in* form_addr(const char* HOST) {
    sockaddr_in* addr = malloc(sizeof(sockaddr_in));
    struct hostent* hp;

    if((hp = gethostbyname(HOST)) == NULL) {
        exit(1);
    }

    memcpy(&addr->sin_addr, hp->h_addr, hp->h_length);
    addr->sin_port = htons(PORT);
    addr->sin_family = AF_INET;
    
    return addr;
}
int init_socket() {
    int sockfd;

    sockfd = socket(
        AF_INET, // ipv4
        SOCK_STREAM, // i dont understand the others...
        0 // irrelevant
    );

    return sockfd;
}
char* read_file(char* file_name) {
    FILE* file = fopen(file_name, "r");
    char* res = malloc(1);
    char buffer[BUFF_SIZE];
    bzero(buffer, BUFF_SIZE);
    res[0] = '\0';

    while(fgets(buffer, BUFF_SIZE - 1, file) != NULL) {
        res = realloc(res, strlen(res) + strlen(buffer) + 1);
        strcat(res, buffer);
        bzero(buffer, BUFF_SIZE);
    }

    return res;
}
char* read_stream(const int fd, const char* request) {
    char* res = malloc(1);
    char buffer[BUFF_SIZE];
    bzero(buffer, BUFF_SIZE);
    res[0] = '\0';

    send(fd, request, strlen(request), 0);
    while(recv(fd, buffer, BUFF_SIZE - 1, 0) > 0) {
        res = realloc(res, strlen(res) + strlen(buffer) + 1);
        strcat(res, buffer);
        bzero(buffer, BUFF_SIZE);
    }

    return res;
}
