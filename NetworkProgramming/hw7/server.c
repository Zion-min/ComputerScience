#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/uio.h>

#define BUF_SIZE 1024

int flag = 0;

typedef struct calc
{
    char id[5];
    char exprs[BUF_SIZE];
}Calc;

Calc calist[20]={0,};


void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
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

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    
    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int state;
    
    int fds1[2], fds2[2];
    
    struct timeval timeout;
    fd_set reads, cpy_reads;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    
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
 
//    select에 사용될 fd_set 초기화
    FD_ZERO(&reads);
//    리스닝 소켓을 관찰대상으로 삼음. 리스닝 소켓 파일 디스크립터가 1로 바뀜
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;
    
    
    
    
    pipe(fds1);
    pipe(fds2);
    pid = fork();
    
    if(pid==0)
    {
        //pipe 1에선 쓰는거 닫고, pipe2 에선 읽는거 닫음
        close(fds1[1]);
        close(fds2[0]);
        
        char id[5];
        int len;
        
        while(1)
        {
//            msgbuf에 계산결과 양식에 맞게 써줘야 함 (process id): 3+4-2=5
//            종료조건
            char msgbuf[BUF_SIZE] ={0,};
            len=read(fds1[0], msgbuf, BUF_SIZE);
//            quit 전달

            if(!strncmp(msgbuf, "quit", 4))
            {
                break;
            }
//            loadiidd 전달, 비교, 파이프2번에 쓰기
            else if(!strncmp(msgbuf, "load", 4))
            {
                
                strcpy_slice(id, msgbuf, 4, 4);
                for(int j=0;j<20;j++)
                {
                    if(!strcmp(id, calist[j].id))
                    {
                        write(fds2[1], calist[j].exprs, strlen(calist[j].exprs));
                        continue;
                    }
                }
//                printf("here\n");
                write(fds2[1], "0", 1);
//                printf("write success? %d\n",len);
            }
            else
            {
//                                쭉 돌면서 확인하고, 문자열 배열에 id가 없으면, 새로 추가해주기
                for(int j=0;j<20;j++)
                {
//                                    printf("id(%d) 길이: %d\n",j,(int)strlen(calist[j].id));
                    if((int)strlen(calist[j].id)==4 && !strncmp(msgbuf,calist[j].id,4))
                    {
                        strncat(calist[j].exprs, msgbuf, strlen(msgbuf));
//                        calist[j].exprs[(int)strlen(calist[j].exprs)]=0;
//                        printf("calist check: %s",calist[j].exprs);
                        break;
                    }
                    
                    if(!(int)strlen(calist[j].id))
                    {
                        strncat(calist[j].id,msgbuf,4);
                        strncat(calist[j].exprs, msgbuf, strlen(msgbuf));
//                        printf("calist check: %s",calist[j].exprs);
//                        printf("id(%d) 수정: %d %s\n",j,(int)strlen(calist[j].id), calist[j].id);
                        break;
                    }
                }
            }
                
        }
        return 0;
    }
    else if(pid>0)
    {
        close(fds1[0]);
        close(fds2[1]);
        
        while(1)
        {
            struct iovec vec[3];
            char buf1[BUF_SIZE]={0,};
            char buf2[BUF_SIZE]={0,};
            char buf3[BUF_SIZE]={0,};
            vec[0].iov_base=buf1;
            vec[0].iov_len=4;
            vec[1].iov_base=buf2;
            vec[1].iov_len=4;
            vec[2].iov_base=buf3;
            vec[2].iov_len=BUF_SIZE;
            
            int str_len;
            
            cpy_reads = reads;
            timeout.tv_sec=5;
            timeout.tv_usec=5000;
            
            if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)
               break;
            
            if(fd_num==0)
               continue;
            
            for(i=0; i<fd_max+1; i++)
            {
                if(FD_ISSET(i, &cpy_reads))
                {
                    if(i==serv_sock)
                    {
                        adr_sz = sizeof(clnt_adr);
                        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                        FD_SET(clnt_sock, &reads);
                        if(fd_max<clnt_sock)
                            fd_max=clnt_sock;
                        printf("connected client: %d \n", clnt_sock);
                    }
                    else
                    {
                        str_len = readv(i, vec, 3);
//                        printf("success? %d\n",str_len);
                        if(str_len==0)
                        {
                            FD_CLR(i, &reads);
                            close(i);
                            printf("closed client: %d \n", i);
                        }
                        else if(str_len==-1)
                        {
                            continue;
                        }
                        else
                        {
//                            printf("mode: %s\n",(char*)vec[0].iov_base);
//                            printf("id: %s\n",(char*)vec[1].iov_base);
                            
                            if(!strcmp((char*)vec[0].iov_base,"save"))
                            {
                                printf("save to %s\n", (char*)vec[1].iov_base);
                                
                                char* temp = vec[2].iov_base;
    
                                char operator[BUF_SIZE];
                                int operand[BUF_SIZE];
                                int opCount = temp[0];
                                
                                for(int j=0; j<opCount; j++)
                                {
                                    operand[j] = ((temp[1+((j*4)+3)] << 24)&0xff000000) | ((temp[1+((j*4)+2)] << 16)&0x00ff0000) | ((temp[1+((j*4)+1)] << 8)&0x0000ff00) | (temp[1+(j*4)]&0x000000ff);
//                                    나중에 프린트 지워야 함. 확인용도임 다른것도 마찬가지!
//                                    printf("Operand %d: %d\n", j, operand[j]);
                                }
                                
                                for(int j=0; j<(opCount-1); j++)
                                {
                                    operator[j] = (char)temp[j+(opCount*4)+1];
//                                    printf("Operator %d: %c\n", j, operator[j]);
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
                                
                    //            클라이언트에게 결과 보내기
                                write(i, &result, 4);
                                
                                
                    //            표준출력하기
//                                printf("Operation result: %d\n", result);
                                
                                
                    //            결과 자식버퍼에 쓰기
                                char tmp[100]={0,};
//                                printf("id: %s",(char*)vec[1].iov_base);
                                sprintf(buf, "%s: ", (char*)vec[1].iov_base);
                                strcat(tmp, buf);
                                for(int i=0; i<opCount; i++)
                                {
                                    sprintf(buf, "%d", operand[i]);
                                    strcat(tmp, buf);
                                    if(i < (opCount-1))
                                    {
                                        sprintf(buf, "%c", operator[i]);
                                        strcat(tmp, buf);
                                    }
                                        
                                    
                                }
                                sprintf(buf, "=%d\n",result);
                                strcat(tmp, buf);
//                                printf("%s",tmp);
                                


//                                자식프로세스에 계산결과 갖다주기
                                write(fds1[1], tmp, strlen(tmp));
                            }
                            
                            if(!strcmp((char*)vec[0].iov_base,"load"))
                            {
                                char readbuf[BUF_SIZE]={0,};
//                                1. Mode가 Load일 경우 “load from ID정보”를 표준 출력하고 다음과 같이 동작함
                                printf("load from %s\n", (char*)vec[1].iov_base);
//                                Mode와 ID 정보를 파이프로 자식 프로세스에게 전달함
                                sprintf(buf,"load%s",(char*)vec[1].iov_base);
//                                printf("load buffer: %s\n",buf);
                                write(fds1[1], buf, strlen(buf));
//                                printf("child write success? %d",str_len);
//                                자식프로세스에서 계산결과 읽어오기
//                                sleep(1);
                                str_len = read(fds2[0], readbuf, BUF_SIZE);
//                                printf("read success? %d\n",str_len);
//                                클라이언트소켓에 적절한 형식으로 보내기
//                                받은게 길이가 0이면 Not exist 보내기
//                                sleep(1);
                                readbuf[str_len-1]=0;
                                if(str_len==1)
                                    write(i, "Not exist\n", strlen("Not exist\n"));
                                else
                                    write(i, readbuf, str_len);
                            }
                            
                            if(!strcmp((char*)vec[0].iov_base,"quit"))
                            {
//                                quit 표준출력
                                printf("quit\n");
//                                mode 정보 자식에게 전달
                                write(fds1[1], "quit", strlen("quit"));
                                break;
                            }
                            
                        }
                    }
                }
            }
                        
        }
        close(serv_sock);
    }
    
    
    return 0;
}
    

