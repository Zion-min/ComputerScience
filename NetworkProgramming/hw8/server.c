#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>

#define BUF_SIZE 1024

char * strcpy_slice(char *buf, char *s, int start, int size);
void error_handling(char *message);

int main(int argc, char* argv[])
{
// discovery server socket: 수신용, 발신용(2개, cal server, client) 브로드캐스팅 소켓
    int recv_sock, calc_sock, clnt_sock;
    struct sockaddr_in recv_adr, calc_adr, clnt_adr;
    
    // 수신용 버퍼 길이 변수
    int recv_len, clnt_len;
// calculator server socket: 수신용(recv_sock), 발신용(dis server) 브로드캐스팅 소켓(dicv_sock), 멀티플렉싱용 TCP서버 소켓(calc_sock), 클라이언트 소켓(clnt_sock)
    int dicv_sock, serv_sock;
    struct sockaddr_in dicv_adr, serv_adr;
    
//    버퍼 초기화
    char buf[BUF_SIZE]={0,};
    char calc_port[6]={0,};
    char success[] ="success";
    char fail[]="fail";
    
//    인자 확인
    if(argc!=2 || (strcmp(argv[1], "discovery") && strcmp(argv[1], "calc")))
    {
        printf("./server discovery | ./server calc\n");
        exit(1);
    }
    
//    discovery 서버 실행
    if(!strcmp(argv[1], "discovery"))
    {
//        calculator port
        char calc_port[6]={0,};
        
//        수신용 브로드캐스트 소켓(8080) 생성
        recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
        memset(&recv_adr, 0, sizeof(recv_adr));
        recv_adr.sin_family = AF_INET;
        recv_adr.sin_addr.s_addr = htonl(INADDR_ANY); //수신은 아무 주소로 포트만 맞게
        recv_adr.sin_port = htons(8080);
        if(bind(recv_sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr))==-1)
            error_handling("bind() error");
        
        



        printf("Discovery Server operating...\n");
        
        while(1)
        {
            memset(&buf, 0, sizeof(buf));
            recv_len = recvfrom(recv_sock, buf, BUF_SIZE-1, 0, NULL, 0);
            if(recv_len <0) break;
            //printf("%s\n", buf);

            //서버일 경우
            if(!strncmp(buf, "server", 6))
            {   
                //발신용 소켓(8081) 생성
                int so_brd1 = 1; //브로드캐스트 옵션 길이용 변수
                calc_sock=socket(PF_INET, SOCK_DGRAM, 0);
                memset(&calc_adr, 0, sizeof(calc_adr));
                calc_adr.sin_family = AF_INET;
                calc_adr.sin_addr.s_addr = inet_addr("255.255.255.255");
                calc_adr.sin_port = htons(8081);
                setsockopt(calc_sock, SOL_SOCKET, SO_BROADCAST, (void*)&so_brd1, sizeof(so_brd1));
                //calc 서버가 등록되지 않은 경우 -> port안에 NULL값이 존재
                if(!strcmp(calc_port, ""))
                {
                    strcpy_slice(calc_port, buf, 7, 5);
                    printf("Calc Server registered(%s)\n", calc_port);
                    int len = sendto(calc_sock, success, strlen(success), 0, (struct sockaddr*)&calc_adr, sizeof(calc_adr));
                    printf("send? %d\n",len);
                }
                //calc 서버가 등록되어 있는 경우 -> 기존의 저장된 port와 임시포트가 동일한 경우
                else
                {
                    sleep(1);
                    int len = sendto(calc_sock, fail, strlen(fail), 0, (struct sockaddr*)&calc_adr, sizeof(calc_adr));
                    printf("send? %d\n",len);
                }
                //close(calc_sock);
                
            }
            //클라이언트일 경우
            if(!strncmp(buf, "client", 6))
            {
                //발신용 소켓(8082) 생성
                int so_brd2 = 1; //브로드캐스트 옵션 길이용 변수
                clnt_sock=socket(PF_INET, SOCK_DGRAM, 0);
                memset(&clnt_adr, 0, sizeof(clnt_adr));
                clnt_adr.sin_family = AF_INET;
                clnt_adr.sin_addr.s_addr = inet_addr("255.255.255.255");
                clnt_adr.sin_port = htons(8082);
                setsockopt(clnt_sock, SOL_SOCKET, SO_BROADCAST, (void*)&so_brd2, sizeof(so_brd2));

                //calc 서버가 등록되지 않은 경우
                if(!strcmp(calc_port, ""))
                {
                    sendto(clnt_sock, fail, strlen(fail), 0, (struct sockaddr*)&clnt_adr, sizeof(clnt_adr));
                }
                //calc 서버가 등록되어 있는 경우
                else
                {
                    sleep(2);
                    sendto(clnt_sock, calc_port, strlen(calc_port), 0, (struct sockaddr*)&clnt_adr, sizeof(clnt_adr));
                }
                //close(clnt_sock);
            }
            //아무것도 아니면 혹은 서버, 클라이언트 분기가 끝나면 반복해주어야 하는 작업
        }
//        완전히 반복문이 끝났을 경우, 소켓 종료 및 프로그램 종료
        close(calc_sock);
        close(clnt_sock);
        close(recv_sock);
        return 0;
    
    }
    
