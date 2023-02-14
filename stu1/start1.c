#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pwd.h>
#include<fcntl.h>

#define FILE_NAME "./test.txt"

void test0(){
    struct stat buf;

    int ret = lstat(FILE_NAME, &buf);
    if(ret < 0){
        printf("lstat failed\n");
        return;
    }

    if(S_ISREG(buf.st_mode)){
        printf("regular\n");
    }else if(S_ISDIR(buf.st_mode)){
        printf("directory\n");
    }

    if(S_ISGID & buf.st_mode){
        printf("gid\n");
    }

    if(S_ISUID & buf.st_mode){
        printf("uid\n");
    }
}

void test1(){
    if(0 == access(FILE_NAME,F_OK)){
        printf("F_OK\n");
    }
    if(0 == access(FILE_NAME,X_OK)){
        printf("X_OK\n");
    }
    if(0 == access(FILE_NAME,W_OK)){
        printf("W_OK\n");
    }
    if(0 == access(FILE_NAME,R_OK)){
        printf("R_OK\n");
    }
}

//chmod
void test2(){
    struct stat statbuf;
    if(stat("test.txt", &statbuf) < 0){ //获取当前状态
        printf("stat failed\n");
    }else{
        chmod("test.txt", (statbuf.st_mode & ~S_IXGRP) | S_ISGID); //在当前状态上进行修改，去掉组执行，打开了设置组ID位
    }

    //chmod("test.txt",0755);
}

//chown
void test3(){
    struct passwd* user = getpwnam("taxue");  //获取chown需要的uid和gid
    //printf("%d\t%d\t%s\t%s\n",user->pw_uid,user->pw_gid,user->pw_name,user->pw_passwd);
    
    if(0==chown("test.txt",user->pw_uid,user->pw_gid)){
        printf("set success\n");
    }else{
        printf("set failed\n");
    }
}

//truncate
void test4(){
    int ret = truncate("test.txt", 3);  //将文件长度截断为3
    if(0 == ret){
        printf("set sucess\n");
    }
}

//link
void test5(){
    int ret = link("test.txt","hh.txt");  //相当于硬链接
    if(0==ret){
        printf("link success\n");
    }else{
        printf("link failed\n");
    }

    symlink("test.txt","abc.txt");  //软连接

    unlink("test.txt"); //删除链接，使该文件的链接计数减一
}
void test6(){
    int fd = open("qwer.txt", O_RDWR|O_CREAT, 0755);
    if(fd < 0){
        printf("open failed\n");
    }else{
        printf("open ok\n");
    }

    sleep(5);

    //文件处于open时，unlink后文件内容仍在（文件系统上无法查看到，但仍在磁盘里）
    //程序结束后被释放
    if(unlink("qwer.txt") < 0){   
        printf("unlink failed\n");
    }

    write(fd,"hhh",sizeof("hhh"));

    sleep(10);
    printf("done\n");
}

void test7(){
    char buf[100]={0};
    int len=readlink("bbb",buf,sizeof(buf));
    printf("read=%s\tlen=%d\n",buf,len);
}

void test8(){
    size_t size = 1024;
    char buf[size];

    if(chdir("/home/taxue/work/practice/apue/stu1") < 0){
        printf("chdir failed\n");
        return;
    }
    
    
    if( NULL == getcwd(buf, size)){
        printf("getcwd failed\n");
        return;
    }
    printf("getcwd = %s\n",buf);
}


int main(){
    //int ret = symlink("aaa","bbb");  //相当于硬链接
    //if(0==ret){
    //    printf("link success\n");
    //}else{
    //    printf("link failed\n");
    //}
    test8();

    
    
    return 0;
}