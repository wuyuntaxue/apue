#include<stdio.h>
#include<unistd.h>

void test0(){
    int c;
    printf("test0\n");
    while( (c=getc(stdin)) != EOF){
        if( putc(c, stdout) == EOF){
            printf("output error\n");
        }
        fflush(NULL);  //冲洗缓冲区，可以将流做参数，表示冲洗
    }

    if(ferror(stdin)){
        printf("stdin error\n");
    }
}

//从标准输入读取一行，输出到标准输出
void test1(){
    char buf[1024];
    printf("test1\n");
    while(fgets(buf,1024,stdin) != NULL){
        if(fputs(buf,stdout) == EOF){
            printf("output error\n");
        }
    }

    if(ferror(stdin)){
        printf("stdin error\n");
    }
}

//创建临时文件
void test2(){
    char name[L_tmpnam], line[1024];
    FILE *fp;

    //tmpnam并不安全，创建和使用之间可能被其他进程创建同名文件
    printf("%s\n", tmpnam(NULL));  //参数为NULL时，返回临时文件的路径

    tmpnam(name);  //文件名保存到数组里
    printf("%s\n",name);

    if( (fp=tmpfile()) == NULL ){  //创建一个临时文件，返回文件的流，关闭文件或进程结束时自动释放
        printf("tmpfile error\n");
        return;
    }

    fputs("on line of output\n", fp);  //将字符串输出到io流里
    rewind(fp);  //将流的文件位置，设置到文件的开始位置

    if(fgets(line, sizeof(line), fp) == NULL){
        printf("fgets error\n");
        return;
    }

    fputs(line, stdout);
    //sleep(5);
}

//使用mkstemp创建临时文件
#include<stdlib.h>
#include<sys/stat.h>
#include<errno.h>

void maketmp_test3(char * template){
    int fd;
    struct stat sbuf;

    if( (fd=mkstemp(template)) < 0){
        printf("can not create temp file\n");
        return;
    }
    printf("temp name = %s\n",template);  //临时文件名

    close(fd);  //close后不会释放

    if( stat(template, &sbuf) < 0){
        if(errno == ENOENT){
            printf("file dose not exist\n");
        }else{
            printf("stat failed\n");
        }
    }else{
        printf("file exist\n");
    }

    unlink(template);  //不会自动删除，需要手动解除链接
}

void test3(){
    char good_template[] = "/tmp/dirXXXXXX";  //因为最后六个X是占位符，创建临时文件时会对其进行修改
    char *bad_template = "/tmp/dirXXXXXX";  //指针的形式只有指针在栈上，修改其指向的内容会段错误

    printf("-----------\n");
    maketmp_test3(good_template);

    printf("-----------\n");
    maketmp_test3(bad_template);
}

//使用fmemopen创建内存流
#include<string.h>
#define BSZ 48
void test4(){
    char buf[BSZ];
    FILE *fp;

    memset(buf, 'a', BSZ-2);
    buf[BSZ-2] = '\0';
    buf[BSZ-1] = 'X';
    printf("before initial buffer contents: %s\n", buf);
    //创建内存流，手动指定缓冲区、大小、方式
    if( (fp = fmemopen(buf, BSZ, "w+")) == NULL){  //fmemopen在缓冲区开始处放置null字节
        printf("fmemopen failed\n");
        return;
    }
    printf("initial buffer contents: %s\n", buf);  //由于buf的开始处是null字节，所以打印为空。字符串识别到null字节
    fprintf(fp, "hello mengyu");
    printf("before flush: %s\n", buf);  //冲洗之前，buf里仍没有hello mengyu
    fflush(fp);  //调用fflush,fclose,fseek,fseeko,fseekpos都会在当前位置写入一个null字节
    printf("after flush: %s\n", buf);  //冲洗之后，有了
    printf("len of string in buf = %ld\n", (long)strlen(buf));

    memset(buf, 'b', BSZ-2);
    buf[BSZ-2] = '\0';
    buf[BSZ-1] = 'X';
    fprintf(fp, "hello mengyu");  //fprintf之前，文件位置在12（hello mengyu长度）字节，只是字符被memset成了b
    fseek(fp, 0, SEEK_SET);  //从文件起始位置，偏移0个。引起缓冲区冲洗，再在当前位置（即开始位置）写入null字节（写入之后还未冲洗）
    printf("after fseek: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));

    memset(buf, 'c', BSZ-2);
    buf[BSZ-2] = '\0';
    buf[BSZ-1] = 'X';
    fprintf(fp, "hello mengyu");
    fclose(fp);  //fclose没有追加写null字节 ??
    printf("after fclose: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));
}

int main(){
    test4();
    char name[100];
    gethostname(name, sizeof(name));
    printf("name=%s\n",name);
    return 0;
}