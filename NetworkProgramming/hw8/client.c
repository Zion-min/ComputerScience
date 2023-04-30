#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char**argv)
{
    //브로드캐스팅용 발신, 수신용, TCP connect 소켓 및 길이 연산자 선언
    int recv_sock, send_sock, serv_sock;
    struct sockaddr_in recv_adr, send_adr, serv_adr;
    
    //그 외 서버 변수들
    int so_brd=1;
    
    //연산에 필요한 변수들
    int opCount, opResult;
    char buf[BUF_SIZE]={0,};
    
    //발신용 소켓(8080) 생성
    send_sock=socket(PF_INET, SOCK_DGRAM, 0);
    memset(&send_adr, 0, sizeof(send_adr));
    send_adr.sin_family = AF_INET;
    send_adr.sin_addr.s_addr = inet_addr("255.255.255.255");
    send_adr.sin_port = htons(8080);
    setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void*)&so_brd, sizeof(so_brd));
    
    //표준 출력 후 브로드캐스트 메시지 전송
    printf("Start to find calc server\n");
    sendto(send_sock, "client", strlen("client"), 0, (struct sockaddr*)&send_adr, sizeof(send_adr));
    close(send_sock);

    //수신용 소켓(8082)생성
    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&recv_adr, 0, sizeof(recv_adr));
    recv_adr.sin_family = AF_INET;
    recv_adr.sin_addr.s_addr = htonl(INADDR_ANY); //수신은 아무 주소로 포트만 맞게
    recv_adr.sin_port = htons(8082);
    if(bind(recv_sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr))==-1)
        error_handling("bind() error");
    
    
    //수신대기
    memset(&buf, 0, sizeof(buf));
    recvfrom(recv_sock, buf, BUF_SIZE-1, 0, NULL, 0);
    close(recv_sock);

    //문자열 fail일 경우 
    if(!strcmp(buf, "fail"))
    {
        printf("Fail\n");
        return 0;
    }

    //이외의 경우는 calc서버의 포트번호를 받았을 경우.
    
    //표준출력
    printf("Found calc server(%s)\n",buf);

    //TCP 서버 연결을 위한 소켓 생성
    serv_sock = socket(PF_INET,SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(atoi(buf));
    //버퍼 비우기
    memset(&buf, 0, sizeof(buf));
    
    //연결
    if(connect(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr))<0)
        error_handling("connect error()");
    
    //연산 결과 계산
    printf("Operand count: ");
    scanf("%d", &opCount);
    
    //오버플로우가 나는 operand count를 받은경우 메시지 전송 X 소켓 닫고 프로그램 종료
    char test = (char)opCount;
    if(test<=0)
    {
        close(serv_sock);
        return 0;
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

    write(serv_sock, buf, (opCount*5));
    
//    연산결과 read해오기, opResult에, 4byte만큼 읽어옴
    
    read(serv_sock, &opResult, 4);
    printf("Operation result: %d\n", opResult);
    
    
    //열었던 소켓 닫고 프로그램 종료
    
    close(serv_sock);
    return 0;
}


void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
