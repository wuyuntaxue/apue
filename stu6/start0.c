#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/*************************************/
//信号处理函数
void sig_usr_handler(int sig){
    if( sig == SIGUSR1 ){
        printf("received SIGUSR1\n");
        sleep(2); //这个信号看起来是支持排队的
    }else if( sig == SIGUSR2 ){
        printf("received SIGUSR2\n");
    }else{
        printf("received unknown sig\n");
    }

    printf("SIG handler done\n");
}

//测试信号捕获
void test0(){
    //捕获USR1信号  kill -USR1 pidnum
    if(signal( SIGUSR1, sig_usr_handler) == SIG_ERR){
        printf("signal SIGUSR1 error\n");
    }

    if(signal( SIGUSR2, sig_usr_handler) == SIG_ERR){
        printf("signal SIGUSR2 error\n");
    }

    //用户中断信号（Ctrl+c），设置为忽略
    if( signal(SIGINT, SIG_IGN) == SIG_ERR){
        printf("signal SIGINT error\n");
    }

    for(;;){
        pause();  //使调用进程（或线程）睡眠状态，直到接收到信号，要么终止
    }

}

/************************************/
//SIGCHLD和SIGCLD信号
//在Linux中两个信号宏定义相同（其他Unix系统不一定），子进程终止时，父进程会收到此信号，但此信号的默认状态是忽略
#include <sys/wait.h>

void sig_cld(int sig){
    pid_t pid;
    int status;
    printf("SIGCHLD received\n");
    if( (pid=wait(&status)) < 0 ){
        printf("wait error\n");
    }else{
        printf("wait pid=%d\n", pid);
    }
}

void test1(){
    pid_t pid;
    if( signal(SIGCHLD, sig_cld) == SIG_ERR ){  //捕获信号
        printf("signal SIGCHLD error\n");
    }

    if( (pid=fork()) < 0){
        printf("fork error\n");
    }else if(pid == 0){
        sleep(10);
        _exit(0);  //子进程终止
    }else{
        printf("child pid=%d\n", pid);
    }

    pause();  //阻塞进程，等待信号
    _exit(0);
}

/************************************/
//alarm和SIGALRM信号
void sig_alrm(int sig){
    printf("sig alrm\n");
}

void test2(){
    char line[1024];
    int n;

    if( signal(SIGALRM, sig_alrm) == SIG_ERR ){
        printf("signal SIGALRM error\n");
    }

    alarm(10);  //设置10s闹钟信号
    //在这里SIGALRM信号会中断read（会因系统而异），就达到设置read超时时间的目的
    if( (n = read(STDIN_FILENO, line, sizeof(line))) < 0 ){  
        printf("read linr error,return:%d\n", n);
    }
    alarm(0);  //取消闹钟

    printf("out----------\n");
    write(STDOUT_FILENO, line, n);
    _exit(0);
}


/***********************************/
//使用longjmp()避免read不会因为SIGALRM信号中断，和上一个例子类似，加了跳转

#include <setjmp.h>
jmp_buf envAlrmJmp;
void sig_alrm1(int sig){
    printf("sig alrm\n");
    longjmp(envAlrmJmp, 1);
}

void test3(){
    char line[1024];
    int n;

    if( signal(SIGALRM, sig_alrm1) == SIG_ERR ){
        printf("signal SIGALRM error\n");
    }

    if( setjmp(envAlrmJmp) != 0 ){  //自己调用返回0，longjmp跳转到这里时返回longjmp的第二个参数
        printf("read time out\n");
        return;
    }

    alarm(10);  //设置10s闹钟信号
    //在这里SIGALRM信号会中断read（会因系统而异），就达到设置read超时时间的目的
    if( (n = read(STDIN_FILENO, line, sizeof(line))) < 0 ){  
        printf("read linr error,return:%d\n", n);
    }
    alarm(0);  //取消闹钟

    printf("out----------\n");
    write(STDOUT_FILENO, line, n);
    _exit(0);
}

/*************************************/
//打印进程的当前信号屏蔽字
#include <errno.h>
void pr_mask(){
    sigset_t sigset;
    int errnoSave;

    errnoSave = errno;
    if( sigprocmask( 0, NULL, &sigset) < 0 ){  //读取当前信号屏蔽字
        printf("get sig mask error\n");
    }else{
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

/*********************************/
//SIGQUIT  CTRL + '\'
//
void sig_quit(int signo){
    printf("received SIGQUIT\n");
    if( signal( SIGQUIT, SIG_DFL) == SIG_ERR ){  //将SIGQUIT设置为默认处理方式
        printf("signal set SIGQUIT default error\n");
    }
}

void test4(){
    sigset_t newmask, oldmask, pendmask;

    if( signal(SIGQUIT, sig_quit) == SIG_ERR ){  //给SIGQUIT信号设置捕获函数
        printf("signal SIGQUIT error\n");
        return;
    }

    sigemptyset( &newmask);  //初始化一个空的信号集
    sigaddset( &newmask, SIGQUIT); //向这个信号集里添加SIGQUIT信号

    if( sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0 ){  //新增阻塞newmask集里的信号，当前的信号屏蔽字传入oldmask
        printf("sigprocmask error\n");
    }

    sleep(5);  //在这期间触发SIGQUIT信号是被阻塞的

    if( sigpending(&pendmask) < 0 ){  //获取当前‘未决’的信号，阻塞的信号多次触发并没有队列存储
        printf("sigpending error\n");
    }
    if( sigismember( &pendmask, SIGQUIT) ){  //判断其中有没有SIGQUIT
        printf("SIGQUIT pending\n"); //如果有，打印，并且会触发之前设置的捕获函数
    }

    if( sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0 ){  //复原信号屏蔽字（设置为oldmask）
        printf("sigprocmask SIG_SETMASK error\n");
    }
    printf("SIGQUIT unblocked\n");

    sleep(5);
    _exit(0);

}

int main(){
    test4();

    return 0;
}
