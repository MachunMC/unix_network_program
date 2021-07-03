# 参数书籍

- 《Linux高性能网络编程》

# 疑问

- [ ] TCP有发送接收缓冲区，UDP有吗？

  

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


## 3. 字节序

字节序指的是数据在内存中存放的一种方式，分为大端字节序和小端字节序

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

- 功能：将 ipv4地址的字符串形式，转换为32位的整数，转换后的整数网络序；或者将ipv6地址的字符串形式，转换为128位的整数
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

## 7. 地址结构体

**通用套接字结构体**

```c
struct sockaddr 
{
	sa_family_t  sa_family;
    char         sa_data[14];
}
```



**ipv4 套接字结构体**

```c
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



# 二、UDP编程基础

## 1. C/S架构模型

<img src="https://note.youdao.com/yws/public/resource/620d2b0bad50ad1582add83f6580470a/xmlnote/AC259A6D387149A3920235A68C66D6F2/19659" style="zoom: 67%;" />





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
  - domain：指定地址族
    - AF_INET：用于ipv4通信
    - AF_INET6：用于ipv6通信
    - AF_UNIX，AF_LOCAL：用于本地通信
  - type：指定服务类型
    - SOCK_STREAM：流式套接字，用于TCP，提供面向连接、可靠、有序、全双工的比特流
    - SOCK_DGRAM：数据报套接字，用于UDP，无连接、不可靠
    - SOCK_RAW：原始套接字
  - protocol：指定socket使用的协议，通常用0表示使用默认协议
- 返回值：成功返回文件描述符；失败返回-1，并设置errno

**注意**：创建socket的时候，系统会为该socket分配两块缓冲区，一个用于读，一个用于写

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

## 3. UDP协议格式



# 三、TCP编程基础

## 1. C/S架构模型

**注意**：下图中的connect到accept这条线画的不对，connect箭头应该指向listen下面

![](https://note.youdao.com/yws/public/resource/620d2b0bad50ad1582add83f6580470a/xmlnote/FEDD4F54F61241939FE05E700F0E3CEA/19661)



## 2. TCP服务器处理流程

![](https://note.youdao.com/yws/public/resource/620d2b0bad50ad1582add83f6580470a/xmlnote/2F9E23982F6246399D5FD9F57D5C8981/24731)

- 监听套接字：只负责和客户端建立连接，不进行数据通信
- 已连接套接字：和客户端进行数据通信

## 3. 函数说明

### 3.1 connect()

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

### 3.2 send()

```c
#include <sys/types.h>
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

- 功能：发送数据到已连接的对端socket
- 参数
  - sockfd：已连接的socket

  - buf：要发送的数据Buffer

  - len：要发送的数据长度

  - flags：一般为0，可使用以下参数，具体用法参考man手册

    ![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/18B617F1A03340C38F09141454D848D1/27317)
- 返回值：成功返回已发送的字节数，失败返回-1，并设置errno

### 3.3 recv()

```
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

- 功能：从已连接的socket中接收数据，默认阻塞，除非socket设置为非阻塞
- 参数 
  - sockfd：已连接的socket
  - buf：接收数据的地址
  - len：接收Buff的长度
  - flags：一般为0，具体用法参考man手册（同send）
- 返回值：成功返回接收的字节数，失败返回-1，并设置errno。如果socket设置为非阻塞，没有收到消息，recv会返回-1，并设置errno为 EAGAIN or EWOULDBLOCK；如果对端关闭，会返回0

```
recv() 等价于 recvfrom(fd, buf, len, flags, NULL, 0);
```

### 3.4 listen(）

​		**监听的时候，套接字会由主动变成被动，同时会创建两个队列，一个是已完成连接队列，一个是未完成连接队列。当客户端connect连接过来，先加入到未完成连接队列，当三次握手完成后，会加入到已完成连接队列。**

```c
#include <sys/types.h>          
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

- 功能：将套接字由主动变为被动，并创建两个连接队列（已完成连接队列和未完成连接队列）
- 参数：
  - sockfd：
  - backlog：两个队列长度总和的最大值，现在一般为128
- 返回值：成功返回0，失败返回-1，并设置error

### 3.5 accept()

