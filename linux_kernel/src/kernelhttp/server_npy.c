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
    int ret, len;
    struct work_struct_data *wsdata = (struct work_struct_data *) work;
    struct kvec vec_client;  
    struct msghdr msg_client;   
    char *recvbuf = NULL;
    char *response_buf = NULL;
  
    recvbuf = kmalloc(1024, GFP_KERNEL);  
    if (recvbuf == NULL){  
        printk("server: recvbuf kmalloc error!\n");  
        return;  
    }  
    memset(recvbuf, 0, 1024);
          
    //receive message from client  
 
    memset(&vec_client, 0, sizeof(struct kvec));  
    memset(&msg_client, 0, sizeof(struct msghdr));  
    vec_client.iov_base = recvbuf;
    vec_client.iov_len = 1024;  
    ret = kernel_recvmsg(wsdata->client, &msg_client, &vec_client, 1, 1024, 0);  

    len = strlen(recvbuf) + 1;
             
    //printk("client request == %s\nlen == %d\n", recvbuf, len);
    
    //send message to client

    response_buf = kmalloc(1024, GFP_KERNEL);  
    if (response_buf == NULL){  
        printk("server: response_buf kmalloc error!\n");  
        return;  
    } 
    memset(response_buf, 0, 1024);

    snprintf(response_buf, 1024,
	"HTTP/1.1 200 OK\r\n"
        "Content-Length: 12\r\n"
        "Connection: keep-alive\r\n"
        "Keep-Alive: timeout=2\r\n"
        "\r\n"
        "Hello World!");

    memset(&vec_client, 0, sizeof(struct kvec));
    memset(&msg_client, 0, sizeof(struct msghdr));  
    len = strlen(response_buf) * sizeof(char);  
    vec_client.iov_base = response_buf; 
    vec_client.iov_len = len;

    ret = kernel_sendmsg(wsdata->client, &msg_client, &vec_client, 1, len);

    kfree(recvbuf);
    kfree(response_buf);
    
    //release client socket
    sock_release(wsdata->client);  
}  


int myserver(void)
{      
    /*create a client_sock to receive client message*/
    /*define a server_sock to accept client connection*/

    int ret, val;
    struct socket *client_sock;  
    struct sockaddr_in s_addr;  
    unsigned short server_portnum = 8888;
  
    memset(&s_addr, 0, sizeof(struct sockaddr_in));  
    s_addr.sin_family = AF_INET;  
    s_addr.sin_port = htons(server_portnum);  
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
    
    server_sock = (struct socket *) kmalloc (sizeof(struct socket), GFP_KERNEL);
    client_sock = (struct socket *) kmalloc (sizeof(struct socket), GFP_KERNEL);
  
    ret = sock_create_kern(current->nsproxy->net_ns, AF_INET, SOCK_STREAM, 0, &server_sock);  
    if (ret) {  
        printk("server: socket_create error!\n");  
    }  
    printk("server: socket_create ok!\n");  
  
    /*set the socket can be reused*/
  
    val = 1; 
    ret = kernel_setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int));  
    if(ret){  
        printk("server: kernel_setsockopt error!\n");  
    }  
  
    /*bind the socket*/
 
    ret = server_sock->ops->bind(server_sock, (struct sockaddr *)&s_addr, sizeof(struct sockaddr_in));
    if (ret < 0) {  
        printk("server: bind error!\n");  
        return ret;
    }
    printk("server: bind ok!\n");  
    
    /*listen the socket*/
  
    ret = server_sock->ops->listen(server_sock, 100);  
    if (ret < 0) {  
        printk("server: listen error!\n");  
        return ret;  
    }  
    printk("server: listen ok!\n");

#ifdef HIGH_PRI
struct workqueue_attrs *attr;
attr = alloc_workqueue_attrs(__GFP_HIGH);
apply_workqueue_attrs(my_wq, attr);
#else
my_wq = create_workqueue("my_queue");
#endif
    
    //my_wq = alloc_workqueue("my_queue",WQ_MEM_RECLAIM | WQ_HIGHPRI, 1);

    
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
    return 0;  
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
