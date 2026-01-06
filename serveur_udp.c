#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>


#define PORT 9600

int main(int argc, char *argv[]){
    int sockfd;
    struct sockaddr_in sockaddr_client;
    struct sockaddr_in sockaddr_serv;
    socklen_t addr_client_size;
    char buf[20];

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    
    sockaddr_serv.sin_family = AF_INET;
    sockaddr_serv.sin_addr.s_addr = INADDR_ANY;
    sockaddr_serv.sin_port = PORT;

    bind(sockfd, (struct sockaddr *)&sockaddr_serv, sizeof(struct sockaddr_in));

    while (1) {
        int n = recvfrom(sockfd, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&sockaddr_client, &addr_client_size);
        write(1, buf, n);
    }

    return 0;
}