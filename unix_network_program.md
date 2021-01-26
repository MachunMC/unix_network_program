# 一、编程基础

## 1. 网络开发模型

B/S模型（browser / server）：浏览器与服务器模型。对服务器要求高，计算均在服务端进行，但移植性好，不依赖具体平台

C/S模型（client / server ）：客户端与服务器模型

## 2. TCP/IP协议栈

<img src="https://tse2-mm.cn.bing.net/th/id/OIP.VsYdx31XsYvx8UkfjgLWUwHaEj?pid=Api&amp;rs=1" style="zoom:120%;" />



TCP和UDP属于传输层协议，IP为网络层协议

**TCP，传输控制协议**

特点：

- 面向连接，类似于打电话的过程，双方需要先建立连接，然后才能进行通信

- 具有超时重传、拥塞控制等机制，可以保证数据的可靠传输

  

**UDP 用户数据报协议**

特点：

- 面向无连接，类似于写信的过程，双方不需要先建立连接

- 不可靠，不保证数据一定能到达对端，需要在应用层增加可靠性机制

  

## 3. 大小端

大小端指的是数据在内存中存放的一种方式

- 大端：低位存高地址，高位存低地址（低高高低）

- 小端：低位存低地址，高位存高地址（低低高高）

```
下面用 unsigned int num = 0x01020304 为例，进行说明

     低地址 --> 高地址
大端  01  02  03  04 
小端  04  03  02  01  
```

不同架构的电脑，存储方式可能不同。一般个人电脑都是小端存储，服务器都是大端存储

按照规定，网络上传输数据，**统一使用大端**，网络上传输的字节序称为**网络字节序**，本地存储的字节序称为**本地字节序**。

发送时，需要将本地字节序转换为网络字节序；接收时，需要将网络字节序转换为本地字节序

```mermaid
graph LR

A[主机A 本地字节序] --> |转网络字节序| B[网络]
B --> |转本地字节序| C[主机B 本地字节序]
```

## 4. 字节序转换

数据在网络上传输时，只要超过1个字节，就需要做字节序转换。1个字节的数据无需转换，因为1个字节没有大小端的问题

```c
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);
 # 将32位的主机字节序转换为网络字节序，一般用于Ip地址的字节序转换

uint16_t htons(uint16_t hostshort);
# 将16位的主机字节序转换为网络字节序，一般用于端口的字节序转换

uint32_t ntohl(uint32_t netlong);
# 将32位的网络字节序转换位主机字节序

uint16_t ntohs(uint16_t netshort);
# 将16位的网络字节序转换位主机字节序
```



## 5. 地址转换函数

```c
#include <arpa/inet.h>

int inet_pton(int af, const char *src, void *dst);
```

- 功能：将 ipv4地址的字符串形式，转换为32位的整数，转换后的整数为大端存储，即网络序；或者将ipv6地址的字符串形式，转换为128位的整数
- 参数：
  - af：协议族，ipv4 为 AF_INET，ipv6 为 AF_INET6
  - src：需要转换的字符串的首地址
  - dst：32位或128位的整数地址

- 返回值：成功返回1；如果要转换的字符串不是一个有效的ipv4/ipv6地址，返回0；如果af 协议族不是一个有效值，返回-1



```c
#include <arpa/inet.h>

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```

- 功能：将网络序的ip地址整数形式，转换为字符串形式

- 参数：

- 返回值：成功返回指向dst 的非空指针，失败返回NULL



## 6. socket

socket为了解决不同主机上的进程间通信

socket必须成对出现



# 二、UDP编程基础

## 1. C/S架构模型

