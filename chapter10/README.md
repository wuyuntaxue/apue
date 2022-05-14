#### start0

```
signal()              信号捕获、修改信号处理方式
SIGCHLD和SIGCLD信号    子进程终止时，父进程会收到此信号
alarm()和SIGALRM信号   可以用alarm中断系统调用(例如read())，但会因系统而异
利用longjmp()避免read不会被SIGALRM中断
sigprocmask()         设置信号屏蔽字
sigpending()          获取当前阻塞/未决的信号
sigismember()         判断信号集是否包含指定信号
```



#### start1

10-11  ~ 10-16

```
信号集 
信号屏蔽字
获取被阻塞的信号
比signal()更好的sigaction()、用sigaction()实现signal()
程序有信号处理时替代setjmp()/longjmp()使用的跳转sigsetjmp()/siglongjmp()
sigsuspend()
strsignal() 描述信号的字符串
```

