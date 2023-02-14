#include <stdio.h>
#include <sys/times.h>
#include <stdlib.h>
#include <unistd.h>
//打印进程执行的时间

//打印调用其他程序返回值的 字符串 含义
static void pr_exit(int status){
    if( WIFEXITED(status) ){
        printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
    }else if( WIFSIGNALED(status) ){
        printf("abnormal termination, signal number = %d  %s\n",
            WTERMSIG(status),
            #ifdef WCOREDUMP
                WCOREDUMP(status) ? "(core file generated)" : "");
            #else
                "");
            #endif
    }else if( WIFSTOPPED(status) ){
        printf("child stoped, signal number = %d\n", WSTOPSIG(status));
    }
}
/******************************/

//打印时间
static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend){
    static long clktck = 0;
    if(clktck == 0){
        if( (clktck = sysconf(_SC_CLK_TCK)) < 0 ){
            printf("sysconf clk tck error\n");
        }
    }

    printf("real time: %f\n", real/(double)clktck); //实际时间
    printf("user: %f\n", (tmsend->tms_utime - tmsstart->tms_utime)/(double)clktck);
    printf("sys:  %f\n", (tmsend->tms_stime - tmsstart->tms_stime)/(double)clktck);

    printf("child user: %f\n", (tmsend->tms_cutime - tmsstart->tms_cutime)/(double)clktck);  //子进程时间
    printf("child sys:  %f\n", (tmsend->tms_cstime - tmsstart->tms_cstime)/(double)clktck);
}

static void do_cmd(const char * cmd){
    struct tms tmsstart, tmsend;
    clock_t start, end;
    int status;

    printf("command: %s\n", cmd);

    //和time函数不同
    if( (start = times(&tmsstart)) == -1 ){
        printf("time start error\n");
    }

    if( (status = system(cmd)) < 0 ){
        printf("system [%s] error\n", cmd);
    }

    if( (end = times(&tmsend)) == -1 ){
        printf("time end error\n");
    }

    pr_times( end-start, &tmsstart, &tmsend);
    pr_exit(status);
    printf("--------------------------\n");
}

//将命令行参数调用system函数执行，计算所使用的时间
int main(int argc, char *argv[]){
    int i;
    for(i=1; i<argc; i++){
        do_cmd(argv[i]);
    }
    exit(0);
}

// ./start "sleep 1" "date" "man bash > /dev/null"