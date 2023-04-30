#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char** argv)
{
    int sockfd;
    int opCount, opResult;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr;
    // receive 할 때는 길이 저장할 socket len 저장할 곳이 있어야 함.
    socklen_t len;
    
    if( argc < 3)
    {
        printf("usage: ./client remotePort remoteAddress\n");
        return -1;
    }
    
    if((sockfd=socket(PF_INET, SOCK_DGRAM, 0))<0)
    {
        perror("socket creation failed");
    }
    
    int enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[2]);
    serv_addr.sin_port = htons(atoi(argv[1]));

    
    printf("Operand count: ");
    scanf("%d", &opCount);
    
    char test = (char)opCount;
    
    if(test<=0)
    {
        sendto(sockfd, &opCount, 4, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        close(sockfd);
        exit(1);
    }
    
    buf[0] = (char)opCount;

    
    for(int i=0; i<opCount; i++)
    {
        printf("Operand %d: ", i);
        scanf("%d", (int*)&buf[(i*4)+1]);
    }
    
    
    for(int i=0; i<(opCount-1); i++)
    {
        printf("Operator %d: ", i);
        scanf(" %c", &buf[i+(opCount*4)+1]);
    }
    
    
    sendto(sockfd, buf, (opCount*5), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
//    연산결과 read해오기, opResult에, 4byte만큼 읽어옴
    
    recvfrom(sockfd, &opResult, 4, 0, (struct sockaddr*)&serv_addr, &len);
    printf("Operation result: %d\n", opResult);

    
    close(sockfd);
    
    
    return 0;
}
