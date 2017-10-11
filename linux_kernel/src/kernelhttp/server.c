#include <linux/in.h>  
#include <linux/inet.h>  
#include <linux/socket.h>  
#include <net/sock.h>  
#include <linux/string.h>
#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/posix_types.h>
#include <uapi/linux/eventpoll.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/workqueue.h> 
#include <linux/slab.h> 
#include <linux/sched.h>
#include <linux/kmod.h>

struct socket *server_sock;
static struct workqueue_struct *my_wq;

struct work_struct_data  
{  
    struct work_struct my_work;
    struct socket * client;
};

static void work_handler(struct work_struct *work)  
{
    struct work_struct_data *wsdata = (struct work_struct_data *) work;  
    char *recvbuf = NULL;  
    recvbuf = kmalloc(1024, GFP_KERNEL);  
    if (recvbuf == NULL){  
        printk("server: recvbuf kmalloc error!\n");  
        return;  
    }  
    memset(recvbuf, 0, sizeof(recvbuf));  
          
    //receive message from client  

    struct kvec vec_client;  
    struct msghdr msg_client;  
    memset(&vec_client, 0, sizeof(vec_client));  
    memset(&msg_client, 0, sizeof(msg_client));  
    vec_client.iov_base = recvbuf;  
    vec_client.iov_len = 1024;  
    int ret = 0;
    ret = kernel_recvmsg(wsdata->client, &msg_client, &vec_client, 1, 1024, 0);  
    
    //create a socket for sending request to url.py

    struct socket *urlsock;
    struct sockaddr_in s_addr2;  
    unsigned short url_portnum = 9999;        
    memset(&s_addr2, 0, sizeof(s_addr2));
    s_addr2.sin_family = AF_INET;  
    s_addr2.sin_port = htons(url_portnum);
    s_addr2.sin_addr.s_addr = in_aton("127.0.0.1");  
    urlsock = (struct socket *) kmalloc (sizeof(struct socket), GFP_KERNEL);  
      
    ret = sock_create_kern(current->nsproxy->net_ns, AF_INET, SOCK_STREAM, 0, &urlsock);  
    if (ret < 0) {  
        printk("client:socket create error!\n");  
        return;  
    }  
    ret = urlsock->ops->connect(urlsock, (struct sockaddr *)&s_addr2, sizeof(s_addr2), 0);  
    if (ret != 0) {  
        printk("client:connect error!\n");  
        return;  
    } 
 
    //send request to url.py

    int len = strlen(recvbuf) + 1;    
    struct kvec vec_sendpy;  
    struct msghdr msg_sendpy;        
    vec_sendpy.iov_base = recvbuf;  
    vec_sendpy.iov_len = len;
 
    printk("client request == %s\nlen == %d\n", recvbuf, len);
    memset(&msg_sendpy, 0, sizeof(msg_sendpy));
    ret = kernel_sendmsg(urlsock, &msg_sendpy, &vec_sendpy, 1, len);  
    if (ret < 0) {  
        printk("client: kernel_sendmsg error!\n");  
        return;   
    }
    else if (ret != len) {  
        printk("client: ret!=len\n");  
    }
  
    kfree(recvbuf);
 
    //recv response from url.py

    char *url_reponse = NULL;  
    url_reponse = kmalloc(1024000, GFP_KERNEL);  
    if (url_reponse == NULL) {  
        printk("server: recvbuf kmalloc error!\n");  
        return ;  
    }
    memset(url_reponse, 0, sizeof(url_reponse)); 

    struct kvec vec_recvpy;
    struct msghdr msg_recvpy;
    memset(&vec_recvpy, 0, sizeof(vec_recvpy));
    memset(&msg_recvpy, 0, sizeof(msg_recvpy));
    vec_recvpy.iov_base = url_reponse;
    vec_recvpy.iov_len = 1024000;
    ret = kernel_recvmsg(urlsock, &msg_recvpy, &vec_recvpy, 1, 1024000, 0);
    if (ret < 0) {
	printk("server: kernel_recvmsg error!\n");
    }

    printk("url.py reponse == %s\n", url_reponse);
    sock_release(urlsock);
    
    //send message to client

    len = strlen(url_reponse) * sizeof(char);  
    vec_client.iov_base = url_reponse; 
    vec_client.iov_len = len;
    memset(&msg_client, 0, sizeof(msg_client));
    ret = kernel_sendmsg(wsdata->client, &msg_client, &vec_client, 1, len);

    kfree(url_reponse);

    //release client socket
    sock_release(wsdata->client);  
}  


int myserver(void)
{      
    /*create a client_sock to receive client message*/
    /*define a server_sock to accept client connection*/

    struct socket *client_sock;  
    struct sockaddr_in s_addr;  
    unsigned short server_portnum = 8888;  
    memset(&s_addr, 0, sizeof(s_addr));  
    s_addr.sin_family = AF_INET;  
    s_addr.sin_port = htons(server_portnum);  
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
    
    server_sock = (struct socket *) kmalloc (sizeof(struct socket), GFP_KERNEL);
    client_sock = (struct socket *) kmalloc (sizeof(struct socket), GFP_KERNEL);
  
    int ret = sock_create_kern(current->nsproxy->net_ns, AF_INET, SOCK_STREAM,0,&server_sock);  
    if (ret) {  
        printk("server: socket_create error!\n");  
    }  
    printk("server: socket_create ok!\n");  
  
    /*set the socket can be reused*/
  
    int val = 1;  
    ret = kernel_setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val));  
    if(ret){  
        printk("server: kernel_setsockopt error!\n");  
    }  
  
    /*bind the socket*/
 
    ret = server_sock->ops->bind(server_sock, (struct sockaddr *)&s_addr, sizeof(struct sockaddr_in));
    if (ret < 0) {  
        printk("server: bind error!\n");  
        return;
    }
    printk("server: bind ok!\n");  
    
    /*listen the socket*/
  
    ret = server_sock->ops->listen(server_sock, 10);  
    if (ret < 0) {  
        printk("server: listen error!\n");  
        return;  
    }  
    printk("server: listen ok!\n");

    my_wq = create_workqueue("my_queue");

    //my_wq = alloc_workqueue("my_queue", WQ_HIGHPRI, 1);

    while(1)
    {
        struct work_struct_data * wsdata;
        ret = kernel_accept(server_sock, &client_sock, 100);   
        if(ret<0)
        {  
            printk("server: accept error!\n");  
            break; 
        }
	printk("server: accept success!\n");

        if (my_wq) {
 
            /*set workqueue data*/

            wsdata = (struct work_struct_data *) kmalloc (sizeof(struct work_struct_data), GFP_KERNEL);
            wsdata->client = client_sock;  
            
	    /*put task into workqueue*/

            if (wsdata) { 
                INIT_WORK(&wsdata->my_work, work_handler);    
                ret = queue_work(my_wq, &wsdata->my_work);  
            }  
        }  
        printk("server: accept ok, Connection Established.\n");    
    }
    sock_release(server_sock);  
    return;  
}  
  
static int server_init(void)
{
    printk("server: init!\n");  
    myserver();  
    return 0;  
}         
  
static void server_exit(void){  
    printk("server: good bye!\n");  
    flush_workqueue(my_wq);    
    destroy_workqueue(my_wq);  
}  

module_init(server_init);  
module_exit(server_exit);    
MODULE_LICENSE("GPL");
