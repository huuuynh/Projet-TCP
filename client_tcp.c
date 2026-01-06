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
    struct hostent *host;
    char buf[100];
    int realbuf_size;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    host = gethostbyname(argv[1]);
    sockaddr_serv.sin_addr = *(struct in_addr *)host->h_addr;
    sockaddr_serv.sin_family = AF_INET;
    sockaddr_serv.sin_port = PORT;

    connect(sockfd, (struct sockaddr *)&sockaddr_serv, sizeof(struct sockaddr_in));
    while(1){
        realbuf_size = read(sockfd, buf, sizeof(buf));
        write(1, buf, realbuf_size);
        if (buf[realbuf_size-2] = ':'){
            int answer_size = read(0, buf, sizeof(buf)-1);
            if (answer_size > 0){
                write(sockfd, buf, answer_size);
            }
        }
    // sendto(sockfd, buf, realbuf_size, 0, (struct sockaddr *)&sockaddr_serv, sizeof(struct sockaddr_in));
        if (strncmp(buf, "exit\n", 5) == 0) break;
        // printf("%d",realbuf_size);
        // if (realbuf_size > 0) write(sockfd, buf, realbuf_size);
        // else break;
    }
    close(sockfd);
}