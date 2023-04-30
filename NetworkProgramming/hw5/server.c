#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
    int sockfd, cSockfd;
    char buf[BUF_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    
    if(argc < 2)
    {
        printf("Usage: ./server localPort\n");
        return -1;
    }
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        return -1;
    }
    
    int enable = -1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    //reusable
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));
    
    if((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0)
    {
        perror("bind failed");
        return -1;
    }
    
    if((listen(sockfd, 5))<0)
    {
        perror("Listen failed");
        return -1;
    }
    
    if((cSockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len)) < 0)
    {
        perror("Accept error");
        return -1;
    }
    
    FILE* fp = fopen("text.txt", "r");
    // 첫 매개변수의 이름으로 된 파일을 열어서 파일 포인터 반환
    // 두번째 매개변수로 파일을 읽을지 쓸지 결정, r, w등
    
    int read_cnt;
    while(1)
    {
        //fp(파일포인터)에 주소에 가서 1바이트로 BUF_SIZE만큼 읽어서 buf에 저장
        read_cnt = fread((void*)buf, 1, BUF_SIZE, fp);
        //실제로 읽은값 크기만큼 read_cnt에 반환
        //만약 읽은 값 크기가 buf_size(1024)만큼 읽으라 그랬는데 그것보다 작으면 일단 버퍼에 있는 값을 client socket에 쓰고 종료
        if(read_cnt < BUF_SIZE)
        {
            write(cSockfd, buf, read_cnt);
            break;
        }
        //그게 아니면 버퍼에 버퍼사이즈(1024바이트)만큼 client에 쓰기.
        //이걸 반복.
        write(cSockfd, buf, BUF_SIZE);
    }
    
    shutdown(cSockfd, SHUT_WR);
    read(cSockfd, buf, BUF_SIZE);
    printf("Message from Client\n");
    printf("%s", buf);
    
    fclose(fp);
    //파일 닫기
    close(cSockfd);
    //이거는 OS가 잡아둔 소켓 리소스를 해제하는 용도임~!
    close(sockfd);
    
    return 0;
}