```c
#include <sys/types.h>    
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

- 功能：从已完成连接队列中，提取新的连接。同时创建一个已连接套接字，和当前这个客户端进行通信。**默认是阻塞的**
- 参数：

  - sockfd：监听套接字
  - addr：保存客户端地址的结构体
  - addrlen：结构体长度
- 返回值：成功返回已连接套接字，失败返回-1

### 3.6 close()

```c
#include <unistd.h>

int close(int fd);
```

- 功能：关闭文件描述符。close并非总是立即关闭一个连接，而是将fd的引用计数减1，当fd的引用计数为0时，才会关闭连接
- 参数：需要关闭的文件描述符
- 返回值：成功返回0，失败返回-1

### 3.7 shutdown()

```c
#include <sys/socket.h>

int shutdown(int sockfd, int how);
```

- 功能：立即关闭连接。相对于close，shutdown是专门为网络编程设计的
- 参数：
  - sockfd：需要关闭的socket fd
  - how
    - SHUT_RD：关闭socket上的读功能。上层应用无法对该socket执行读操作，并且socket接收缓冲区中的数据会被丢弃
    - SHUT_WR：关于socket上的写功能。socket发送缓冲区中的数据，会在连接关闭前全部发送出去，上层应用无法对该socket执行写操作。这种状态下，连接处于半关闭状态
    - SHUT_RDWR：关闭socket上的读写功能
- 返回值：成功返回0，失败返回

### 3.8 getsockname()

```c
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

- 功能：获取socket对应本地的ip和端口信息
- 参数：
- 返回值：成功返回0，失败返回-1

### 3.9 getpeername()

```c
#include <sys/socket.h>

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

- 功能：获取socket对应的对端的ip和端口信息
- 参数：
- 返回值：成功返回0，失败返回-1

### 3.10 getsockopt()

```c
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
```

- 功能：获取socket选项
- 参数：
  - sockfd：要设置的socket fd
  - level：指定要操作哪个协议的选项
  - optname：指定选项的名字
  - optval：保存选项的值
  - optlen：选项的长度
- 返回值：成功返回0，失败返回-1

![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/7898B29AF69A4160BF2779A99C670685/27334)

### 3.11 setsockopt()

```c
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

- 功能：设置socket选项
- 参数：同上
- 返回值：成功返回0，失败返回-1

# 四、TCP协议

## 1. 简介

​		**TCP提供一种面向连接的、可靠的、字节流的服务**。通信双方需要先建立连接，才能交换数据。TCP仅适用于双方通信，多播和广播不能用TCP。

​		TCP通过下列方式提供可靠性。

- 数据被切割成TCP认为最合适发送的数据段
- 当发送一个TCP报文段后，发送端会启动一个定时器，等待对端确认接收到该报文段。如果没有及时收到确认，会重发该报文段
- 当接收端收到对端的TCP报文段后，会发送一个确认（ACK）。**一般会推迟几分之一秒发送，不知道为啥？**
- 首部和数据校验和，检测数据在传输过程中没有出现差错。有错误就不确认，等待对端重传
- IP数据报不保证顺序，TCP会对收到的数据进行排序，将收到的数据按照正确的顺序给到应用层。
- TCP会丢弃重复的数据
- TCP提供流量控制



如何理解TCP提供**字节流**的服务？

​		发送端连续多次执行发送操作时，这些数据会先放到TCP发送缓冲区中。当TCP协议栈真正开始发送数据时，发送缓冲区中等待发送的数据，会被封装成一个或多个TCP报文段发出。所以，TCP协议栈发送的TCP报文段数量，和应用程序执行的发送操作数量不一定是一一对应的关系。

​		接收端协议栈收到一个或多个TCP报文段后，将应用程序数据按照TCP报文段的序号，依次放到TCP接收缓冲区中，然后通知上层应用来读数据。上层应用可以一次将缓冲区的数据全部读完，也可分多次读取，这取决于上层应用接收缓冲区的大小。所以，应用程序执行的读操作次数，和TCP协议栈收到的报文段个数也不一定是一一对应的关系。

​		综上所述，发送端执行的写操作次数，和接收端执行的读操作次数，没有任何数量对应你关系。也就是说，应用程序对数据的发送和接收没有边界限制。

![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/A798D9D6A0BA400FB56C7DAFE38180BE/27214)



