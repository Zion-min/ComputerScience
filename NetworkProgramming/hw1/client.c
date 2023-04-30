#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char** argv)
{
    int sockfd;
    char buf[1024];
    char *student = "2019112635";
    struct sockaddr_in servaddr;
    
    if(argc < 3){
        printf("usage:./client remoteAddress remotePort\n");
        return -1;
    }
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        //0으로 하면 자동으로 들어감
        perror("socket creation failed");
        return -1;
    }
    
    int enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    //Reusable 옵션이라 그래서 바로 전에 사용한거 바로 사용할 수 있음. 4handshake에서 종료될때까지 기다려 주는거.
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[2]);
    //INADDR_ANY: 결국 0.0.0.0 이건 굳이
    servaddr.sin_port = htons(atoi(argv[1]));
    
    if(connect(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect failed");
        return -1;
    }
    
    memset(buf, 0, sizeof(buf));
    printf("%s\n", student);
    write(sockfd, student, strlen(student));
    read(sockfd, buf, sizeof(buf));
    //서버 터미널에 학번 받은거 프린트
    printf("%s\n", buf);
    
    close(sockfd);
    return 0;
}
