/**
 * 线程取消选项：可取消状态、可取消类型。并未包含在pthread_sttr_t结构里
 * pthread_setcancelstate()  设置是否相应cancel
 * pthread_setcanceltype()  设置为异步取消或推迟取消
 * pthread_testcancel()  除了系统规定的取消点函数，还可以手动设置取消点
 * **/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *pth_handler(void *arg){
    int oldstate;
    int oldtype;

    //线程默认是相应取消信号的
    pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, &oldstate);   //设置取消状态，不响应pthread_cancel
    printf("cancel disable\n");
    
    sleep(1);  //在这里cancel不响应，待设置为使能后也会相应之前触发的cancel
    pthread_testcancel();  //手动设置取消点

    pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL);  //设置取消状态，为使能，相应pthread_cancel
    printf("cancel enable\n");

    /****/
    pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);  //设置为异步取消，线程可在任意时间取消，不是非要在检查点退出

    if(oldtype == PTHREAD_CANCEL_ASYNCHRONOUS){
        printf("oldtype == PTHREAD_CANCEL_ASYNCHRONOUS\n");
    }else if(oldtype == PTHREAD_CANCEL_DEFERRED){
        printf("oldtype == PTHREAD_CANCEL_DEFERRED\n");
    }

    sleep(3);
    printf("pthread end\n");
    pthread_exit(NULL);
}

int main(){
    pthread_t thID;

    if( pthread_create( &thID, NULL, pth_handler, NULL) != 0){
        printf("pthread create failed\n");
    }

    sleep(3);
    pthread_cancel(thID);
    printf("send cancel \n");

    pthread_join( thID, NULL);
}