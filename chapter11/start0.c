/***
 * pthread_creat  创建线程
 * pthread_exit、pthread_cancel  结束线程
 * pthread_join 等待线程结束
 * pthread_cleanup_push\pthread_cleanup_pop  注册/执行线程清理处理函数
 * pthread_detach  分离线程（和主线程分离，之后不能join等待线程）
 * **/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void clean_up(void *arg){
    printf("cleanup: %s\n", (char *)arg);
}

void *thr_fn1(void * arg){
    //线程清理处理程序，在线程结束时执行，注册函数进入线程的栈中，先进后出
    //push和pop在这里是由宏实现的，需要在线程相同的作用域中成对出现
    //函数，传入的参数
    pthread_cleanup_push(clean_up, "thread 1 first handler");  //push入栈
    pthread_cleanup_push(clean_up, "thread 1 second handler"); //push入栈

    //pthread_exit(0);  //退出线程，可以正常调用cleanup注册的函数
    //return NULL;  //退出线程，没有调用cleanup注册的函数（在其他系统上还可能产生未知的问题）
    printf("th1 sleep start， arg = %d\n", *(int *)arg);
    sleep(5);
    printf("th1 sleep done\n");
    
    //即使线程在此之前退出（pthread_exit/其他线程调用pthread_cancel，如果是return则不行），也会按照先进后出的顺序执行
    pthread_cleanup_pop(0);  //pop出栈，参数为0代表不执行出栈的函数
    pthread_cleanup_pop(1);  //pop出栈，参数非0代表执行出栈函数
    //在此之后怎么退出线程则没关系

    pthread_exit(NULL);
}

void test_pthread_creat(){
    int ret;
    pthread_t tid1;
    void* tret;

    int a = 10;

    //线程ID，线程属性，执行函数，传递的参数
    ret = pthread_create(&tid1, NULL, thr_fn1, (void *)&a);
    if(ret != 0){
        printf("creat pthread pth1 failed\n");
    }

    sleep(1);

    pthread_cancel(tid1);  //向线程发送终止信号

    ret = pthread_join(tid1, &tret);  //阻塞等待线程终止
    if(ret != 0){
        printf("pthread join failed\n");
    }
}

int main(){
    test_pthread_creat();
    return 0;
}
//gcc -o start start0.c -lpthread