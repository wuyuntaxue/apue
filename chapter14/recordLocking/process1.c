/************
 * 记录锁（record locking）
 * 使用fcntl()在多进程间给文件的一部分加锁
 * ***************/


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("lost arg\n");
        exit(-1);
    }

    //打开测试用的文件
    int fd = open(argv[1], O_RDWR | O_CREAT, 0755);
    if(fd == -1){
        printf("open file %s failed\n", argv[1]);
        exit(-1);
    }

    //100字节，到文件结尾
    struct flock flk={
        l_type:   F_WRLCK,    //写锁
        l_whence: SEEK_SET,
        l_start:  100,
        l_len: 0,  //0意味着从指定的偏移位置到文件末尾，即使新增也包括在内
    };
    //尝试获取记录锁
    int ret = fcntl(fd, F_SETLK, &flk);  //F_SETLK尝试获取锁、不阻塞；F_SETLKW是它的阻塞版
    if(ret == -1){
        printf("setlk failed\n");
    }else{
        sleep(10);
        
        lseek( fd, 100, SEEK_SET);  //设置文件偏移（不会影响其他进程）
        char dat[100]={0};
        sprintf(dat, "hello record lock, I am %s %d\n", argv[0], getpid());
        write( fd, dat, strlen(dat));
        

        flk.l_type = F_UNLCK;  //解锁
        if( fcntl(fd, F_SETLK, &flk) == -1 ){
            printf("F_UNLCK failed\n");
        }
    }

    printf("done\n");
    return 0;
}
