/***
 * 线程特定数据（线程私有数据）
 * pthread_key_t  与数据关联的key的数据类型
 * pthread_key_create()  初始化一个key并指定析构函数，这个key可以被进程中的所有线程使用（只需要一份）
 * pthread_key_delete()  解除key和数据的绑定，但不会调用key关联的析构函数
 * pthread_getspecific()  返回key关联的特定数据，如果没有返回NULL
 * pthread_setspecific()  向key指定特定数据
 * 
 * 
 * pthread_once_t  
 * pthread_once()  确保pthread_once_t对应的函数只执行一次
 * ******/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXDATASIZE 2048

pthread_key_t pthKey;
pthread_once_t initDone = PTHREAD_ONCE_INIT;  //需要初始化成这个值
pthread_mutex_t mutexLock;

void get_data_destory(void *arg){
    printf("destory %x\n", arg);
    //arg参数就是特定数据的地址
    free(arg);  
}

void thread_key_init(){
    printf("key create\n");
    pthread_key_create( &pthKey, get_data_destory);  //初始化key，指定线程结束时的析构函数
}

char* get_data_r(char val){
    pthread_once( &initDone, thread_key_init);  //保证这个函数只执行一次，函数里初始化key值

    pthread_mutex_lock( &mutexLock);
    char* buf = (char *)pthread_getspecific(pthKey);  //获取线程特定数据
    if(buf == NULL){  //如果==NULL，说明没有指定数据
        printf("buf is null, start malloc\n");
        buf = malloc(MAXDATASIZE);  //申请空间
        if(buf == NULL){
            printf("malloc failed\n");
            pthread_mutex_unlock( &mutexLock);
            return NULL;
        }else{
            pthread_setspecific(pthKey, buf);  //指定特定数据
        }
    }

    /***用户操作****/
    //开始给buf赋值
    buf[0] = 0x10;
    buf[1] = val;

    pthread_mutex_unlock( &mutexLock);
    return buf;
}

void *pth1_handler(void *arg){
    char* dat = get_data_r(0x20);
    if(dat != NULL){
        printf("pth1 dat addr=%x, dat[0]=%x,dat[1]=%x\n", dat, dat[0], dat[1]);
    }

    printf("\n");
    sleep(1);
    printf("pth1 sleep 1, second get data-----------------\n");

    dat = get_data_r(0x22);
    if(dat != NULL){
        printf("pth1 dat addr=%x, dat[0]=%x,dat[1]=%x\n", dat, dat[0], dat[1]);
    }

    pthread_exit(NULL);
}

void *pth2_handler(void *arg){
    char* dat = get_data_r(0x30);
    if(dat != NULL){
        printf("pth2 dat addr=%x, dat[0]=%x,dat[1]=%x\n", dat, dat[0], dat[1]);
    }

    printf("\n");
    sleep(1);
    printf("pth2 sleep 1, second get data-----------------\n");

    dat = get_data_r(0x33);
    if(dat != NULL){
        printf("pth2 dat addr=%x, dat[0]=%x,dat[1]=%x\n", dat, dat[0], dat[1]);
    }

    pthread_exit(NULL);
}

int main(){
    pthread_t pthID[2];
    int ret;

    if(pthread_mutex_init( &mutexLock, NULL) != 0){
        printf("mutex init failed\n");
    }

    ret = pthread_create(&pthID[0], NULL, pth1_handler, NULL);
    if(ret != 0){
        printf("pth1 create failed\n");
    }
    ret = pthread_create(&pthID[1], NULL, pth2_handler, NULL);
    if(ret != 0){
        printf("pth2 create failed\n");
    }

    pthread_join( pthID[1], NULL);
    pthread_join( pthID[0], NULL);

    printf("main end\n");

    return 0;
}
/*****
 * 创建两个线程，分别调用get_data_r()函数，
 * 可以观察到，不同线程得到的地址不一样，
 * 而同一个线程再次调用时得到的地址和上次相同。
 * 说明，通过pthread_key_t，每个线程得到了自己的特定数据
 * 
 * ********/

//gcc -o start start1.c -lpthread