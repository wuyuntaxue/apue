/******
 * 利用select同时监听多个udp端口
 * 
 * *****/

#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//初始化udp的socket，监听端口，参数为端口号，返回值为socket句柄
int socket_init(int port){
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_fd < 0){
        perror("creat socket failed\n");
        return -1;
    }

    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons( port);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(udp_fd, &local_addr, sizeof(local_addr));
    if(ret < 0){
        perror("bind failed\n");
        close(udp_fd);
        return -1;
    }else{
        printf("bind success\n");
    }
    return udp_fd;
}

//读取socket数据，并打印
int udp_recv(int udp_fd){
    struct sockaddr_in src_addr = {0};
    int src_addr_len = sizeof(src_addr);
    char buf[1024] = {0};

    if( -1 == recvfrom(udp_fd, buf, sizeof(buf), 0, &src_addr, &src_addr_len)){
        perror("recv failed\n");
        return -1;
    }
    printf("from%s:%d ", inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port));
    printf("recv=%s\n",buf);
}

int main(){
    //监听9870 - 9874五个端口，创建对应的socket
    int fdArr[5]={0};
    fdArr[0] = socket_init(9870);
    fdArr[1] = socket_init(9871);
    fdArr[2] = socket_init(9872);
    fdArr[3] = socket_init(9873);
    fdArr[4] = socket_init(9874);

    for(int i=0; i<sizeof(fdArr)/sizeof(int); i++){
        if(fdArr[i] < 0){
            printf("socket %d init failed\n", i);
            return -1;
        }
    }

    fd_set fdReadSet;

    for(;;){

        FD_ZERO(&fdReadSet);  //清空fd_set
        for(int i=0; i<sizeof(fdArr)/sizeof(int); i++){
            FD_SET( fdArr[i], &fdReadSet);  //将描述符 加入到 fd_set
        }

        //描述符最大量、读描述符集、写集、异常集、等待时长(为NULL时表示一直等待)
        int ret = select( FD_SETSIZE, &fdReadSet, NULL, NULL, NULL);
        printf("ret=%d\n", ret);
        if(ret < 0){
            printf("select failed\n");  //select出错
        }else if(ret == 0){
            printf("fdset not ready\n");  //超时后，仍然没有描述符准备好
        }else{
            //有描述符准备好，需要轮询描述符，匹配准备好的
            for(int i=0; i<sizeof(fdArr)/sizeof(int); i++){
                if( FD_ISSET( fdArr[i], &fdReadSet) ){  //是否匹配
                    printf("fd %d isset\n",i);
                    udp_recv(fdArr[i]);
                }
            }
        }
    }
}