## 1. 编程基础

### 1.1 网络开发模型

B/S模型（browser / server）：浏览器与服务器模型。对服务器要求高，计算均在服务端进行，但移植性好，不依赖具体平台

C/S模型（client / server ）：客户端与服务器模型

### 1.2 TCP 传输控制协议

特点：

- 面向连接，类似于打电话，需要先建立连接
- 稳定可靠，保证数据的安全传输

### 1.3 UDP 用户数据报协议

特点：

- 面向无连接，类似于写信，不需要先建立连接
- 不可靠，不保证数据一定能到达对端，可以在应用层加上可靠性机制

### 1.4 大小端

大端：低位存高地址，高位存低地址（低高高低）

小端：低位存低地址，高位存高地址（低低高高）

```
下面用 unsigned int num = 0x01020304 为例，进行说明

     低地址 --> 高地址
小端  04  03  02  01  
大端  01  02  03  04 
```

不同架构的电脑，存储数据的大小端可能不同。一般个人电脑都是小端存储，服务器都是大端存储

因此，规定网络上传输数据，**统一使用大端**，网络上传输的字节序称为**网络字节序**，本地存储的字节序称为**本地字节序**。

发送时，需要将本地字节序转换为网络字节序；接收时，需要将网络字节序转换为本地字节序

```mermaid
graph LR

A[主机A 主机字节序] --> |转网络字节序| B[网络]
B --> |转本地字节序| C[主机B 主机字节序]
```

### 1.5 字节序转换

用于网络传输数据时，只要超过1个字节，就需要做字节序转换。1个字节的数据无需转换，因为1个字节没有大小端的问题

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



### 1.6 地址转换函数

```c
#include <arpa/inet.h>

int inet_pton(int af, const char *src, void *dst);
```

功能：将 ipv4地址的字符串形式，转换为32位的整数，转换后的整数为大端存储，即网络序；或者将ipv6地址的字符串形式，转换为128位的整数

参数：

- af：协议族，ipv4 为 AF_INET，ipv6 为 AF_INET6
- src：需要转换的字符串的首地址
- dst：32位或128位的整数地址

返回值：成功返回1；如果要转换的字符串不是一个有效的ipv4/ipv6地址，返回0；如果af 协议族不是一个有效值，返回-1



```c
#include <arpa/inet.h>

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```

功能：将网络序的ip地址整数形式，转换为字符串形式

参数：

返回值：成功返回指向dst 的非空指针，失败返回NULL



### 1.7 socket

socket为了解决不同主机上的进程间通信

socket必须成对出现



## 2. UDP编程

### 2.1 C/S架构

<img src="C:\Users\machun\AppData\Roaming\Typora\typora-user-images\1596997224887.png" alt="1596997224887" style="zoom: 80%;" />



服务器必须绑定固定的 IP 和端口，否则客户端不知道该往哪儿发

客户端无需绑定IP和端口，客户端发送时，端口由系统随机分配



### 2.2 函数说明

#### 1. socket()

```
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

功能：创建一个socket

参数：

- domain：指定通信领域，用于选择通信的协议族
  - AF_UNIX，AF_LOCAL：用于本地通信
  - AF_INET：用于ipv4通信
  - AF_INET6：用于ipv6通信
- type：指定通信语义
  - SOCK_STREAM：流式套接字，用于TCP，提供有序、可靠、全双工、面向连接的比特流
  - SOCK_DGRAM：数据报套接字，用于UDP，无连接、不可靠
  - SOCK_RAW：原始套接字
- protocol：指定socket使用的协议，通常为0

返回：成功返回文件描述符；失败返回-1，并设置errno

#### 2. bind()

```
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

功能：给socket 分配一个地址（IP和端口）

参数：

- sockfd：要绑定的socket
- addr：绑定的地址，需要将ipv4 / ipv6 套接字结构体转成通用套接字结构体
- addrlen：ipv4 / ipv6 地址长度（字节数）

返回值：成功返回0，失败返回-1

#### 3. sendto()



#### 4. recvfrom()

#### 5. 套接字相关结构体

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

```



**通用套接字结构体**

```
struct sockaddr 
{
	sa_family_t  sa_family;
    char         sa_data[14];
}
```



 https://blog.csdn.net/euyy1029/article/details/107708365?utm_medium=distribute.pc_relevant.none-task-blog-baidulandingword-1&spm=1001.2101.3001.4242 

























