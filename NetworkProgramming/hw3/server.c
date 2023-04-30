#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
    int sockfd;
    int opCount;
  
    char buf[BUF_SIZE];
    char operator[BUF_SIZE];
    int operand[BUF_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    
    socklen_t len;
    
    if(argc < 2)
    {
        perror("Usage: ./server localPort\n");
        return -1;
    }
    
    if((sockfd=socket(PF_INET, SOCK_DGRAM, 0))<0)
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
    
    //    읽어와서 변수에 저장


    while(1)
    {
        recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&cliaddr, &len);

        opCount = buf[0];

        char test = (char)opCount;

        if(test <= 0)
        {
            printf("Server close(%d)\n",test);
            break;
        }

        printf("Operand count: %d\n", opCount);
        
        for(int i=0; i<opCount; i++)
        {
            operand[i] = (int)buf[(i*4)+1];
            printf("Operand %d: %d\n", i, operand[i]);
        }
        
        
        for(int i=0; i<(opCount-1); i++)
        {
            operator[i] = (char)buf[i+(opCount*4)+1];
            printf("Operator %d: %c\n", i, operator[i]);
        }

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
        
        sendto(sockfd, &result, 4, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
        printf("Operation result: %d\n", result);

    }
       
    close(sockfd);
    
    
    
    return 0;
}