​		UDP则不然。UDP提供**数据报**服务，发送端上层应用，每执行一次发送，UDP协议栈就将其封装成一个UDP数据报并发送出去。接收端必须及时执行接收操作，否则就会丢包。并且，如果上层应用没有指定足够的缓冲区，则UDP数据会被截断。

![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/8E5CA2B3D5EE4CDF8B3BBB1A0F269773/27216)



## 2. 协议格式

TCP固定包头是20个字节（不加选项）

![](https://note.youdao.com/yws/public/resource/620d2b0bad50ad1582add83f6580470a/xmlnote/8E7078B0A252493DB66F200196C85815/24733)



- 源端口、目的端口（16位）：端口用于区分数据是进程发送的、或者要给到哪个进程。客户端一般使用系统自动分配的端口
- **序列号（32位）**：
- **确认序列号（32位）**：
- 首部长度（4位）：首部中32bit的数量，所以首部长度需要用该值 * 4，最多60字节
- 6个保留位：
- 6个标志位：
  - SYN：发起连接
  - ACK：表示确认
  - PSH：接收方应该尽快将这个报文段交给应用层（表示推送数据）
  - FIN：断开连接
  - RST：重新建立连接
  - URG：紧急指针有效
- **窗口大小（16位）**：用于流量控制。告诉对端，我的TCP接收缓冲区还可以接收多少字节的数据
- 校验和（16位）：用于校验整个TCP报文段在传输过程中有没有出现异常。由发送端计算，接收端校验
- 紧急指针（16位）：URG标志位置1时有效
- 选项：可选
- 数据：可选。一个TCP报文段可以只包括TCP首部，TCP建立连接和终止连接过程中交换的报文段，只包含TCP首部



关于**序列号**和**确认序列号**

 - 二者都是随机值，wireshark显示Seq从0开始，实际上是经过处理的，可以看到下面有一个Sequence Number (raw)
 - **序列号**表示当前这个包的序号
 - **确认序列号**
    - 用于确认收到对方的报文
    - 希望下一次对方报文的序列号为我的确认序列号，这样可以确认对端收到了我的这条报文
    - 确认序列号 = 收到对方的报文（序号 + SYN标志 ( 0 / 1 ) + 报文数据的长度）



![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/8B40BD02E5E34D2E872EAD861687DF84/26773)



## 3. 三次握手

- TCP的三次握手，对应的是tcp connect请求。调用connect函数，底层协议栈会和对端进行三次握手
- 三次握手成功后，服务端会将该连接由**未完成连接队列**加入到**已完成连接队列**（该队列是listen的时候创建），然后accept会从**已完成连接队列**中提取连接，并创建一个已连接套接字，和这个客户端进行通信
- 如果server收到SYN，并且也回复了SYN + ACK，但始终收不到client的ACK，久而久之，server的未完成连接队列就会满，进而无法处理新的连接，这就是以前的**SYN攻击**，现在应该有机制来处理这种问题

<img src="https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/E6E8EF8754D74A918F92E3E3E46A7538/26765" style="zoom:80%;" />

**第一次握手**

客户端请求和服务器建立连接，发送SYN请求，TCP报文Flags字段中，SYN位置1

![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/208949C7DE96492BAE4C0CD0E58C2915/26767)



**第二次握手**

服务端也请求和客户端建立连接，所以也会发送一个SYN请求。同时，服务端会回复客户端的SYN，所以同时会回复一个ACK

![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/EC2962BFC6A8431FACB4E3903A566AD0/26769)



**第三次握手**

客户端对服务端的SYN进行回复，回复一个ACK

![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/E65D363AE8184492B2CEBCB6491366AA/26771)



**TCP握手为什么是三次？两次行不行？**

答：不行，因为第二次握手时，server也向client发起了连接请求，server需要明确client收到了请求，所以需要第三条信令，让server知道client收到了请求，并且准备好了。

​		如果只有两次握手，如果第二条信令丢失，则server 认为握手已经完成，而client则因为没有收到server的确认，认为握手失败，就导致两边的状态不一致。



## 4. 四次挥手

- 四次挥手发生在，客户端主动调用close关闭连接（**当然也有可能是服务端主动关闭连接**）
- 客户端调用close，服务端会收到长度为0的数据，表示对端请求断开连接，此时服务端也应该调用close断开连接
- 抓包有时候会看到，服务器的ACK和FIN是一起发过去的，这是因为TCP有一个原则是，尽可能携带更多数据，此时四次挥手就变成了三次挥手
- 客户端调用close后，协议栈并没有关闭，还在等待对方的确认。当收到对方的FIN，并回复ACK后，协议栈并不会立即关闭，还会等待一段时间，因为回复的ACK中途可能丢了，然后服务器收不到确认还会重传。客户端等待的时间为2MSL，谁先主动关闭，谁就需要等待



![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/B3486701C9BC4984B7D5E91574772CE2/26777)



**第一次挥手**

客户端主动调用close，协议栈会发送一个标志位为FIN的数据包，请求断开连接

**第二次挥手**

服务器协议栈收到FIN后，会立即回复一个ACK

**第三次挥手**

服务端recv会返回0，表示对端请求关闭，此时服务端也应该调用close关闭连接，对应的协议栈会发送FIN请求

**第四次挥手**

客户端协议栈收到FIN请求，会回复ACK



![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/9C684C883F3B4DB2913F42DDEE1769A5/26779)



- 这里第二次挥手和第三次挥手，合并成一次了，所以抓包显示3条
- 第一条除了FIN，还有一个ACK，ACK可能是上次请求的回复，合并到一起



## 5. TCP状态转换图

要求能够了解三次握手和四次挥手，对应的状态及转换

![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/7C23447128E24529B5C7A566B71F120E/27218)

对应到 TCP建立连接和断开连接

<img src="https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/020F4C2C4C6E45F3A3002F94ACFFF958/26851" style="zoom:80%;" />



## 6. 半关闭状态

​		主动关闭的一端，调用close后，收到对端的ACK，但是没有收到对端的FIN，此时就处于半关闭状态，对应上图中的FIN_WAIT_2 状态。**这种状态下，应用层还可以接收数据，但无法发送数据。**

​		如果收到对端的FIN，并回复ACK，就会从TIME_WAIT状态变成完全关闭状态

## 7. 端口复用

TCP socket主动关闭的一端，需要经过 2MSL时间，socket才会完全关闭。

所以在这段时间内，如果绑定这个端口，bind会报address already in use的错误。为了解决这个问题，可以设置端口复用，使这个端口能够**重新**被使用，bind的时候不会报错。

当新的进程绑定了这个端口，则之前的进程就无法再使用这个端口，因为一个端口只能标识一个进程

```c
int opt = 1; // 1表示开启端口复用

setsockopt(nsockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

## 8. 滑动窗口

滑动窗口用于告诉对端，我的接收缓冲区剩余的空间大小，对应wireshark抓包中的win字段。

滑动窗口的作用是，通信两端的处理速度不一致，如果发送端的发送速度比较快，而接收端的处理速度比较慢，就会导致**接收端的接收缓冲区**满。如果发送端再发送数据，就会导致数据被丢弃。

使用滑动窗口，可以告诉对端我的接收缓冲区还剩多少，此时发送端就可以根据该值来控制发送速度。所以**滑动窗口用于进行流量控制**

滑动窗口站16个字节，最大65535

![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/73FA2F18E9B14DB693E779027484D773/26800)

## 9. 拥塞控制

拥塞控制包含4个部分

- 慢启动（slow start）
- 拥塞避免（congestion avoidance）
- 快速重传（fast retransmit）
- 快速恢复（fast recovery）

## 10. 复位报文段（RST）

某些特殊情况下，TCP连接的一端会往另一端发送携带RST的复位报文段，通知对方关闭连接或重新建立连接。常见的有3种情况

- 访问的端口不存在，会返回RST复位报文段
- 一端异常终止连接，会发送RST复位报文段
- 往半打开状态下的一端发送数据，会返回RST复位报文段

## 11. IO复用之select

IO复用，只用一个进程或线程，来实现多路并发。其本质上是内核监听文件描述符的读写缓冲区，是否可读或可写。

**优点：**

- 跨平台

**缺点：**

- 有最大1024个文件描述符的限制（可以修改宏FD_SETSIZE，但需要重新编译内核，比较麻烦）
- 每次重新监听，需要重新添加文件描述符（将需要监听的文件描述符集合，从用户态拷贝到内核态）
- 如果同时有大量连接，但只有少部分文件描述符活跃，由于需要遍历文件描述符是否在集合内，所以会导致效率很低



```c
/* According to POSIX.1-2001, POSIX.1-2008 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

struct timeval 
{
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* microseconds */
};
```

- 功能：监听多个文件描述符读写属性的变化（也就是读写缓冲区的变化）。**当select返回时，会修改对应的文件描述符集合**
- 参数：
  - nfds：监听的最大文件描述符 + 1
  - readfds：需要监听的读属性变化的文件描述符集合
  - writefds：需要监听的写属性变化的文件描述符集合
  - exceptfds：异常的文件描述符集合
  - timeout：超时时间。NULL表示阻塞，一直监听
- 返回值：成功返回属性变化的文件描述符个数，失败返回-1，超时返回0



**文件描述符操作函数**

```c
void FD_CLR(int fd, fd_set *set);
```

功能：将文件描述符fd从集合中清除



```c
int  FD_ISSET(int fd, fd_set *set);
```

功能：判断文件描述符fd是否在集合中。在返回非0，不在返回0



```c
void FD_SET(int fd, fd_set *set);
```

功能：将文件描述符fd添加到集合中



```c
void FD_ZERO(fd_set *set);
```

功能：清空文件描述符



## 12. IO复用之epoll

epoll的底层实现，是**红黑树**

**优点：**

- 没有文件描述符限制（修改ulimit限制即可）
- 每次重新监听，不需要重新添加文件描述符
- 监听到文件描述符变化，会返回变化的文件描述符（不需要遍历查找变化的文件描述符）

### 12.1 epoll_create

```c
#include <sys/epoll.h>

