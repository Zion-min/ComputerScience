#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUF_SIZE 1024

int main(int argc, char** argv)
{
    struct hostent * host, *host2;
    struct sockaddr_in addr;

    int sockfd;
    int sock_type;
    socklen_t optlen;
    int state;
    struct sockaddr_in servaddr;
    char buf[BUF_SIZE];
    
    
    if(argc == 3)
    {
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket creation failed");
            return -1;
        }
        
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        
        optlen = sizeof(sock_type);
        state = getsockopt(sockfd, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
        if(state)
        {
            perror("getsockopt() error");
            return 1;
        }
        
        printf("This Socket type is: %d(%d) \n", sock_type, SOCK_STREAM);
        
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(atoi(argv[1]));
        servaddr.sin_addr.s_addr = inet_addr(argv[2]);
        
        
        FILE* fp = fopen("copy.txt", "w+");
    // fopen은 w형태로 열고 파일포인터 반환. 근데 이 이름을 가진 파일이 없다면 생성함.
        
        if(connect(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            perror("connect error");
            return -1;
        }
        
        int read_cnt;
        while((read_cnt = read(sockfd, buf, BUF_SIZE))!=0)
            fwrite((void*)buf, 1, read_cnt, fp);
        // fp에 read()한 데이터를 씀. read한 데이터는 위에서 buf에 담겼음.
        
        puts("Received file data");
        
        while(1)
        {
            //fp(파일포인터)에 주소에 가서 1바이트로 BUF_SIZE만큼 읽어서 buf에 저장
            read_cnt = fread((void*)buf, 1, BUF_SIZE, fp);
            //실제로 읽은값 크기만큼 read_cnt에 반환
            //만약 읽은 값 크기가 buf_size(1024)만큼 읽으라 그랬는데 그것보다 작으면 일단 버퍼에 있는 값을 client socket에 쓰고 종료
            if(read_cnt < BUF_SIZE)
            {
                write(sockfd, buf, read_cnt);
                break;
            }
            //그게 아니면 버퍼에 버퍼사이즈(1024바이트)만큼 client에 쓰기.
            //이걸 반복.
            write(sockfd, buf, BUF_SIZE);
        }
        fclose(fp);
        close(sockfd);
    }
    else if(argc == 2)
    {
        //    첫번째 인자로 들어온
        host = gethostbyname(argv[1]);
        if(!host)
        {
            perror("gethostbyname() error");
            return 1;
        }
        
        printf("gethostbyname()\n");
        printf("Official name: %s \n", host->h_name);
        for(int i=0; host->h_aliases[i];i++)
            printf("Aliases %d: %s \n", i, host->h_aliases[i]);
        
        printf("Address type: %s \n", (host->h_addrtype==AF_INET)? "AF_INET" : "AF_INET6");
    //    inet_addr와 반대되는 inet_ntoa. 얘는 IP 주소스럽게 바꾸는 거임
        for(int i=0; host->h_addr_list[i]; i++)
            printf("IP addr %d: %s \n", i, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
        

        memset(&addr, 0, sizeof(addr));
        //이거 머지...
        host2 = gethostbyaddr(host->h_addr_list[0], 4, AF_INET);
        if(!host2){
            perror("gethostbyaddr() error");
            return 1;
        }
        
        printf("\ngethostbyaddr()\n");
        printf("Official name: %s \n", host2->h_name);
        for(int i=0; host2->h_aliases[i];i++)
            printf("Aliases %d: %s \n", i, host2->h_aliases[i]);
        
        printf("Address type: %s \n", (host2->h_addrtype=AF_INET)? "AF_INET": "AF_INET6");
        for(int i=0;host2->h_addr_list[i];i++)
            printf("IP addr %d: %s \n", i, inet_ntoa(*(struct in_addr*)host2->h_addr_list[i]));

    }
    else
    {
        printf("Usage(1): %s <DomainName>\n", argv[0]);
        printf("Usage(2): %s <RemotePort> <RemoteAddress>\n", argv[0]);
        return -1;
    }

    
    
    return 0;
}
