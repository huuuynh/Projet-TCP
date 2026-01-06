#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netdb.h>
#include <string.h>


#define SIZE 100
#define PORT 9600
#define h_addr h_addr_list[0]

int main(int argc, char *argv[]){
    int sockfd;
    struct sockaddr_in sockaddr_serv;
    struct sockaddr_in sockaddr_fd;
    struct hostent *host;
    socklen_t addr_serv_size = sizeof(struct sockaddr_in);
    char buf[20];
    int buf_size = 0;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    host = gethostbyname(argv[1]);
    sockaddr_serv.sin_addr = *(struct in_addr *)host->h_addr;

    sockaddr_fd.sin_family = AF_INET;
    sockaddr_fd.sin_addr.s_addr = INADDR_ANY;
    sockaddr_fd.sin_port = 0;

    sockaddr_serv.sin_family = AF_INET;
    sockaddr_serv.sin_port = PORT;

    bind(sockfd, (struct sockaddr *)&sockaddr_fd, sizeof(struct sockaddr_in));
    
    int buf_size = read(0, buf, sizeof(buf));
    
    sendto(sockfd, buf, buf_size, 0, (struct sockaddr *)&sockaddr_serv, addr_serv_size);
    close(sockfd);
}