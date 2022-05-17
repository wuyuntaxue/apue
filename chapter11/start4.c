/***
 * 自旋锁，适用于锁持有时间很短的情景。
 * 获取已经锁定的自旋锁时，程序会原地‘自旋’等待，不会切换至内核态的阻塞/睡眠，所以长时间持有自旋锁，会造成CPU的浪费；如果时间很短，则会省去用户/内核的切换的消耗
 * 有些互斥锁不会立即睡眠，而会短暂自旋再睡眠，所以用户态更多应使用互斥锁，自旋锁只在特定场景使用
 * 本例故意在自旋锁中加了大量sleep，可以看到在等待另一线程释放锁时，CPU的占用很高（有一个核可以达到100%）
 * 
 * pthread_spinlock_t  自旋锁类型
 * pthread_spin_init/pthread_spin_destroy  初始化/销毁
 * pthread_spin_lock/pthread_spin_trylock  锁定/不阻塞锁定
 * pthread_spin_unlock  解除锁定
 * ********/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_spinlock_t spinLock;

void *pth_handler(void *arg){
    pthread_spin_lock( &spinLock);

    printf("pthread sleep start\n");
    sleep(10);
    printf("pthread sleep end\n");

    pthread_spin_unlock( &spinLock);

    pthread_exit(NULL);
}

int main(){
    pthread_t pthid;  //线程ID
    
    pthread_spin_init( &spinLock, PTHREAD_PROCESS_PRIVATE);  //只有调用进程可使用

    if( 0 != pthread_create(&pthid, NULL, pth_handler, NULL)){
        printf("pthread create failed\n");
    }

    pthread_spin_lock( &spinLock);

    printf("main sleep start\n");
    sleep(10);
    printf("main sleep end\n");

    pthread_spin_unlock( &spinLock);

    pthread_spin_destroy( &spinLock);
    pthread_join( pthid, NULL);
}