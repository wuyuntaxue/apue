#include<stdio.h>
#include<stdlib.h>

void exit_handler1(){
    printf("exit handler1 \n");
}
void exit_handler2(){
    printf("exit handler2 \n");
}
void exit_handler3(){
    printf("exit handler3 \n");
}

int main(int argc, char *argv[]){
    atexit(exit_handler1);  //登记终止处理程序，先登记的最后执行？
    atexit(exit_handler2);
    printf("hello\n");
    atexit(exit_handler3);

    //命令行参数，argv[0]一般是程序名称，之后再是命令行参数
    for(unsigned int i=0; i<argc; i++){
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    //获取环境变量
    printf("PWD = %s\n", getenv("PWD"));
    printf("LOGNAME = %s\n", getenv("LOGNAME"));

    exit(0); //会先调用各种终止程序，之后再返回内核
    //_Exit(0); //直接返回内核
}