![](https://note.youdao.com/yws/public/resource/620d2b0bad50ad1582add83f6580470a/xmlnote/AC259A6D387149A3920235A68C66D6F2/19659)





- 服务器必须绑定固定的 IP 和端口，否则客户端不知道该往哪儿发
- 客户端无需绑定本地IP和端口，需要指定服务端IP和端口，客户端发送时，本地端口由系统随机分配

## 2. 函数说明

### 2.1 socket()

```
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

- 功能：创建一个socket

- 参数：
  - domain：指定通信域，用于选择通信的协议族
    - AF_INET：用于ipv4通信
    - AF_INET6：用于ipv6通信
    - AF_UNIX，AF_LOCAL：用于本地通信
  - type：指定通信语义（指定创建 TCP / UDP / RAW socket)
    - SOCK_STREAM：流式套接字，用于TCP，提供面向连接、可靠、有序、全双工的比特流
    - SOCK_DGRAM：数据报套接字，用于UDP，无连接、不可靠
    - SOCK_RAW：原始套接字
  - protocol：指定socket使用的协议，通常为0

- 返回值：成功返回文件描述符；失败返回-1，并设置errno

### 2.2 bind()

```
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

- 功能：给socket 绑定IP和端口

- 参数：
  - sockfd：绑定的socket
  - addr：绑定的IP和端口。IP和端口为网络序，并需要将ipv4 / ipv6 套接字结构体转成通用套接字结构体
  - addrlen：ipv4 / ipv6 地址长度（字节数）

- 返回值：成功返回0；失败返回-1，并设置errno

**通用套接字结构体**

```
struct sockaddr 
{
	sa_family_t  sa_family;
    char         sa_data[14];
}
```



**ipv4 套接字结构体**

```
struct sockaddr_in 
{
	sa_family_t     sin_family; /* address family: AF_INET */
	in_port_t       sin_port;   /* port in network byte order */
	struct in_addr  sin_addr;   /* internet address */
};

/* Internet address. */
struct in_addr 
{
	uint32_t       s_addr;     /* address in network byte order */
};
```



**ipv6套接字结构体**

```
https://blog.csdn.net/euyy1029/article/details/107708365utm_medium=distribute.pc_releant.none-task-blog-baidulandingword-1&spm=1001.2101.3001.4242 
```



### 2.3 sendto()

```
#include <sys/types.h>
#include <sys/socket.h>

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
```

- 功能：通过socket 发送数据

- 参数：

  - sockfd：发送数据的 socket
  - buf：指向要发送数据的buf首地址
  - len：要发送的数据长度
  - flags：一般为0，可使用以下参数，具体用法参考man手册
    - MSG_CONFIRM
    - MSG_DONTROUTE
    - MSG_DONTWAIT
    - MSG_EOR
    - MSG_MORE
    - MSG_NOSIGNAL
    - MSG_OOB
  - dest_addr：目的地址
  - addrlen：目的地址长度

- 返回值：成功返回发送的字节数，失败返回-1，并设置errno

  

### 2.4 recvfrom()

```
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
```

- 功能：从socket 接收数据。如果接收的数据过长，Buf 不够，数据可能会被丢弃；如果没有消息可以接收，recvfrom会阻塞，除非该socket设置的非阻塞，那么此时会返回-1，并设置errno为EAGAIN 或 EWOULDBLOCK

- 参数：
  - sockfd：接收数据的sokcet
  - buf：存放数据的buf首地址
  - len：接收buf长度
  - flags：一般为0，可使用以下参数，具体用法参考man手册
    - MSG_CMSG_CLOEXEC
    - MSG_DONTWAIT
    - MSG_ERRQUEUE
    - MSG_OOB
    - MSG_PEEK
    - MSG_TRUNC
    - MSG_WAITALL
  - src_addr：如果底层协议提供消息的源地址，那么该源地址会被填入该缓冲区；如果不关心源地址，可以置为NULL
  - addrlen：该参数初始值需要设置为src_addr的长度，返回值为源地址实际长度。如果不关心源地址，可以置为NULL

- 返回值：成功返回接收到的字节数，失败返回 -1，并设置errno。如果为流式套接字，对端关闭时返回0；如果要接收的字节数为0，同样也会返回0



# 三、TCP编程基础

## 1. C/S架构模型

![](https://note.youdao.com/yws/public/resource/620d2b0bad50ad1582add83f6580470a/xmlnote/FEDD4F54F61241939FE05E700F0E3CEA/19661)



## 2. 函数说明

### 2.1 connect()

```c
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

- 功能：连接指定地址的服务器（通常，面向连接的socket只能使用一次，面向无连接的socket可以连接多次）
- 参数：
  - sockfd：创建的socket
  - addr：要连接的服务器地址
  - addrlen：服务器地址长度
- 返回值：成功返回0，失败返回-1，并设置errno



### 2.2 send()

```c
#include <sys/types.h>
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

- 功能：发送数据到已连接的对端socket
- 参数
  - sockfd：已连接的socket
  - buf：要发送的数据地址
  - len：要发送的数据长度
  - flags：一般为0，可使用以下参数，具体用法参考man手册
    - MSG_CONFIRM
    - MSG_DONTROUTE
    - MSG_DONTWAIT
    - MSG_EOR
    - MSG_MORE
    - MSG_NOSIGNAL
    - MSG_OOB
- 返回值：成功返回已发送的字节数，失败返回-1，并设置errno



### 2.3 recv()

```
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

- 功能：从已连接的socket中接收数据，默认阻塞，除非socket设置的非阻塞
- 参数 
  - sockfd：已连接的socket
  - buf：接收数据的地址
  - len：接收Buff的长度
  - flags：一般为0，可使用以下参数，具体用法参考man手册
    - MSG_CMSG_CLOEXEC
    - MSG_DONTWAIT
    - MSG_ERRQUEUE
    - MSG_OOB
    - MSG_PEEK
    - MSG_TRUNC
    - MSG_WAITALL
- 返回值：成功返回接收的字节数，失败返回-1，并设置errno。如果socket设置为非阻塞，没有收到消息，recv会返回-1，并设置errno为 EAGAIN or EWOULDBLOCK；如果对端关闭，会返回0

```
recv() 等价于 recvfrom(fd, buf, len, flags, NULL, 0);
```



### 2.4 listen()







### 2.5 accept()

















