#include<stdio.h>
#include<setjmp.h>
#include<sys/resource.h>

jmp_buf jmpBuffer;


void test1(){
    struct rlimit r1;
    //获得当前进程的资源限制，有一个对应的set函数可以设置
    getrlimit(RLIMIT_STACK, &r1);  //RLIMIT_STACK代表栈的最大长度

    printf("max stack:%ld\t%ld\n", r1.rlim_cur, r1.rlim_max);  //软限制和硬限制
}

//setjmp设置跳转位置
//longjmp跳转至设置的位置
void func1(){
    printf("func1\n");
    longjmp( jmpBuffer, 1);
}

int main(){

    int val = 1;
    volatile int volval = 2;
    register int regval = 3;
    static int  stval = 4;

    test1();

    if(setjmp(jmpBuffer) != 0){  //longjmp调用时，返回longjmp的第二个参数
        printf("i am jmp here\n");
        printf("val=%d volval=%d regval=%d stval=%d\n", val, volval, regval, stval);
    }else{  //自己调用时返回0
        val +=100; volval+=100;
        regval += 100; stval += 100;
        func1();
        printf("val=%d volval=%d regval=%d stval=%d\n", val, volval, regval, stval);
    }

    return 0;
}