//    calculator 서버 실행
    if(!strcmp(argv[1], "calc"))
    {
        //멀티캐스팅
        struct timeval timeout;
        fd_set reads, cpy_reads;
        int fd_max, str_len, fd_num, i;
        
        //포트번호 생성 및 표준 출력
        srand((unsigned int)time(NULL));
        int port = rand()%(50000-10000+1)+10000;
        printf("Register calc server\n");

        //발신용 소켓(8080) 생성
        int so_brd = 1; //브로드캐스트 옵션 길이용 변수
        dicv_sock=socket(PF_INET, SOCK_DGRAM, 0);
        memset(&dicv_adr, 0, sizeof(dicv_adr));
        dicv_adr.sin_family = AF_INET;
        dicv_adr.sin_addr.s_addr = inet_addr("255.255.255.255");
        dicv_adr.sin_port = htons(8080);
        setsockopt(dicv_sock, SOL_SOCKET, SO_BROADCAST, (void*)&so_brd, sizeof(so_brd));
        
        //브로드캐스트 메시지 전송
        char tmp[12];
        sprintf(tmp, "server:%d", port);
        //printf("calc: %s\n", tmp);
        sendto(dicv_sock, tmp, strlen(tmp), 0, (struct sockaddr*)&dicv_adr, sizeof(dicv_adr));
        //printf("%d char\n",len);
        
        //수신용 소켓(8081) 생성
        recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
        memset(&recv_adr, 0, sizeof(recv_adr));
        recv_adr.sin_family = AF_INET;
        recv_adr.sin_addr.s_addr = htonl(INADDR_ANY); //수신은 아무 주소로 포트만 맞게
        recv_adr.sin_port = htons(8081);
        if(bind(recv_sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr))==-1)
            error_handling("bind() error");
        
        //수신대기
        memset(&buf, 0, sizeof(buf));
        int len = recvfrom(recv_sock, buf, BUF_SIZE-1, 0, NULL, 0);
        printf("recv? %d %s\n",len,buf);
        
        //문자열 fail일 경우 소켓 닫고 프로그램 종료
        if(!strcmp(buf, "fail"))
        {
            printf("Fail\n");
            close(dicv_sock);
            close(recv_sock);
            return 0;
        }
        
        //문자열 success일 경우 표준출력 후 브로드캐스팅용 소켓 닫기
        if(!strcmp(buf, "success"))
        {
            printf("Calc Server(%d) operating...\n",port);
            close(dicv_sock);
            close(recv_sock);
        }
        
        
        //지정된 포트로 TCP 서버 리스닝 소켓 생성
        serv_sock = socket(PF_INET, SOCK_STREAM, 0);
        memset(&serv_adr, 0, sizeof(serv_adr));
        serv_adr.sin_family=AF_INET;
        serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
        serv_adr.sin_port=htons(port);
        if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
            error_handling("bind() error");
        if(listen(serv_sock, 5)==-1)
            error_handling("listen() error");
        
        //fd_set 초기화 작업
        FD_ZERO(&reads);
        FD_SET(serv_sock, &reads);
        fd_max = serv_sock;
        
        //멀티플렉싱 서버 구동
        while(1)
        {
            cpy_reads = reads;
            timeout.tv_sec = 5;
            
            fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout);
            if(fd_num == 0) continue;
            
            for(i=0; i<fd_max+1; i++)
            {
                if(FD_ISSET(i, &cpy_reads))
                {
                    if(i==serv_sock)
                    {
                        clnt_len = sizeof(clnt_adr);
                        clnt_sock = accept(serv_sock, (struct sockaddr*)&serv_adr, &clnt_len);
                        
                        FD_SET(clnt_sock, &reads);
                        
                        if(fd_max<clnt_sock)
                            fd_max=clnt_sock;
                        printf("connected client: %d \n", clnt_sock);
                    }
                    else
                    {
                        int opCount;
                        char operator[BUF_SIZE];
                        int operand[BUF_SIZE];
                        int str_len;
                        
                        str_len = read(i, &opCount, 1);
                        
                        if(str_len==0)
                        {
                            FD_CLR(i, &reads);
                            close(i);
                            printf("closed client: %d \n", i);
                        }
                        else
                        {
                            //printf("Operand count: %d\n", opCount);

                            
                            for(int j=0; j<opCount; j++)
                            {
                                read(i, &operand[j], 4);
                                //printf("Operand %d: %d\n", j, operand[j]);
                            }
                            
                            for(int j=0; j<(opCount-1); j++)
                            {
                                read(i, &operator[j], 1);
                                //printf("Operator %d: %c\n", j, operator[j]);
                            }
                            

                        //    결과계산
                            int result = operand[0];
                            for(int j = 0; j < (opCount-1); j++)
                            {
                                switch (operator[j]) {
                                    case '+':
                                        result += operand[j+1];
                                        break;
                                    case '-':
                                        result -= operand[j+1];
                                        break;
                                    case '*':
                                        result *= operand[j+1];
                                        break;
                                }
                            }
                            
                        //    결과출력
                            //printf("Operation result: %d\n", result);
                            write(i, &result, 4);

                        }
                    }
                }
            }
        }
        close(serv_sock);
    }
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

char * strcpy_slice(char *buf, char *s, int start, int size)
{
  if (strlen(s) > start) {
    s += start;   // 시작 위치로 변경
    while (size-- > 0 && *s != '\0')   // size가 0보다 크고, 문자열 끝이 지나지 않은 경우
      *(buf++) = *(s++);   // 복사
    *buf = '\0';    // 끝에 널 문자 처리
  }

  return buf;
}