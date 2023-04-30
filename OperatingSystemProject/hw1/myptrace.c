#define _CRT_SECURE_NO_WARNINGS
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#define BUF_SIZE 128
#define SYSCALL_SIZE 500
#define SYMBOL_SIZE 20

typedef struct syscall_
{
    char* symbol;
    int num;
    int count;
}SYSCALL;

char * strcpy_slice(char *buf, char *s, int start, int size);
SYSCALL token(char *buf);
int get_syscall_table(SYSCALL* buf);
int cmpfunc(const void *a, const void *b);

int main(int argc, char **argv)
{
    int status, callnum;
    int total = 0;
    int syscall_entry = 1;
    struct user_regs_struct reg;
    
    SYSCALL *syscall_table = (SYSCALL*)calloc(SYSCALL_SIZE, sizeof(SYSCALL));
    
    get_syscall_table(syscall_table);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"arguments\"\n", argv[0]);
        exit(1);
    }


    pid_t child = fork();
    if (child == 0)
    {
        argc -=1;
        argv +=1;

        char *args [argc+1];
        memcpy(args, argv, argc * sizeof(char*));
        args[argc] = NULL;
        ptrace(PTRACE_TRACEME, NULL, NULL, NULL);
        execvp(args[0], args);
        exit(1);

    }
    else if(child>0)
    {
        wait(&status);
        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);
        while(1) {
            if(syscall_entry==1)
            {
                ptrace(PTRACE_GETREGS, child, NULL, &reg);
                callnum = reg.orig_rax;
                syscall_table[callnum].count++;
                total++;

                syscall_entry = 0;
            }
            else
            {
                syscall_entry = 1;
            }

 
            ptrace(PTRACE_SYSCALL, child, 0, 0);
            wait(&status);
            if (WIFEXITED(status))
                break;
            }
        }
    else
        printf("fork error\n");
        
  
    printf("Total number of syscalls: %d\n", total);

    qsort((void *)syscall_table, SYSCALL_SIZE, sizeof(SYSCALL), cmpfunc);

    for(int i=0; i<SYSCALL_SIZE;i++)
    {
        if(syscall_table[i].count!=0)
        {
            printf("    %-4d %s\n", syscall_table[i].count, syscall_table[i].symbol);
        }
    }

    free(syscall_table);

    return 0;

}



char * strcpy_slice(char *buf, char *s, int start, int size)
{
  if (strlen(s) > start) {
    s += start;   
    while (size-- > 0 && *s != '\0')   
      *(buf++) = *(s++);   
    *buf = '\0';   
  }
  return buf;
}

SYSCALL token(char *buf)
{
    SYSCALL sys_;
    memset(&sys_, 0, sizeof(SYSCALL));
    char* sym;

    char* ptr = strtok(buf, " ");
    while(ptr!= NULL)
    {
        if(strstr(ptr, "__NR_")!=NULL)
        {
            sym=ptr;
        }
        if(atoi(ptr)!=0)
        {
            sys_.num=atoi(ptr);
            sys_.symbol=sym;
        }
        ptr = strtok(NULL, " ");
    }

    return sys_;
}


int get_syscall_table(SYSCALL* arr)
{
    char* text;
    SYSCALL temp;

    arr[0].num=0;
    arr[0].symbol= "read";

    FILE* fp = NULL;

    fp = fopen("/usr/include/x86_64-linux-gnu/asm/unistd_64.h", "r");
    if (fp == NULL)
    {
        fprintf(stderr, "File Open Error!\n");
        exit(1);
    }

    while(1)
    {
        text = (char*)malloc(sizeof(char)*BUF_SIZE);
        if(fgets(text, BUF_SIZE, fp)==NULL)
            break;
        //
        temp = token(text);
        if(temp.num!=0)
        {
            arr[temp.num].symbol=temp.symbol;
            arr[temp.num].num=temp.num;
        }
    }

    //preprocessing
    for(int i =0; i<SYSCALL_SIZE;i++)
    {
        arr[i].num=i;
        if(arr[i].symbol!=NULL)
            strcpy_slice(arr[i].symbol, arr[i].symbol, 5, 20);
        //printf("%d : %s\n", arr[i].num, arr[i].symbol);
    }
    free(text);
    fclose(fp);

    return 0;
}

int cmpfunc(const void *a, const void *b)
{
    SYSCALL *item1= (SYSCALL*)a;
    SYSCALL *item2= (SYSCALL*)b;
    return item2->count > item1->count;
}
