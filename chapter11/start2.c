/****
 * rwlock 读写锁
 * ****/

#include <stdio.h>
#include <pthread.h>

/*******************************/
pthread_rwlock_t rwLock;

void test_rwlock(){
    if(0 != pthread_rwlock_init( &rwLock, NULL) ){
        printf("rwlock init failed\n");
    }

    pthread_rwlock_wrlock(&rwLock);  //写锁，不能同时写
    pthread_rwlock_unlock(&rwLock);

    pthread_rwlock_rdlock(&rwLock);  //读锁，在没有被写锁锁定时，可以同时读
    pthread_rwlock_unlock(&rwLock);

    //读写锁还提供了（非阻塞）try和（带超时）timed版本的读锁和写锁函数

    pthread_rwlock_destroy(&rwLock);  //读写锁需要销毁
}

int main(){
    
}