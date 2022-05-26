/**
 * 线程和信号
 * pthread_sigmask() 设置线程的信号屏蔽字
 * sigwait()  阻塞等待线程中被挂起的信号
 * pthread_kill()  向线程发送信号
 * **/

/***
 * 设置线程的信号屏蔽字，然后这些被屏蔽的信号触发时就处于挂起状态
 * 使用sigwait等待指定的信号集中有被挂起的信号
 * 达到接收信号的目的
 * ***/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

sigset_t mask;

void *pth_handler(void *arg){
    int ret;
    int signo;
    for(;;){
        //要检测的信号集、返回的信号量
        ret = sigwait( &mask, &signo);  //如果信号集中有挂起的信号，则返回，并移除信号的挂起状态
        if(ret != 0){
            printf("sigwait failed\n");
            continue;
        }

        switch (signo)
        {
        case SIGINT:
            printf("recv inter\n");
            break;
        case SIGQUIT:
            printf("recv sigquit\n");
            pthread_exit(NULL);
            break;
        
        default:
            printf("unknown sig %d\n",signo);
            break;
        }
    }
}

int main(){
    pthread_t tid;

    sigemptyset(&mask);  //初始化空的信号集
    sigaddset( &mask, SIGINT);  //向信号集添加信号
    sigaddset( &mask, SIGQUIT);
    if( pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0){  //设置线程的信号屏蔽字
        printf("pthread sigmask failed\n");
    }

    if( pthread_create( &tid, NULL, pth_handler, NULL) != 0){  //新创建的线程会继承当前线程的屏蔽字
        printf("pthread create failed\n");
    }

    sleep(1);
    if(pthread_kill( tid, 0) != 0){  //检查线程是否存活
        printf("pthread invalid\n");
    }

    raise(SIGQUIT);
    //主线程并没有处理挂起信号，所以这个raise信号没有作用

    pthread_join( tid, NULL);
    return 0;
}
