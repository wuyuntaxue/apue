#include <stdio.h>
#include <unistd.h>


void test0(){
    printf("pid group = %d\n", getpgrp());

    printf("pid group = %d\n", getpgid(0));  //参数是0表示获取本进程的组ID
    //setpgid()
    
    printf("pid group = %d\n", getsid(0));  //调用进程的会话首进程的进程组ID ？？
}

int main(){
    test0();
    return 0;
}