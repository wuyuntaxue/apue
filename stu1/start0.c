#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>   //close所需头文件

#define FILE_NAME "./test.txt"

void displayGetTFL(int val){
    //printf("val = %d\n", val);
    switch( val & O_ACCMODE){
        case O_RDONLY:
            printf("read only");
            break;
        case O_WRONLY:
            printf("write only");
            break;
        case O_RDWR:
            printf("read and write");
            break;
        default:
            printf("unkonwn access mode\n");
    }

    if(val & O_APPEND){
        printf(" , append");
    }
    if(val & O_SYNC){
        printf(" , sync");
    }
    if(val & O_CREAT){
        printf(" , creat");
    }

    printf("\n");
}

int main(int argc, char *argv[]){
    printf("Hello\n");
    int fd;
    fd = open(FILE_NAME, O_RDONLY | O_CREAT | O_SYNC, 0755);
    if(fd == -1){
        printf("open failed\n");
        return -1;
    }

    int val = fcntl(fd, F_GETFL, 0);  //F_GETTFL获取文件状态标志
    if(val < 0){
        printf("fcntl failed\n");
    }

    displayGetTFL(val);

    int fd2 = dup(fd);

    close(fd);
    


    val |= O_APPEND;
    if( fcntl(fd2, F_SETFL, val) < 0){  //F_SETTFL 设置文件状态
        printf("fcntl set failed\n");
    }else{
        int val2 = fcntl(fd2, F_GETFL, 0);
        if(val2 < 0){
            perror("fcntl val2 failed\n");
        }
        displayGetTFL(val2);
    }

    close(fd2);
    

    return 0;
}