int epoll_create(int size);
```

- 功能：创建一个epoll句柄
- 参数：epoll监听的数量，大于0即可。Since Linux 2.6.8, the size argument is ignored, but must be greater than zero
- 返回值：成功返回epoll句柄，失败返回-1

### 12.2 epoll_ctl

```c
#include <sys/epoll.h>

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

typedef union epoll_data {
	void        *ptr;
    int          fd;
    uint32_t     u32;
    uint64_t     u64;
} epoll_data_t;

struct epoll_event {
	uint32_t     events;      /* Epoll events */
    epoll_data_t data;        /* User data variable */
};

events：
    - EPOLLIN：文件描述符是否可读
	- EPOLLOUT：文件描述符是否可写
    - 其他
```

- 功能：注册需要监听的文件描述符和对应的事件
- 参数：
  - epfd：epoll句柄，epoll_create创建的文件描述符
  - op：文件描述符操作
    - EPOLL_CTL_ADD：**添加**需要监听的文件描述符和对应的事件
    - EPOLL_CTL_DEL：**删除**需要监听的文件描述符和对应的事件
    - EPOLL_CTL_MOD：**修改**需要监听的文件描述符和对应的事件
  - fd：需要监听的文件描述符
  - event：需要监听的fd的事件类型
    - events：监听的事件类型。**按位操作**，可以使用按位与监听多个事件
    - data：用户数据，可以用来传递文件描述符或其他需要传递的信息
- 返回值：成功返回0，失败返回-1

### 12.3 epoll_wait

```c
#include <sys/epoll.h>

