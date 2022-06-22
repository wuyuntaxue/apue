//创建UDP通信socket，然后使用sendto()函数向指定的地址发送数据
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> // for close

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//命令行参数：端口号、要发送的字符串

int main(int argc, char * argv[])
{
    if(argc==1){
        printf("no argv\n");
    }else{
        printf("ip=%s port=%s\nsend:%s\n", argv[1],argv[2], argv[3]);
    }

    char addr[100]={0};
    strcpy( addr, argv[1]);
    int s_port = atoi(argv[2]);  //should 1025 - 65535

    //const char * s_address = "192.168.2.57";

    int udp_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(udp_fd == -1){
        perror("socket failed\n");
        return -1;
    }

    struct sockaddr_in dest_addr = {0};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(s_port);
    dest_addr.sin_addr.s_addr = inet_addr(addr);

    char buf[1024] = "Hello world";
    if(argc > 2){
        strcpy(buf, argv[3]);
    }
    

    sendto(udp_fd, buf, strlen(buf), 0, &dest_addr, sizeof(dest_addr));
    
    close(udp_fd);

    return 0;
}