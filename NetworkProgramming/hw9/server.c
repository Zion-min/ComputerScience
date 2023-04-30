#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/uio.h>

#define BUF_SIZE 1024
#define MAX_CLNT 256

//critical section
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

void* handle_clnt(void* arg);
void send_msg(char *msg, int len);
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    socklen_t clnt_adr_sz;
    struct sockaddr_in serv_adr, clnt_adr;
    pthread_t t_id;
    
    // 1. Main 함수 인자로부터 포트번호를 받고 이를 활용하여 서버 소켓을 생성
    if(argc !=2)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }
    
    pthread_mutex_init(&mutx, NULL);
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    
    int enable = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    
    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error");
    if(listen(serv_sock, MAX_CLNT)==-1)
        error_handling("listen() error");
    
    while(1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

        // 1. 클라이언트의 요청이 오면 Mutex를 이용하여 클라이언트 소켓 fd(File Descriptor)를 관리하는 배열에 스레드가 접근하지 못하도록 하고, 전체 소켓 fd를 관리하는 배열의 count 수를 늘려서 저장함
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        //2. 스레드를 생성하고 생성된 클라이언트 소켓 fd를 인자로 넘겨줌
        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
        //3. 스레드를 detach하고 “Connected client Port: 클라이언트포트번호”를 표준 출력한 뒤, 다음 연결 요청을 대기함
        pthread_detach(t_id);
        printf("Connected client Port: %d \n", clnt_adr.sin_port);
    }
    close(serv_sock);
    
    return 0;
}

void* handle_clnt(void* arg)
{
    int clnt_sock=*((int*)arg);
    int str_len = 0, i;
    char msg[BUF_SIZE];
    
    int opCount;
    int operand[BUF_SIZE];
    int operator[BUF_SIZE];
    
    struct iovec vec[2];
    char name[10]={0,};
    char calcdata[BUF_SIZE]={0,};
    // 1. 클라이언트는 writev()를 통해 첫 번째 배열에는 4글짜의 ID 값을 주고, 두 번째 배열에는 Network Assignment #2 과제와 동일한 계산 데이터를 줌
    vec[0].iov_base = name;
    vec[0].iov_len = 4;
    // calcdata라는 배열에 계산결과 저장됨
    vec[1].iov_base = calcdata;
    vec[1].iov_len = BUF_SIZE;
    
    
    // 1. 메인 스레드로부터 받은 클라이언트 소켓 fd를 활용하여 해당 클라이언트의 계산 요청을 readv()로 대기함
    while((str_len=readv(clnt_sock, vec, 2))!=0)
    {
        // 2. readv()에서 계산 요청을 받으면 계산을 해서 결과 값과 함께 아래와 같은 문자열 형식을 만듦 [id] 계산식: 계산결과
        memset(msg, 0, sizeof(msg));
        opCount = calcdata[0];
        
        for(int j=0; j<opCount; j++)
        {
            operand[j] = ((calcdata[1+((j*4)+3)] << 24)&0xff000000) | ((calcdata[1+((j*4)+2)] << 16)&0x00ff0000) | ((calcdata[1+((j*4)+1)] << 8)&0x0000ff00) | (calcdata[1+(j*4)]&0x000000ff);
    //                                    나중에 프린트 지워야 함. 확인용도임 다른것도 마찬가지!
        //    printf("Operand %d: %d\n", j, operand[j]);
        }
        
        for(int j=0; j<(opCount-1); j++)
        {
            operator[j] = (char)calcdata[j+(opCount*4)+1];
        //    printf("Operator %d: %c\n", j, operator[j]);
        }
        
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
        
        //printf("result: %d", result);
        
        char buf[100]={0,};
       // printf("id: %s", name);
        sprintf(buf, "[%s] ", name);
        strcat(msg, buf);
        for(int i=0; i<opCount; i++)
        {
            sprintf(buf, "%d", operand[i]);
            strcat(msg, buf);
            if(i < (opCount-1))
            {
                sprintf(buf, "%c", operator[i]);
                strcat(msg, buf);
            }
        }
        sprintf(buf, "=%d\n",result);
        strcat(msg, buf);
        
        //  3. 이 문자열을 현재 연결되어 있는 모든 클라이언트에게 write()하고 다음 요청을 대기함
        send_msg(msg, strlen(msg));
    }
    //  4. 이 과정에서 Mutex를 활용하여 공유 자원을 참조할 때 의도치 않은 상황이 일어나는 것을 배제함
    pthread_mutex_lock(&mutx);
    for (i = 0; i < clnt_cnt; i++)
    {
      if (clnt_sock == clnt_socks[i])
      {
          while (i++ < clnt_cnt - 1)
              clnt_socks[i] = clnt_socks[i + 1];
          break;
      }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
  //  5. 클라이언트가 연결을 종료한다면 Closed client와 함께 연결을 종료함 – detach를 통해 스레드 자원은 반납됨
    printf("Closed client\n");
    return NULL;
          
    
}

void send_msg(char *msg, int len)
{
    int i;
    pthread_mutex_lock(&mutx);
    for (i = 0; i < clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


