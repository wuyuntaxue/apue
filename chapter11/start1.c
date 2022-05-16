/***
 * pthread_mutex_t  互斥锁类型
 * pthread_mutex_init  初始化互斥锁
 * pthread_mutex_destroy  销毁互斥锁，如果是动态申请的互斥锁（malloc）则需要在释放内存前调用
 * pthread_mutex_lock/pthread_mutex_unlock  互斥锁的锁定、解锁
 * pthread_mutex_trylock  锁定的非阻塞的版本
 * pthread_mutex_timedlock  锁定的设置超时时间版本
 * **/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

//pthread_mutex_t mutexLock = PTHREAD_MUTEX_INITIALIZER; //也可以这样初始化互斥锁
pthread_mutex_t mutexLock;

typedef struct{
    int count;
    int age;
}ParaType;


/*******************************************************/
void *pth_handler_try(void *arg){

    for(;;){
        //如果锁定成功，返回0，否则返回错误码（如已经被锁定）
        if(0 == pthread_mutex_trylock(&mutexLock)){  //这里前五秒被主线程锁定
            ParaType * a = arg;

            a->count += 10;
            a->age -= 10;

            pthread_mutex_unlock(&mutexLock);
            break;
        }else{
            printf("try lock failed\n");
        }
        usleep(1000*500);
    }

    pthread_exit(NULL);
}

//pthread_mutex_trylock演示
void test_try(){
    pthread_t thid;
    ParaType pa = {0,0};

    if(pthread_mutex_init(&mutexLock, NULL) != 0){  //初始化互斥锁
        printf("mutex init failed\n");
    }

    int ret = pthread_create(&thid, NULL, pth_handler_try, (void *)&pa);
    if(ret != 0){
        printf("pthread creat failed\n");
    }

    pthread_mutex_lock(&mutexLock);  //锁定，如果已经被锁定则阻塞

    for(int i=0; i < 5; i++){
        pa.age++;
        pa.count++;
        sleep(1);
    }

    pthread_mutex_unlock(&mutexLock);

    pthread_join(thid, NULL);
    printf("a=%d  c=%d\n", pa.age, pa.count);
}

/************************************************/
void *pth_handler_timed(void *arg){
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);  //当前时间
    t.tv_sec += 3; //加三秒

    //这里的时间是绝对时间
    if(0 == pthread_mutex_timedlock(&mutexLock, &t) ){  //如果在设置时间内成功锁定则返回0，否则返回错误码
        ParaType * a = arg;

        a->count += 1;
        a->age -= 1;
        printf("timedlock here\n");
        pthread_mutex_unlock(&mutexLock);
    }else{
        printf("mutex timeout\n");
    }

    pthread_exit(NULL);
}

//pthread_mutex_timedlock演示
void test_timed(){
    pthread_t thid;
    ParaType pa = {0,0};

    if(pthread_mutex_init(&mutexLock, NULL) != 0){
        printf("mutex init failed\n");
    }

    int ret = pthread_create(&thid, NULL, pth_handler_timed, (void *)&pa);
    if(ret != 0){
        printf("pthread creat failed\n");
    }

    pthread_mutex_lock(&mutexLock);

    for(int i=0; i < 5; i++){
        pa.age++;
        pa.count++;
        sleep(1);
    }

    pthread_mutex_unlock(&mutexLock);

    pthread_join(thid, NULL);
    printf("a=%d  c=%d\n", pa.age, pa.count);
}

/***********************************************/
int main(){
    test_try();
    printf("---------------\n");
    test_timed();
    return 0;
}
//gcc -o start start1.c -lpthread