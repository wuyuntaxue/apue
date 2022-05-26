/***
 * pthread_attr_t  线程属性
 * pthread_attr_init/pthread_attr_destroy  初始化/销毁
 * pthread_attr_setxxxxxx/pthread_attr_getxxxxxxxx  设置属性/获取属性
 * ********/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *pth_handler(void *arg){
    printf("pthread here\n");
    pthread_exit(NULL);
}

int main(){
    pthread_attr_t pAttr;
    pthread_attr_init( &pAttr);
    pthread_attr_setdetachstate( &pAttr, PTHREAD_CREATE_DETACHED);  //设置为创建后默认分离
    //相当于线程建立后调用pthread_detach

    int stat;
    if(0 != pthread_attr_getdetachstate( &pAttr, &stat) ){
        printf("detach state get failed\n");
    }else if(stat == PTHREAD_CREATE_DETACHED){
        printf("detach state = PTHREAD_CREATE_DETACHED\n");
    }

    pthread_t tid;
    if( 0 != pthread_create(&tid, &pAttr, pth_handler, NULL)){  //在创建线程时，传入属性参数
        printf("pthread create failed\n");
    }

    //pthread_detach(tid);
    sleep(1);

    pthread_attr_destroy( &pAttr);
    return 0;
}