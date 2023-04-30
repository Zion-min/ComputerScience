#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int flag = 0;

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    
    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int state;
    int fds[2];
    
    
    if(argc!=2){
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    state=sigaction(SIGCHLD, &act, 0);
    
    
    if((serv_sock=socket(PF_INET, SOCK_STREAM, 0))<0)
    {
        perror("Socket creation failed");
        return -1;
    }
    
    int enable = -1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    //reusable
    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    serv_adr.sin_family = AF_INET;
       
    if((bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)))<0)
    {
        perror("Bind failed");
        return -1;
    }
    
    if((listen(serv_sock, 5))<0)
    {
        perror("Listen failed");
        return -1;
    }
 
    pipe(fds);
    pid = fork();
    
    if(pid==0)
    {
        FILE* fp = fopen("log.txt", "wt");
        char msgbuf[BUF_SIZE];
        int len;
        
        while(1)
        {
//            msgbuf에 계산결과 양식에 맞게 써줘야 함 (process id): 3+4-2=5
//            종료조건
            len=read(fds[0], msgbuf, BUF_SIZE);
            if(len>1)
            {
                fwrite((void*)msgbuf, 1, len, fp);
            }
            else
            {
                printf("종료\n");
                break;
            }
                
        }
        fclose(fp);
        return 0;
    }
    while(1)
    {
        adr_sz=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
        if(clnt_sock==-1)
            continue;
        else
            puts("new client connected...");
        
        pid=fork();
        if(pid==0)
        {
            int opCount=0;
            char operator[BUF_SIZE];
            int operand[BUF_SIZE];
            char buf[BUF_SIZE];
            
            close(serv_sock);
 
            read(clnt_sock, &opCount, 1);
            
            char test = (char)opCount;
            
            if(test <=0)
            {
                char sig = 'S';
                write(fds[1], &sig, 1);
                printf("Save file(%d)\n",test);
                close(clnt_sock);
                return 0;
            }
            
            printf("Operand count: %d\n", opCount);

            
            for(int i=0; i<opCount; i++)
            {
                read(clnt_sock, &operand[i], 4);
                printf("Operand %d: %d\n", i, operand[i]);
            }
            
            for(int i=0; i<(opCount-1); i++)
            {
                read(clnt_sock, &operator[i], 1);
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
            
//            클라이언트에게 결과 보내기
            write(clnt_sock, &result, 4);
            
            
//            표준출력하기
            printf("Operation result: %d\n", result);
            
            
//            결과 버퍼에 쓰기
            char temp[100];
            sprintf(buf, "%d: ", getpid());
            strcat(temp, buf);
            for(int i=0; i<opCount; i++)
            {
                sprintf(buf, "%d", operand[i]);
                strcat(temp, buf);
                if(i < (opCount-1))
                {
                    sprintf(buf, "%c", operator[i]);
                    strcat(temp, buf);
                }
                    
                
            }
            sprintf(buf, "=%d\n",result);
            strcat(temp, buf);
            printf("%s",temp);
            
            
            
            write(fds[1], temp, strlen(temp));

            
            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        }
        else
            close(clnt_sock);
    }
    
    close(serv_sock);
    
    
    return 0;
}
    
