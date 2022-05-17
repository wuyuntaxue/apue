/***
 * 屏障，允许任意数量的线程等待，直至所有调用屏障的线程到达屏障的wait函数，然后所有线程可以继续工作
 * pthread_barrier_t  屏障数据类型
 * pthread_barrier_init/pthread_barrier_destroy  初始化/销毁
 * pthread_barrier_wait  等待指定数量（初始化指定）的线程调用此函数，然后继续运行
 * ***/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUMS 8

pthread_barrier_t bWait;

void *pth_handler(void *arg){

    int *a = arg;
    sleep((*a) + 1);

    printf("pthread %d wait\n", *a);
    pthread_barrier_wait( &bWait);  //等待其他线程
    printf("pthread %d wait done\n", *a);

    pthread_exit(NULL);
}

int main(){
    pthread_t tid[NUMS];
    int countList[NUMS];

    //屏障、屏障属性、到达屏障的线程数量
    pthread_barrier_init( &bWait, NULL, NUMS+1);  //接下来要创建NUMS个线程，在主线程也调用wait，所以一共是NUMS+1个线程

    for(int i=0; i<NUMS; i++){
        countList[i] = i;
        if( 0 != pthread_create(&tid[i], NULL, pth_handler, (void *)&countList[i])){
            printf("pthread create failed, nums=%d\n", i);
        }
    }

    printf("main wait\n");
    pthread_barrier_wait( &bWait);  //等待其他线程到达wait函数
    printf("main wait done\n");  //等待完成，所有线程可以继续执行

    sleep(1);
    for(int i=0; i<NUMS; i++){
        pthread_join( tid[i], NULL);
    }

    pthread_barrier_destroy( &bWait);
    
}