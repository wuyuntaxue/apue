#### 线程的限制

一个进程内可创建的线程数、一个线程的栈的最小字节数、、、等等

#### 线程的属性设置/获取

分离属性、栈大小、栈地址、等

#### 同步属性设置/获取

互斥量（进程共享、健壮、状态）、读写锁（进程共享）、条件变量（进程共享、时钟）、屏障（进程共享）

线程和同步属性的设置获取函数格式都是类似的：

|            | 线程属性                    | 互斥量                     | 读写锁                        |
| ---------- | --------------------------- | -------------------------- | ----------------------------- |
| 数据类型   | pthread_attr_t              | pthread_mutexattr_t        | pthread_rwlockattr_t          |
| 初始化函数 | pthread_attr_init           | pthread_mutexattr_init     | pthread_rwlockattr_init       |
| 销毁       | pthread_attr_destroy        | pthread_mutexattr_destroy  | pthread_rwlockattr_destroy    |
| 获取属性   | pthread_attr_getxxxxx       | pthread_mutexattr_getxxxxx | pthread_rwlockattr_getpshared |
| 设置属性   | pthread_attr_setxxxxx       | pthread_mutexattr_setxxxxx | pthread_rwlockattr_setpshared |
|            | xxx代表对应不同属性的函数名 |                            |                               |



#### 线程特定数据

线程私有数据。通过pthread_key_t来给调用函数的每个线程一份特定的数据空间，让函数在不同线程重入时保证数据对每个线程独立



#### 线程取消选项

不包含在pthread_sttr_t结构里的两个函数

取消状态：设置是否相应cancel（enable或disable）和  

取消类型：异步取消（立即响应取消）或推迟取消（等到检查点响应取消）



#### 线程和信号

每个线程可以设置不同的信号屏蔽字，和处理方式

sigwait可以获取当前线程中挂起的信号（和其他线程可以分开）



#### 线程和fork

见书12.9章节



------



#### start0

线程属性



#### start1

线程特定数据（线程私有数据）



#### start2

取消选项



#### start3

线程和信号