int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);
```

- 功能：监听文件描述符事件变化
- 参数
  - epfd：epoll句柄
  - events：返回的事件，需要传递结构体数组指针
  - maxevents：数组长度
  - timeout：超时时间，单位ms。-1表示一直等待，0表示不阻塞
- 返回值：变化的文件描述符个数。0表示超时时间到，没有文件描述符有事件变化；-1表示出错

### 12.4 水平触发和边沿触发

epoll_wait有两种触发方式，一个是水平触发，一个是边沿触发。

**读缓冲区**

- 水平触发（EPOLLLT）：epoll_wait默认的触发方式，读缓冲区只要有数据就会触发。如果缓冲区的数据没有读完，会一直触发
- 边沿触发（EPOLLET）：读缓冲区收到数据就会触发。只有收到数据后才会触发，触发后如果数据没有读完，在没有收到新数据之前，不会触发。所以**如果设置了边沿触发，一定要保证数据能读完**

**写缓冲区**

- 水平触发：只要写缓冲区没有满，就会一直触发。这种触发方式不好
- 边沿触发：只要写缓冲区的数据发送一次，就会触发一次



读缓冲区设置为水平触发和边沿触发都可以，但设置为边沿触发，可以减少内核态向用户态的切换，可以提高效率，只要保证数据可以读完就可以。但写缓冲区如果设置为水平触发，就会一直触发，这样不好，所以要设置为边沿触发。

**所以，如果只监听读事件，可以使用默认的水平触发；如果除了监听读事件，还要监听写事件，要设置为边沿触发**



```c
tEpollEvent.events = EPOLLIN | EPOLLET; // EPOLLET表示设置为边沿触发
tEpollEvent.data.fd = nListenFd;
epoll_ctl(nEpollFd, EPOLL_CTL_ADD, nListenFd, &tEpollEvent);
```



**如何保证将数据读完？**

循环读，直到缓冲区没有数据。但是recv是阻塞的，所以需要将fd设置成非阻塞，当recv返回-1，且errno为 EAGAIN或EWOULDBLOCK，表示缓冲区暂时没有数据可读

```
// 设置fd为非阻塞
int flag = fcntl(fd, F_GETFL);
flag |= O_NONBLOCK;
fcntl(fd, F_SETFL, flag);
```



<img src="https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/249DDF72D9FC453DAA586DCA1F50FF66/26916" style="zoom:80%;" />

### 12.5 epoll + 线程池

一个线程中，while循环处理epoll_wait的事件，会有一个问题。如果某些处理比较耗时，就会导致epoll_wait处理不及时，影响其他事件的处理。一般采用 epoll + 线程池的方式来解决这个问题

# 五、高级IO函数

## 1. pipe()

```c
#include <unistd.h>

