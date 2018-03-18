#ifndef HTTPCONNECTION_H_
#define HTTPCONNECTION_H_

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include "14-2-locker.h"

class http_conn
{
public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;

    enum MEHTOD {GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH};

    enum CHECK_STATE {CHECK_STATE_REQUESTLINE = 0,
                      CHECK_STATE_HEADER,
                      CHECK_STATE_CONTENT };
    enum HTTP_CODE {NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE,
                    FORBIDDEN_REQUEST, FILE_REQUEST, 
                    INTERNAL_ERROR, CLOSED_CONNECTION };

    enum LINE_STATUS {LINE_OK = 0, LINE_BAD, LINE_OPEN};

public:
    http_conn(){}
    ~http_conn(){}

public:
    //initial new accept connection
    void init(int sockfd, const sockaddr_in &addr);
    //close connection
    void close_conn(bool real_close = true);
    
    void process();
    
    //nonblock read 
    bool read();
    
    //nonblock write
    bool write();
    
private:
    void init();
    
    //parse http request
    HTTP_CODE process_read();
    
    //fill http response
    bool process_write(HTTP_CODE ret);

    //following functions called by process_read to parse http request
    HTTP_CODE parse_request_line(char * text);
    HTTP_CODE parse_headers(char * text);
    HTTP_CODE parse_content(char * text);
    HTTP_CODE do_request();
    char * get_line() {return m_read_buf + m_start_line; }
    LINE_STATUS parse_line();

    //following functions called by process_write to fill http response
    void unmap();
    bool add_response(const char * format, ...);
    bool add_content(const char * content);
    bool add_status_line(int status, const char * title);
    bool add_headers(int content_length);
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();

public:
    static int m_epollfd;

    static int m_user_count;

private:
    int m_sockfd;
    sockaddr_in m_address;

    char m_read_buf[READ_BUFFER_SIZE];
   
    //标识读缓冲去已经读入的客户数据的最后一个字节的下一个位置
    int m_read_idx;
    //当前正在分析的字符在读缓冲区中的位置
    int m_checked_idx;
    //当前正在解析的行的起始位置
    int m_start_line;
    //写缓冲区
    char m_write_buf[WRITE_BUFFER_SIZE];
    //写缓冲去中待发送的字节数
    int m_write_idx;
    
    //主状态机当前所处的状态
    CHECK_STATE m_check_state;
    //请求方法
    MEHTOD m_method;

    //客户请求的目标文件的完整路径，其内容等于doc_root + m_url,
    //doc_root是网站根目录
    char m_real_file[FILENAME_LEN];
    //客户请求的目标文件的文件名
    char *m_url;

    //HTTP 协议版本号， 我们仅支持HTTP/1.1
    char* m_version;
    char* m_host;   //主机名
    
    int m_content_length; //http请求的消息体的长度
    bool m_linger;        //请求是否要求保持连接

    //客户请求的目标文件被mmap到内存中的起始位置
    char * m_file_address;
    
    //目标文件的状态，
    //通过它我们可以判断文件是否存在、是否为目录、是否可读，并获取文件大小等信息
    struct stat m_file_stat;

    //我们将采用writev来执行写操作，所以定义下面两个成员，其中m_iv_count表示被写内存块的数量
    struct iovec m_iv[2];
    int m_iv_count;
    
};

#endif
