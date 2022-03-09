#include<stdio.h>
#include<unistd.h>

#include<stdlib.h>
#include<sys/wait.h>

void pr_exit(int status){
    if( WIFEXITED(status) ){
        printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
    }else if( WIFSIGNALED(status) ){
        printf("abnormal termination, signal number = %d  %s\n",
            WTERMSIG(status),
            #ifdef WCOREDUMP
                WCOREDUMP(status) ? "(core file generated)" : "");
            #else
                "");
            #endif
    }else if( WIFSTOPPED(status) ){
        printf("child stoped, signal number = %d\n", WSTOPSIG(status));
    }
}

int main(){

    pid_t pid;
    int status;

    if( (pid = fork()) < 0 ){
        printf("fork failed\n");
    }else if(pid==0){
        exit(7);
    }

    if( wait(&status) != pid ){  //取得已终止的子进程的终止状态，如果没有则阻塞
        printf("wait failed\n");
    }
    pr_exit(status);

    /***********************/
    if( (pid = fork()) < 0 ){
        printf("fork failed\n");
    }else if(pid==0){
        abort();
    }

    if( wait(&status) != pid ){
        printf("wait failed\n");
    }
    pr_exit(status);

    /***********************/
    if( (pid = fork()) < 0 ){
        printf("fork failed\n");
    }else if(pid==0){
        status /= 0;
    }

    if( waitpid(pid, &status, 0) != pid ){  //等待指定pid的子进程
        printf("wait failed\n");
    }
    pr_exit(status);

    /***********************/
    if( (pid = fork()) < 0 ){
        printf("fork failed\n");
    }else if(pid==0){   //first子进程
        if( (pid = fork()) < 0){
            printf("fork failed\n");
        }else if(pid > 0){  //返回子进程的pid，这里还是first子进程
            exit(0);
        }

        //这里是second子进程，它的父进程是first。
        //但first直接exit，second进程会sleep(2)，会成为孤儿进程，交由init进程领养
        //这样主进程结束时，不必wait处理second的善后
        sleep(2);
        printf("second child, parent pid = %ld\n", (long)getppid());
        exit(0);
    }

    if( waitpid(pid, NULL, 0) != pid ){
        printf("wait failed\n");
    }

    return 0;
}