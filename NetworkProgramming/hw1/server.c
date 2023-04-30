#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char** argv)
{
    int sockfd, cSockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buf[1024];
    socklen_t len;
    
    if(argc < 2){
        printf("usage:./server localPort\n");
        return -1;
    }
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        //0으로 하면 자동으로 들어감
        perror("socket creation failed");
        return -1;
    }
    
    //internet을 사용화기 위한 구조체.
    //첫번째 if, PORT안 받았다는 의미
    
    int enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    //Reusable 옵션이라 그래서 바로 전에 사용한거 바로 사용할 수 있음. 4handshake에서 종료될때까지 기다려 주는거.
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //INADDR_ANY: 결국 0.0.0.0 이건 굳이
    servaddr.sin_port = htons(atoi(argv[1]));
    
    if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        return -1;
    }
    
    if(listen(sockfd, 5) < 0){
        perror("socket failed");
        return -1;
    }
    
    if((cSockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
        //위에서 internet 버전으로 선언. 그래서 타입변환해야 함.
        perror("accept error");
        return -1;
    }
    
    memset(buf, 0, sizeof(buf));
    read(cSockfd, buf, sizeof(buf));
    //서버 터미널에 학번 받은거 프린트
    printf("%s\n", buf);
    
    //
    strcat(buf, "_민시온");
    write(cSockfd, buf, strlen(buf));
    
    close(cSockfd);
    close(sockfd);

        
}
