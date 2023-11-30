#include <netinet/in.h>
#include <stdio.h>

#define BUFF_SIZE 4096
#define PORT 80

typedef struct sockaddr_in sockaddr_in;

int init_socket();
char* read_file(char* file_name);
char* read_stream(const int, const char*);
sockaddr_in* form_addr(const char* HOST);
