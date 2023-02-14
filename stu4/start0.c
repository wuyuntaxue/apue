#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>

//进程调度-友好值，友好值越小，优先级越高
//关于默认友好值，和友好值范围，在本机上与书中描写不一致，实际友好值范围-18~19，默认2
void test0(){
    int nzero;
    #ifdef NZERO
        nzero = NZERO;
    #elif defined(_SC_NZERO)
        nzero = sysconf(_SC_NZERO);  //获取默认友好度
    #else
        #error NZERO undefined
    #endif
    printf("NZERO = %d\n", nzero);  //默认友好度
    //但实际运行这个参数并无意义，和实际不符

    int curNzero = getpriority( PRIO_PROCESS, 0); //当前进程的友好度
    printf("curNZERO = %d\n", curNzero);  //当前友好度

    //int ret = setpriority( PRIO_PROCESS, 0, 10);  //设置友好度，当前友好度+10，可正可负，可以超出范围（函数会自动限幅）
    int ret = nice(-2);  //设置友好度，当前友好度-2，可正可负，可以超出范围（函数会自动限幅）
    if(ret == -1){
        printf("set nzero failed\n");
    }else{
        printf("set nzero sucess, %d\n", ret);
    }

    int modifiedNzero = getpriority( PRIO_PROCESS, 0); //修改之后的友好度
    printf("modifiedNzero = %d\n", modifiedNzero);
}

int main(){
    printf("user = %s\n", getlogin());  //如果函数没有连接到终端，则会失败

    test0();
    return 0;
}