int pipe(int fd[2]);
```

- 功能：创建一根管道，用于进程间通信
- 参数：包含两个int型变量的数组首地址
- 返回值：成功返回0，失败返回-1

创建成功后，fd[0]和fd[1]两个文件描述符，分别表示管道的两端，fd[0]用于读，fd[1]用于写，从fd[1]写入的数据可以从fd[0]读取。如果要实现双向管道，需要创建两根管道。默认情况下，两个文件描述符都是阻塞的

## 2. dup()和dup2()

```c
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);
```









# 六、UDP协议

数据报套接字

# 七、IP协议

## 1. 简介

IP协议提供一个无状态、无连接、不可靠的服务。主要的功能是路由

## 2. 协议格式

<img src="https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/11CEC110472F401FBB165304A688D5D3/27181"  />

- 版本号：指定IP协议版本
- 头部长度：标识 IP头部有多少个32bit位，所以IP头最大60字节
- 服务类型（TOS）：3位优先权，4位TOS，一个保留位。4个TOS位分别为：最小延时，最大吞吐量，最高可靠性，最小费用
- 总长度：整个IP数据报的总长度，单位是字节，因此IP数据报最大长度65535字节。但由于MTU的限制，超过MTU的数据报会分片传输
- 标识：标识主机发送的每一个数据报。初始值由系统随机生成，后面每次发送一个数据报，就加1。同一个数据报的所有分片，该标识字段相同
- 标志：第1位保留，第2位表示禁止分片，第3位表示更多分片
- 片偏移：分片相对于原始IP数据报开始处的偏移
- 生存时间（TTL）：数据报达到目的地前，最大可经过的路由器的跳数。每经过一个路由器，值减1
- 协议：区分上层协议，TCP为6，UDP为17
- 头部校验和：接收端检测**头部**在传输过程中是否损坏
- 源IP
- 目的IP
- 可选项

## 3. 分片

IP数据报超过MTU时，会分片传输。接收端通过标识、标志、片偏移三个字段可以重组IP数据报。

以太网的MTU是1500，一个长度为1481字节的IP数据报，会被拆分成两个数据报发送，两个数据报的标识字段相同。第一个数据报，标志位设置更多分片，长度为20个字节的IP头部，加上1480个字节的数据部分；第二个数据报，标志位设置为禁止分片，长度为20个字节的IP头部，加上1字节的数据部分。

## 4. 路由

IP层根据路由表，来决定数据如何转发。

下图为路由表

![](https://note.youdao.com/yws/public/resource/a66685a4842f56c1ad2c2aaf50a39424/xmlnote/BD207DFAFD4A48368D6847D6CD0D9538/27200)



具体转发机制如下：

- 查找路由表，有没有和目标IP地址完全匹配的地址。如果找到，则按照这一条路由进行转发；没找到，则进行下个步骤
- 查找路由表，有没有和目标IP地址处于同一个网段的地址。如果找到，则按照这一条路由进行转发；没找到，则进行下个步骤
- 选择默认路由，将数据转发给网关

## 5. IP层收到数据后如何处理？

# 八、原始套接字

## 1. 简介

TCP和UDP套接字，只能收发应用层数据，收到的数据不包含传输层、网络层、数据链路层的协议头。如果想要修改源、目的IP或者mac地址，TCP和UDP套接字就无能为力了。

原始套接字（SOCK_RAW），可以接收数据链路层的所有数据帧，可以自己组装各种协议头。一般可以用于抓包和网络流量分析。

## 2. 创建原始套接字

```c
#include <sys/socket.h>
#include <linux/if_ether.h>

