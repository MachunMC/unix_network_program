/*
 * @Author: machun Michael 
 * @Date: 2021-07-09 19:20:50 
 * @Last Modified by: machun Michael
 * @Last Modified time: 2021-07-09 20:20:53
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

enum CHECK_STATE
{
    CHECK_STATE_REQUEST_LINE = 0,  // 正在分析请求行
    CHECK_STATE_HEADER,            // 正在分析头部字段

    CHECK_STATE_CNT,
};

enum LINE_STATUS
{
    LINE_OK = 0,        // 读取到一个完整的行
    LINE_BAD,           // 行出错
    LINE_OPEN,          // 行数据不完整

    LINE_STATUS_CNT,
};

enum HTTP_CODE
{
    NO_REQUEST = 0,     // 请求不完整，需要继续读取客户数据
    GET_REQUEST,        // 获得了一个完整的客户请求
    BAD_REQUEST,        // 客户请求有语法错误
    FORBIDDEN_REQUEST,  // 客户对资源没有访问权限
    INTERNAL_ERROR,     // 服务器内部错误
    CLOSED_CONNECTION,  // 客户端已经关闭连接
};

// 为了简化问题，我们没有给客户端发送一个完整的HTTP应答报文，而只是根据服务器的
// 处理结果发送如下成功或失败信息
static const char * szret[] = 
{
    "I get a correct result\n",
    "Something wrong\n"
};

LINE_STATUS parse_line(char *buffer, int &checked_index, int &read_index)
{
    char temp;

    // checked_index指向buffer中当前正在分析的字节，read_index指向buffer中客户
    // 数据的尾部的下一个字节。buffer中第0 ~ checked_index字节都已分析完毕，第
    // checked_index ~ (read_index -1) 字节由下面的循环挨个分析
    for (; checked_index < read_index; ++checked_index)
    {
        // 获取当前要分析的字节
        temp = buffer[checked_index];

        // 如果当前的字节是'\r'，则说明可能读取到了一个完整行
        if (temp == '\r')
        {
            // 如果'\r'字符碰巧是目前buffer中的最后一个已经被读入的客户数据，
            // 那么这次分析没有读取到一个完整的行，返回LINE_OPEN用以表示还需要
            // 继续读取客户数据才能进一步分析
            if ((checked_index + 1) == read_index)
            {
                return LINE_OPEN;
            }
            // 如果下一个字符是'\n'，则说明成功读取到一个完整的行
            else if (buffer[checked_index + 1] == '\n')
            {
                buffer[checked_index++] == '\0';
                buffer[checked_index++] == '\0';
                return LINE_OK;
            }

            // 否则说明客户发送的HTTP请求存在语法问题
            return LINE_BAD;
        }
        else if (temp == '\n')
        {
            if ((checked_index > 1) && buffer[checked_index -1] == '\r')
            {
                buffer[checked_index -1] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
        }

        return LINE_BAD;
    }

    // 如果所有内容都分析完毕也没遇到'\r'字符，则返回LINE_OPEN，表示还需要继续读取客户数据才能进一步分析
    return LINE_OPEN;
}

// 分析请求行
HTTP_CODE parse_requestline(char *temp, CHECK_STATE &checkstate)
{
    // 如果请求行中没有空白字符或'\t'字符，则HTTP请求必有问题
    char *url = strpbrk(temp, "\t");
    if (!url)
    {
        return BAD_REQUEST;
    
    }
    *url++ = '\0';
    
    char *method = temp;
    if (strcasecmp(method, "GET") == 0) // 仅支持GET方法
    {
        printf("The request method is GET\n");
    }
    else
    {
        return BAD_REQUEST;
    }

    url += strspn(url, "\t");
    char *version = strpbrk(url, "\t");
    if (!version)
    {
        return BAD_REQUEST;
    }

    *version++ = '\0';
    version += strspn(version, "\t");
    
    //仅支持HTTP/1.1
    if (strcasecmp(version, "HTTP/1.1") != 0)
    {
        return BAD_REQUEST;
    }

    // 检查URL是否合法
    if (strncasecmp(url, "http://", 7) == 0)
    {
        url += 7;
        url = strchr(url, '/');
    }

    if (!url || url[0] != '/')
    {
        return BAD_REQUEST;
    }
    printf("The request URL is:%s\n", url);

    // HTTP请求行处理完毕，状态转移到头部字段的分析
    checkstate = CHECK_STATE_HEADER;
    return NO_REQUEST;
}


// 分析头部字段
HTTP_CODE parse_headers(char *temp)
{
    // 遇到一个空行，说明我们得到了一个正确的HTTP请求
    if (temp[0] == '\0')
    {
        return GET_REQUEST;
    }
    else if (strncasecmp(temp, "Host:", 5) == 0) // 处理"HOST"头部字段
    {
        temp += 5;
        temp += strspn(temp, "\t");
        printf("the request host is:%s\n", temp);
    }
    else // 其他头部字段都不处理
    {
        printf("I can not handle this header\n");
    }

    return NO_REQUEST;
}

// 分析HTTP请求的入口函数
HTTP_CODE parse_content(char *buffer, int& checked_index, CHECK_STATE&
                        checkstate, int &read_index, int &start_line)
{
    LINE_STATUS linestatus = LINE_OK;
    HTTP_CODE retcode = NO_REQUEST;

    // 主状态机，用于从buffer中取出所有完整的行
    while((linestatus = parse_line(buffer, checked_index, read_index)) == LINE_OK)
    {
        char *temp = buffer + start_line;
        start_line = checked_index;
        
        // checkstate记录主状态机当前的状态
        switch (checkstate)
        {
            case CHECK_STATE_REQUEST_LINE:
            {
                retcode = parse_requestline(temp, checkstate);
                if (retcode == BAD_REQUEST)
                {
                    return BAD_REQUEST;
                }
                break;
            }
            case CHECK_STATE_HEADER:
            {
                retcode = parse_headers(temp);
                if (retcode == BAD_REQUEST)
                {
                    return BAD_REQUEST;
                }
                else if (retcode == GET_REQUEST)
                {
                    return GET_REQUEST;
                }
                break;
            }
            default:
            {
                return INTERNAL_ERROR;
            }
        }
    }

    // 若没有读取到一个完整的行，则表示还需要继续读取客户数据才能进一步分析
    if (linestatus == LINE_OPEN)
    {
        return NO_REQUEST;
    }
    else
    {
        return BAD_REQUEST;
    }
}

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    int ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);
    
    ret = listen(listenfd, 5);
    assert(ret != -1);

    struct sockaddr_in client_address;
    socklen_t client_addrlenth = sizeof(client_address);
    int fd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlenth);
    if (fd < 0)
    {
        printf("errno is :%d\n", errno);
    }
    else
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));
        int data_read = 0;
        int read_index = 0;
        int checked_index = 0;
        int start_line = 0;
        CHECK_STATE checkstate = CHECK_STATE_REQUEST_LINE;

        while(1)
        {
            data_read = recv(fd, buffer + read_index, BUFFER_SIZE - read_index, 0);
            if (-1 == data_read)
            {
                printf("reading failed\n");
                break;
            }
            else if (data_read == 0)
            {
                printf("remote client has closed the connection\n");
                break;
            }
            read_index += data_read;
            HTTP_CODE result = parse_content(buffer, checked_index, checkstate, 
                                            read_index, start_line);
            if (result == NO_REQUEST)
            {
                continue;
            }
            else if (result == GET_REQUEST)
            {
                send(fd, szret[0], strlen(szret[0]), 0);
                break;
            }
            else
            {
                send(fd, szret[1], strlen(szret[0]), 0);
                break;
            }
        }
        close(fd);
    }
    close(listenfd);
    return 0;
}