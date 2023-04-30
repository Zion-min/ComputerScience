#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/uio.h>

#define BUF_SIZE 1024

int main(int argc, char** argv)
{
    int sockfd;
    int opCount, opResult, i;
    char buf[BUF_SIZE];
    char mode[5];
    char id[5];
    struct iovec vec[3];
//    전송된 바이트 수가 반환 될 .. 변수
    int str_len;
    
    struct sockaddr_in serv_addr;
    
    if( argc < 3)
    {
        printf("usage: ./client remotePort remoteAddress\n");
        return -1;
    }
    
    if((sockfd=socket(PF_INET, SOCK_STREAM, 0))<0)
    {
        perror("socket creation failed");
    }
    
    int enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[2]);
    serv_addr.sin_port = htons(atoi(argv[1]));
    
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        perror("connect failed");
        return -1;
    }
    
    
    printf("Mode: ");
    scanf("%s", mode);
    
    // 1이어야 됨. save도 아니고, load도 아니고, quit도 아니면 -> 1. save가 아니니까 1반환
    if( strcmp(mode,"save") && strcmp(mode,"load") && strcmp(mode, "quit"))
    {
        printf("supported mode: save load quit\n");
        exit(1);
    }
        
    if( !strcmp(mode,"save") || !strcmp(mode,"load"))
    {
        printf("ID: ");
        scanf("%s", id);
        if(strlen(id)!=4)
        {
            printf("ID length must be 4\n");
            exit(1);
        }
    }
    
    
    if(!strcmp(mode,"save"))
    {
        vec[0].iov_base=mode;
        vec[0].iov_len=4;
        vec[1].iov_base=id;
        vec[1].iov_len=4;

        printf("Operand count: ");
        scanf("%d", &opCount);
        
        char test = (char)opCount;
        
        if(test<=0)
        {
            printf("Overflow will happen(%d)\n", test);
            exit(1);
        }
        
        buf[0] = (char)opCount;
        
        for(i=0; i<opCount; i++)
        {
            printf("Operand %d: ", i);
            scanf("%d", (int*)&buf[(i*4)+1]);
        }
        
        for(i=0; i<(opCount-1); i++)
        {
            printf("Operator %d: ", i);
            scanf(" %c", &buf[i+(opCount*4)+1]);
        }
        
        vec[2].iov_base= buf;
        vec[2].iov_len = opCount*5;

        str_len=writev(sockfd, vec, 3);
//        printf("succes? %d\n",str_len);
        //puts(""); //vec에 담긴 데이터가 표시된다. ABC, 1234가 표시된다.
        //printf("Write bytes: %d - %d\n", str_len, opCount*5+8);
        read(sockfd, &opResult, 4);
        printf("Operation result: %d\n", opResult);

        
        close(sockfd);
        
    }

    
    
    if(!strcmp(mode,"load"))
    {
        vec[0].iov_base=mode;
        vec[0].iov_len=4;
        vec[1].iov_base=id;
        vec[1].iov_len=4;
        writev(sockfd, vec, 3);
//        printf("load success? %d\n",str_len);
//        뭔가 계산결과 말고 계산결과 정보들을 문자열로 보내주면 이걸 read로 받아 출력하고 소켓닫고 종료
        str_len = read(sockfd, buf, BUF_SIZE);
        printf("%s",buf);
        
        close(sockfd);
        
    }
    
    if(!strcmp(mode,"quit"))
    {
        vec[0].iov_base=mode;
        vec[0].iov_len=4;
        str_len=writev(sockfd, vec, 3);
        close(sockfd);
    }
    

    
    return 0;
}