int socket(PF_PACKET, SOCK_RAW, protocol);
```

- 功能：创建链路层的原始套接字
- 参数：
  - protocol：指定收发的数据包类型
    - ETH_P_IP：IPv4数据包
    - ETH_P_ARP：ARP数据包
    - ETH_P_ALL：任何协议类型的数据包
- 返回值：成功返回套接字fd，失败返回-1



一般这样使用

```c
#include <sys/socket.h>
#include <linux/if_ether.h>
int sock_fd = 0;

sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
```

接收使用recvfrom，发送使用

# 九、其他

## 1. TCP socket缓冲区大小

系统版本信息：centos 7

```shell
[machun@localhost ipv4]$ cat /proc/version 
Linux version 3.10.0-1127.el7.x86_64 (mockbuild@kbuilder.bsys.centos.org) (gcc version 4.8.5 20150623 (Red Hat 4.8.5-39) (GCC) ) #1 SMP Tue Mar 31 23:36:51 UTC 2020
```

### 1.1 通过/proc查看socket缓冲区大小

查看TCP发送、接收缓冲区大小

```shell
[machun@localhost ipv4]$ cat /proc/sys/net/ipv4/tcp_wmem 
4096	16384	4194304   // 默认值为16384
[machun@localhost ipv4]$ cat /proc/sys/net/ipv4/tcp_rmem 
4096	87380	6291456   // 默认值为87380
```

### 1.2 getsockopt获取缓冲区大小

通过如下代码，可以获取tcp socket读写缓冲区大小

```c
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

dwLen = sizeof(nRcvBufLen);
nRet = getsockopt(nTcpSockFd, SOL_SOCKET, SO_RCVBUF, &nRcvBufLen, &dwLen);

dwLen = sizeof(nSndBufLen);
nRet = getsockopt(nTcpSockFd, SOL_SOCKET, SO_SNDBUF, &nSndBufLen, &dwLen);
```

运行结果如下：

```
getsockopt tcp socket SO_RCVBUF succ, nRcvBufLen:87380
getsockopt tcp socket SO_SNDBUF succ, nSndBufLen:16384
```

可以通过setsockopt函数，设置socket发送、接收缓冲区大小，但实际生效的值，是我们设置的2倍

## 2. TCP socket缓冲区低水位标志

TCP socket缓冲区低水位标志，用于IO复用时，判断socket是否可读可写。当TCP socket接收缓冲区中的可读数据量大于其低水位时，IO复用接口将通知上层应用可以从socket读取数据；当TCP socket发送缓冲区中的空闲空间（可写入的空间）大于其低水位时，IO复用接口将通知上层应用可以往socket写入数据。

默认情况下，TCP发送、接收缓冲区低水位标志，均为1字节

```c
dwLen = sizeof(nRcvLowFlag);
nRet = getsockopt(nTcpSockFd, SOL_SOCKET, SO_RCVLOWAT, &nRcvLowFlag, &dwLen); // TCP接收缓冲区低水位标志
    
dwLen = sizeof(nSndLowFlag);
nRet = getsockopt(nTcpSockFd, SOL_SOCKET, SO_SNDLOWAT, &nSndLowFlag, &dwLen); // TCP发送缓冲区低水位标志
```

打印如下

```
getsockopt tcp socket SO_RCVLOWAT succ, nRcvLowFlag:1
getsockopt tcp socket SO_SNDLOWAT succ, nSndLowFlag:1
```

