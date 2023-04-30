#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/uio.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char *message);

char name[NAME_SIZE]="[DEFAULT]";

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void* thread_return;
    // 1. Main 함수 인자로부터 포트번호, IP, ID(4글짜) 순으로 데이터를 받음
    if(argc!=4)
    {
        printf("Usage: %s <IP> <port> <name> \n", argv[0]);
        exit(1);
    }
    
    // 2. ID가 4글짜가 아니라면 “ID have to be 4” 메시지를 표준 출력하고 종료
    if(strlen(argv[3])!=4)
    {
        printf("ID have to be 4\n");
        return 0;
    }
    
    sprintf(name, "%s", argv[3]);
    
    //3. 소켓을 열어 서버와 연결하고 멀티스레드 형식으로 Send 및 Receive를 하는 스레드들을 생성하여 서버와의 통신을 진행
    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error");
    
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    
    return 0;
}

void* send_msg(void* arg)
{
    int sock = *((int*)arg);
    char buf[BUF_SIZE];
    char* tok[BUF_SIZE];
    char name_msg[NAME_SIZE+BUF_SIZE];
    struct iovec vec[3];
    
    while(1)
    {
        int opCount;
        int i=0;
        
        memset(&buf, 0, sizeof(buf));
        memset(&tok, 0, sizeof(tok));
        memset(&name_msg, 0, sizeof(name_msg));
        
//        파일디스크립터 stdin으로, BUF_SIZE만큼 입력받아 버퍼에 저장
// 1. 키보드를 통해 띄어쓰기를 기준으로 아래와 같이 계산 요청 데이터를 구성함 3 1 2 3 + +
        fgets(buf, BUF_SIZE, stdin);
        
//        overflow
        char *ptr = strtok(buf, " ");      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환

        while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
        {
            tok[i++]=ptr;
            ptr = strtok(NULL, " ");
        }
               
        opCount = atoi(tok[0]);
        char test = (char)opCount;
        if(test<=0)
        {
            printf("Overflow Number(%d) - Closed client\n", test);
            exit(1);
        }
        
// 2. 이때 바이트 수 역시 Network assignment #2와 동일하게 구성해야 됨 (Ex. Operand count는 1바이트)
        name_msg[0]= (char)opCount;
        
        for(int j=0; j<opCount; j++)
        {
            int num = atoi(tok[j+1]);
            name_msg[(j*4)+1] = ((num << 24)&0xff000000) | ((num << 16)&0x00ff0000) | ((num << 8)&0x0000ff00) | (num&0x000000ff);
        }
        
        for(int j=0; j<(opCount-1); j++)
        {
            name_msg[j+(opCount*4)+1] = tok[j+opCount+1][0];
        }
        
        // 3. iovec구조체를 활용하여, 첫 번째 배열에는 main 함수 인자로 얻은 ID를 입력 받고, 두 번째 배열에 계산 요청 데이터를 입력 받아서 writev()를 통해 데이터를 전송함
        vec[0].iov_base = name;
        vec[0].iov_len = 4;
        vec[1].iov_base= name_msg;
        vec[1].iov_len=opCount*5;
        
        int len = writev(sock, vec, 2);
       // printf("send len: %d\n",len);
        // 4. 전송 후에는 다시 키보드를 통해 계산 데이터를 받는 것을 대기함
    
    }

    return NULL;
   
}

void* recv_msg(void* arg)
{
    int sock = *((int *)arg);
    char name_msg[NAME_SIZE+BUF_SIZE];
    int str_len;
    while (1)
    {
        // 1. 서버로부터의 데이터를 수신하기 위해 read()를 하여 대기함
        str_len = read(sock, name_msg,  NAME_SIZE+BUF_SIZE-1);
        if (str_len == -1)
            return (void *)-1;
        name_msg[str_len] = 0;
        // 2. 데이터를 수신하면 이를 화면 그대로 출력해주고 다시 대기함
        fputs(name_msg, stdout);
    }
    return NULL;
}


void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

