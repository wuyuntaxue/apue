/***
 * 条件变量，配合互斥量使用。对互斥量进行条件保护
 * pthread_cond_t  条件变量类型 
 * pthread_cond_init  初始化
 * pthread_cond_wait/pthread_cond_timedwait  等待/超时等待条件变量的信号，等待时释放指定的互斥量，等待结束重新锁住互斥量
 * pthread_cond_signal/pthread_cond_broadcast  向 至少一个/全部 等待该条件变量的线程发送信号
 * pthread_cond_destroy  销毁
 * *******/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct{
    int age;
    int count;
}PersonType;

pthread_cond_t qCond;
pthread_mutex_t qMutex;

void *pth_handler(void *arg){
    pthread_mutex_lock( &qMutex);
    printf("cond wait start\n");
    //把锁住的互斥量传入函数
    pthread_cond_wait( &qCond, &qMutex);  //等待条件变量信号，这时候不占用互斥量
    //等wait函数返回时，互斥量再次被锁住
    printf("cond wait done\n");

    PersonType * p = arg;
    p->count++;
    p->age = 20;

    pthread_mutex_unlock( &qMutex);

    pthread_exit(NULL);
}

int main(){
    pthread_t pthid;  //线程ID
    PersonType p1 = {0,0};

    pthread_mutex_init( &qMutex, NULL);
    pthread_cond_init( &qCond, NULL);  //初始化条件变量

    if( 0 != pthread_create(&pthid, NULL, pth_handler, (void *)&p1)){
        printf("pthread create failed\n");
    }

    sleep(1);  //让刚才创建的线程先执行

    //修改p1
    printf("main sleep done\n");
    pthread_mutex_lock( &qMutex); 

    p1.age = 10;
    p1.count++;
    printf("main change p\n");

    pthread_mutex_unlock( &qMutex);

    printf("cond signal\n");
    //pthread_cond_signal( &qCond);  //向至少一个等待该条件变量的线程发信号
    pthread_cond_broadcast( &qCond);  //向所有等待该条件变量的线程发信号


    pthread_cond_destroy(&qCond);
    pthread_mutex_destroy(&qMutex);

    pthread_join( pthid, NULL);

    printf("age = %d, count = %d\n", p1.age, p1.count);
}