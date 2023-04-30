#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
    int sockfd, cSockfd;
    int opCount;
    char operator[BUF_SIZE];
    int operand[BUF_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    
    socklen_t len;
    
    if(argc < 2)
    {
        perror("Usage: ./server localPort\n");
        return -1;
    }
    
    if((sockfd=socket(PF_INET, SOCK_STREAM, 0))<0)
    {
        perror("Socket creation failed");
        return -1;
    }
    
    int enable = -1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    //reusable
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));
    servaddr.sin_family = AF_INET;
       
    if((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)))<0)
    {
        perror("Bind failed");
        return -1;
    }
    
    if((listen(sockfd, 5))<0)
    {
        perror("Listen failed");
        return -1;
    }
    
    
    
    while(1)
    {
        if((cSockfd=accept(sockfd, (struct sockaddr *)&cliaddr, &len))<0)
       {
           perror("Accept error");
           return -1;
       }

        //    읽어와서 변수에 저장
        read(cSockfd, &opCount, 1);
        
        char test = (char)opCount;
    
        if(test <= 0)
        {
            printf("Server close(%d)\n",test);
            close(cSockfd);
            break;
        }
        
        printf("Operand count: %d\n", opCount);

        
        for(int i=0; i<opCount; i++)
        {
            read(cSockfd, &operand[i], 4);
            printf("Operand %d: %d\n", i, operand[i]);
        }
        
        for(int i=0; i<(opCount-1); i++)
        {
            read(cSockfd, &operator[i], 1);
            printf("Operator %d: %c\n", i, operator[i]);
        }
        

    //    결과계산
        int result = operand[0];
        for(int i = 0; i < (opCount-1); i++)
        {
            switch (operator[i]) {
                case '+':
                    result += operand[i+1];
                    break;
                case '-':
                    result -= operand[i+1];
                    break;
                case '*':
                    result *= operand[i+1];
                    break;
        }
        }
        
        
    //    결과출력
        printf("Operation result: %d\n", result);
        write(cSockfd, &result, 4);
        
        close(cSockfd);

    
    }


    close(sockfd);
    
    
    
    return 0;
}
