#include<stdio.h>
#include<sys/time.h>
#include<time.h>

//系统时间和日期
void test0(){
    time_t tim;
    struct tm *mytm;
    char buf[64];

    tim = time(&tim);  //获取系统时间，存入time_t结构里，失败返回负数
    //mytm = gmtime( &tim );  //将time_t格式的时间转换成tm格式的
    mytm = localtime( &tim ); //和gmtime相同，但包含本地的时区和夏令时计算

    printf("tm year=%d  mon=%d day=%d  hour=%d  min=%d  sec=%d\n",\
        mytm->tm_year, mytm->tm_mon, mytm->tm_mday, \
        mytm->tm_hour, mytm->tm_min, mytm->tm_sec);

    if (0 == strftime(buf, sizeof(buf), "hello %r, %a %b %d, %Y", mytm) ){  //按照格式将tm转换成字符串
        printf("buf len too small\n");
    }else{
        printf("%s\n",buf);
    }

    time_t tim_set;
    tim_set = mktime(mytm);
    int ret = stime(&tim_set);  //stime设置系统时间，mktime将tm转换成time_t
    if(ret != 0){
        printf("set failed\n");
    }
    //printf("set time ret = %d\n", ret);
}

int main(){
    //printf("------\n");
    test0();
    return 0;
}