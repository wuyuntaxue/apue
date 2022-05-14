/*********
 * sigset_t 信号集
 * sigprocmask 信号屏蔽字
 * sigpending 获取当前被阻塞的信号（例如：被屏蔽期间触发的信号）
 * sigaction 检查/修改指定信号相关联的动作（如果可以应该优先于signal函数使用）
 * sigsetjmp & siglongjmp 提供了可选的保存/恢复信号屏蔽字的函数外跳转
 * sigsuspend(sigmask) 设置信号屏蔽字为sigmask，然后阻塞进程，在捕获到信号并从处理程序返回后，该函数也返回，并将屏蔽字设置为调用前的值
 * abort()
 * strsignal()  返回信号的字符串描述
***********/
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void test_sigset(){
    sigset_t newSet;  //信号集
    sigemptyset(&newSet);  //初始化为空信号集

    sigaddset(&newSet, SIGINT);  //向信号集里增加信号
    sigaddset(&newSet, SIGTERM);

    if( sigismember(&newSet, SIGINT) ){  //判断信号集里是否包含SIGINT信号
        printf("newSet contain SIGINT\n");
    }else{
        printf("not contain SIGINT\n");
    }

    sigdelset(&newSet, SIGINT);  //从信号集里删除SIGINT信号
    printf("del SIGINT in newSet\n");
    if( sigismember(&newSet, SIGINT) ){
        printf("newSet contain SIGINT\n");
    }else{
        printf("not contain SIGINT\n");
    }
}

//sigprocmask仅针对单线程的进程
void test_sigprocmask(){
    sigset_t newSet,oldSet;
    sigemptyset(&newSet);
    sigaddset(&newSet, SIGINT);

    //how:SIG_BLOCK新增set， SIG_UNBLOCK减去set， SIG_SETMASK覆盖设置
    //新增的方式增加newSet信号集里的信号，返回之前进程的屏蔽字到oldSet里
    sigprocmask(SIG_BLOCK, &newSet, &oldSet);  //新增屏蔽集newSet里的信号

    printf("block SIGINT\n");
    sleep(10);

    //如果屏蔽期间触发了信号，则进程不做响应，取消屏蔽后会做一次响应（触发多次也只有一次响应）
    sigprocmask(SIG_SETMASK, &oldSet, NULL);  //设置回之前的信号屏蔽字
    printf("unblock SIGINT\n");
}

//获取当前被阻塞的信号
void test_sigpending(){
    sigset_t newSet,oldSet;
    sigemptyset(&newSet);
    sigaddset(&newSet, SIGINT);

    sigprocmask(SIG_BLOCK, &newSet, &oldSet);  //新增屏蔽集newSet里的信号

    printf("block SIGINT\n");
    sleep(5);

    sigset_t blockSet;
    sigpending(&blockSet);  //获取阻塞/未决的信号
    if( sigismember(&blockSet, SIGINT) ){  //如果sleep期间触发了SIGINT，则这个信号集里会有这个信号
        printf("sigint in pending\n");
    }else{
        printf("sigint not in pending\n");
    }

    sleep(5);

    sigprocmask(SIG_SETMASK, &oldSet, NULL);  //复原信号屏蔽字
    printf("unblock SIGINT\n");
}

//用sigaction函数实现signal函数
//sigaction实现的信号捕获，在处理信号时会阻塞相关信号
__sighandler_t signal_realize(int signo, __sighandler_t func){
    struct sigaction act,oact;

    act.sa_handler = func;  //处理函数
    sigemptyset(&act.sa_mask);  //初始化结构体中的信号集
    
    act.sa_flags = 0;
    if(signo == SIGALRM){  //不希望alarm信号重启系统调用，可以用来中断read阻塞啥的
        #ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
        #endif
    }else{
        act.sa_flags |= SA_RESTART;
    }

    if( sigaction(signo, &act, &oact) < 0){
        return SIG_ERR;
    }

    return oact.sa_handler;
}

void sig_int_handler(int signo){
    printf("received sig:%d\n",signo);
    sleep(2);
}


/*************************************/
//打印进程的当前信号屏蔽字
#include <errno.h>
void pr_mask(const char * str){
    sigset_t sigset;
    int errnoSave;

    errnoSave = errno;
    if( sigprocmask( 0, NULL, &sigset) < 0 ){  //读取当前信号屏蔽字
        printf("get sig mask error\n");
    }else{
        printf("%s: ",str);
        if( sigismember(&sigset, SIGINT) ){
            printf("SIGINT ");
        }
        if( sigismember(&sigset, SIGQUIT) ){
            printf("SIGQUIT ");
        }
        if( sigismember(&sigset, SIGUSR1) ){
            printf("SIGUSR1 ");
        }
        if( sigismember(&sigset, SIGALRM) ){
            printf("SIGALRM ");
        }
        printf("\n");
    }
}

//siglongjmp sigsetjmp可选择保存恢复屏蔽字
#include <setjmp.h>
#include <time.h>

sigjmp_buf jmpbuf;
int canjump=0;

void sig_usr1_handler(int signo){
    time_t starttime;

    if(canjump == 0){
        return;
    }

    //触发USR1信号，这时信号屏蔽字为SIG_USR1
    pr_mask("before alarm");

    alarm(3);
    starttime = time(NULL);

    while( time(NULL) < starttime + 5);  //5s

    //ALRM信号处理结束，这时信号屏蔽字依然为SIG_USR1
    pr_mask("after alarm");

    canjump = 0;
    siglongjmp(jmpbuf, 1);
    
}

void sig_alrm_handler(int signo){
    printf("received SIGALRM\n");
    pr_mask("in alrm handler");
    //在USR1还未退出的时候触发的SIGALRM信号，这时信号屏蔽字为 SIGUSR1 | SIGALRM
}

void test_sigjmp(){
    if( signal_realize(SIGUSR1, sig_usr1_handler) == SIG_ERR ){
        printf("SIGUSR1 failed\n");
    }

    if( signal_realize(SIGALRM, sig_alrm_handler) == SIG_ERR ){
        printf("SIGALRM failed\n");
    }

    //如果第二个参数非0，则保存当前屏蔽字，跳转时恢复
    if( sigsetjmp(jmpbuf, 1) ){  //直接调用返回0，从siglongjmp跳转返回非0
        printf("jmp here\n");
        pr_mask("end");
        _exit(0);
    }
    canjump = 1;

    for(;;)
        pause();
}

/******************************/
#include <stdlib.h>
void test_abort(){
    //将SIGABRT发送给调用进程、进程不应该忽略此信号
    abort();  //使程序异常终止
}

#include <string.h>  //strsignal()

int main(){
    printf("%s\n",strsignal(SIGALRM));
    printf("%s\n",strsignal(SIGINT));

    test_sigjmp();
    
    //pause();  //阻塞进程，直到收到信号
}