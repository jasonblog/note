#include "pthread_pool.h"

/*
*child_work:the code exec in child thread
*ptr: the ds of thread_node of current thread.
*return :nothing.void * just avoid warning.
*/
void*
child_work(void* ptr)
{
    THREAD_NODE* self = (THREAD_NODE*) ptr;

    /*modify the tid attribute the first time exec */
    pthread_mutex_lock(&self->mutex);

    self->tid = syscall(SYS_gettid);

    pthread_mutex_unlock(&self->mutex);

    while (1) {
        pthread_mutex_lock(&self->mutex);

        /*if no task exec,blocked */
        if (NULL == self->work) {
            pthread_cond_wait(&self->cond, &self->mutex);
        }

        pthread_mutex_lock(&self->work->mutex);

        /*execute the real work. */
        self->work->fun(self->work->arg);

        /*after finished the work */
        self->work->fun = NULL;
        self->work->flag = 0;
        self->work->tid = 0;
        self->work->next = NULL;

        free(self->work->arg);

        pthread_mutex_unlock(&self->work->mutex);   //unlock the task
        pthread_mutex_destroy(&self->work->mutex);

        /*free the task space */
        free(self->work);

        /*make self thread no work */
        self->work = NULL;
        self->flag = 0;

        pthread_mutex_lock(&task_queue_head->mutex);


        /*
         *get new task from the task_link if not NULL.
         *there no idle thread if there are task to do.
         *if on task ,make self idle and add to the idle queue.
         */
        if (task_queue_head->head != NULL) {
            TASK_NODE* temp = task_queue_head->head;

            /*get the first task */
            task_queue_head->head = task_queue_head->head->next;

            /*modify self thread attribute */
            self->flag = 1;
            self->work = temp;
            temp->tid = self->tid;
            temp->next = NULL;
            temp->flag = 1;

            task_queue_head->number--;

            pthread_mutex_unlock(&task_queue_head->mutex);

            pthread_mutex_unlock(&self->mutex);

            continue;
        } else {
            /*no task need to exec, add self to idle queue and del from busy queue */
            pthread_mutex_unlock(&task_queue_head->mutex);

            pthread_mutex_lock(&pthread_queue_busy->mutex);

            /*self is the last execte thread */
            if (pthread_queue_busy->head == self
                && pthread_queue_busy->rear == self) {
                pthread_queue_busy->head = pthread_queue_busy->rear = NULL;
                self->next = self->prev = NULL;
            }

            /*the first one thread in busy queue */
            else if (pthread_queue_busy->head == self
                     && pthread_queue_busy->rear != self) {
                pthread_queue_busy->head = pthread_queue_busy->head->next;
                pthread_queue_busy->head->prev = NULL;

                self->next = self->prev = NULL;
            }

            /*the last one thread in busy queue */
            else if (pthread_queue_busy->head != self
                     && pthread_queue_busy->rear == self) {
                pthread_queue_busy->rear = pthread_queue_busy->rear->prev;
                pthread_queue_busy->rear->next = NULL;

                self->next = self->prev = NULL;
            }

            /*middle one */
            else {
                self->next->prev = self->prev;
                self->prev->next = self->next;
                self->next = self->prev = NULL;
            }

            pthread_mutex_unlock(&pthread_queue_busy->mutex);

            /*add self to the idle queue */
            pthread_mutex_lock(&pthread_queue_idle->mutex);

            /*now the idle queue is empty */
            if (pthread_queue_idle->head == NULL
                || pthread_queue_idle->head == NULL) {
                pthread_queue_idle->head = pthread_queue_idle->rear = self;
                self->next = self->prev = NULL;
            }

            else {
                self->next = pthread_queue_idle->head;
                self->prev = NULL;
                self->next->prev = self;

                pthread_queue_idle->head = self;
                pthread_queue_idle->number++;
            }

            pthread_mutex_unlock(&pthread_queue_idle->mutex);

            pthread_mutex_unlock(&self->mutex);

            /*signal have idle thread */
            pthread_cond_signal(&pthread_queue_idle->cond);
        }
    }
}

/*
*create thread pool when the system on, and thread number is THREAD_DEF_NUM.
*when init, initial all the thread into a double link queue and all wait fo self->cond.
*/
void
create_pthread_pool(void)
{
    THREAD_NODE* temp =
        (THREAD_NODE*) malloc(sizeof(THREAD_NODE) * THREAD_DEF_NUM);

    if (temp == NULL) {
        printf(" malloc failure\n");
        exit(EXIT_FAILURE);
    }

    /*init as a double link queue */
    int i;

    for (i = 0; i < THREAD_DEF_NUM; i++) {
        temp[i].tid = i + 1;
        temp[i].work = NULL;
        temp[i].flag = 0;

        if (i == THREAD_DEF_NUM - 1) {
            temp[i].next = NULL;
        }

        if (i == 0) {
            temp[i].prev = NULL;
        }

        temp[i].prev = &temp[i - 1];
        temp[i].next = &temp[i + 1];

        pthread_cond_init(&temp[i].cond, NULL);
        pthread_mutex_init(&temp[i].mutex, NULL);

        /*create this thread */
        pthread_create(&temp[i].tid, NULL, child_work, (void*) &temp[i]);
    }

    /*modify the idle thread queue attribute */
    pthread_mutex_lock(&pthread_queue_idle->mutex);

    pthread_queue_idle->number = THREAD_DEF_NUM;
    pthread_queue_idle->head = &temp[0];
    pthread_queue_idle->rear = &temp[THREAD_DEF_NUM - 1];

    pthread_mutex_unlock(&pthread_queue_idle->mutex);
}

/*
*init_system :init the system glob pointor.
*/
void
init_system(void)
{
    /*init the pthread_queue_idle */
    pthread_queue_idle =
        (PTHREAD_QUEUE_T*) malloc(sizeof(PTHREAD_QUEUE_T));

    pthread_queue_idle->number = 0;
    pthread_queue_idle->head = NULL;
    pthread_queue_idle->rear = NULL;
    pthread_mutex_init(&pthread_queue_idle->mutex, NULL);
    pthread_cond_init(&pthread_queue_idle->cond, NULL);

    /*init the pthread_queue_busy */
    pthread_queue_busy =
        (PTHREAD_QUEUE_T*) malloc(sizeof(PTHREAD_QUEUE_T));

    pthread_queue_busy->number = 0;
    pthread_queue_busy->head = NULL;
    pthread_queue_busy->rear = NULL;
    pthread_mutex_init(&pthread_queue_busy->mutex, NULL);
    pthread_cond_init(&pthread_queue_busy->cond, NULL);

    /*init the task_queue_head */
    task_queue_head = (TASK_QUEUE_T*) malloc(sizeof(TASK_QUEUE_T));

    task_queue_head->head = NULL;
    task_queue_head->number = 0;
    pthread_cond_init(&task_queue_head->cond, NULL);
    pthread_mutex_init(&task_queue_head->mutex, NULL);

    /*create thread poll */
    create_pthread_pool();
}

/*
*thread_manager:code exec in manager thread.
*               block on task_queue_head->cond when no task come.
*               block on pthread_queue_idle->cond when no idle thread
*ptr:no used ,in order to avoid warning.
*return :nothing.
*/

void*
thread_manager(void* ptr)
{
    while (1) {
        THREAD_NODE* temp_thread = NULL;
        TASK_NODE* temp_task = NULL;

        /*
         *get a new task, and modify the task_queue.
         *if no task block om task_queue_head->cond.
         */
        pthread_mutex_lock(&task_queue_head->mutex);

        if (task_queue_head->number == 0)
            pthread_cond_wait(&task_queue_head->cond,
                              &task_queue_head->mutex);

        temp_task = task_queue_head->head;
        task_queue_head->head = task_queue_head->head->next;
        task_queue_head->number--;

        pthread_mutex_unlock(&task_queue_head->mutex);


        /*
         *get a new idle thread, and modify the idle_queue.
         *if no idle thread, block on pthread_queue_idle->cond.
         */
        pthread_mutex_lock(&pthread_queue_idle->mutex);

        if (pthread_queue_idle->number == 0)
            pthread_cond_wait(&pthread_queue_idle->cond,
                              &pthread_queue_idle->mutex);

        temp_thread = pthread_queue_idle->head;

        /*if this is the last idle thread ,modiry the head and rear pointor */
        if (pthread_queue_idle->head == pthread_queue_idle->rear) {
            pthread_queue_idle->head = NULL;
            pthread_queue_idle->rear = NULL;
        }
        /*if idle thread number>2, get the first one,modify the head pointor  */
        else {
            pthread_queue_idle->head = pthread_queue_idle->head->next;
            pthread_queue_idle->head->prev = NULL;
        }

        pthread_queue_idle->number--;

        pthread_mutex_unlock(&pthread_queue_idle->mutex);

        /*modify the  task attribute. */
        pthread_mutex_lock(&temp_task->mutex);

        temp_task->tid = temp_thread->tid;
        temp_task->next = NULL;
        temp_task->flag = 1;

        pthread_mutex_unlock(&temp_task->mutex);

        /*modify the idle thread attribute. */
        pthread_mutex_lock(&temp_thread->mutex);

        temp_thread->flag = 1;
        temp_thread->work = temp_task;
        temp_thread->next = NULL;
        temp_thread->prev = NULL;

        pthread_mutex_unlock(&temp_thread->mutex);

        /*add the thread assinged task to the busy queue. */
        pthread_mutex_lock(&pthread_queue_busy->mutex);

        /*if this is the first one in busy queue */
        if (pthread_queue_busy->head == NULL) {
            pthread_queue_busy->head = temp_thread;
            pthread_queue_busy->rear = temp_thread;
            temp_thread->prev = temp_thread->next = NULL;
        } else {
            /*insert in thre front of the queue */
            pthread_queue_busy->head->prev = temp_thread;
            temp_thread->prev = NULL;
            temp_thread->next = pthread_queue_busy->head;
            pthread_queue_busy->head = temp_thread;
            pthread_queue_busy->number++;
        }

        pthread_mutex_unlock(&pthread_queue_busy->mutex);

        /*signal the child thread to exec the work */
        pthread_cond_signal(&temp_thread->cond);
    }
}

/*
*code to process the new client in every chilld pthead.
*ptr: the fd come from listen thread that can communicate to the client.
*return:nothing. void * only used to avoid waring.
*/
void*
prcoess_client(void* ptr)
{
    int net_fd;
    net_fd = atoi((char*) ptr);

    struct info client_info;

    memset(&client_info, '\0', sizeof(client_info));

    /*get the command mesg from client */
    if (-1 == recv(net_fd, &client_info, sizeof(client_info), 0)) {
        printf("recv msg error\n");
        close(net_fd);
        goto clean;
    }

    /* if the client requre the attribute of one file. */
    if (client_info.flag == 1) {
        struct stat mystat;

        if (-1 == stat(client_info.buf, &mystat)) {
            printf("stat %s error\n", client_info.buf);
            close(net_fd);
            goto clean;
        }

        char msgbuf[1024];

        memset(msgbuf, '\0', 1024);
        sprintf(msgbuf, "%d", htonl(
                    mystat.st_size));  /*only send the length of the file. */


        /*send info of the file */
        if (-1 == send(net_fd, msgbuf, strlen(msgbuf) + 1, 0)) {
            printf("send msg error\n");
            close(net_fd);
            goto clean;
        }

        close(net_fd);
        return ;
    }

    else {
        int local_begin = ntohl(
                              client_info.local_begin);   /* the content begining location. */
        int length = ntohl(
                         client_info.length);             /* the length of the content. */

        int file_fd;

        /* open the src file and get the real content. */
        if (-1 == (file_fd = open(client_info.buf, O_RDONLY))) {
            printf("open file %s error\n", client_info.buf);
            close(net_fd);
            goto clean;
        }

        /*seek the read local to the real location. */
        lseek(file_fd, local_begin, SEEK_SET);

        int need_send = length;
        int ret;

        do {
            char buf[1024];

            memset(buf, '\0', 1024);

            /*read 1024bytes from file everytome, if the last one,read the real length. */
            if (need_send < 1024) {
                ret = read(file_fd, buf, need_send);
            } else {
                ret = read(file_fd, buf, 1024);
            }

            if (ret == -1) {
                printf("read file %s error\n", client_info.buf);
                close(net_fd);
                close(file_fd);
                return ;
            }

            /*send the data to client. */
            if (-1 == send(net_fd, buf, ret, 0)) {
                printf("send file %s error\n", client_info.buf);
                close(net_fd);
                close(file_fd);
                return;
            }
        } while (ret > 0);

        close(net_fd);
        close(file_fd);
    }

    return ;

clean:
    sys_clean();
}

/*
*task_manager: get new task and add to the tail of the task_link.
*ptr: no used. just avoid warning.
*return:nothing.
*/

void*
task_manager(void* ptr)
{
    int listen_fd;

    if (-1 == (listen_fd = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("socket");
        goto clean;
    }

    struct ifreq ifr;

    strcpy(ifr.ifr_name, "eth0");

    if (ioctl(listen_fd, SIOCGIFADDR, &ifr) < 0) {
        perror("ioctl");
        goto clean;
    }

    struct sockaddr_in myaddr;

    myaddr.sin_family = AF_INET;

    myaddr.sin_port = htons(PORT);

    myaddr.sin_addr.s_addr =
        ((struct sockaddr_in*) & (ifr.ifr_addr))->sin_addr.s_addr;

    if (-1 == bind(listen_fd, (struct sockaddr*) &myaddr, sizeof(myaddr))) {
        perror("bind");
        goto clean;
    }

    if (-1 == listen(listen_fd, 5)) {
        perror("listen");
        goto clean;
    }

    /*i is the id of the task */
    int i;

    for (i = 1;; i++) {
        int newfd;
        struct sockaddr_in client;
        socklen_t len = sizeof(client);

        if (-1 ==
            (newfd = accept(listen_fd, (struct sockaddr*) &client, &len))) {
            perror("accept");
            goto clean;
        }

        TASK_NODE* temp = NULL;
        TASK_NODE* newtask = (TASK_NODE*) malloc(sizeof(TASK_NODE));

        if (newtask == NULL) {
            printf("malloc error");
            goto clean;
        }

        /*
         *initial the attribute of the task.
         *because this task havn't add to system,so,no need lock the mutex.
         */

        newtask->arg = (void*) malloc(128);

        memset(newtask->arg, '\0', 128);
        sprintf(newtask->arg, "%d", newfd);

        newtask->fun = prcoess_client;
        newtask->tid = 0;
        newtask->work_id = i;
        newtask->next = NULL;
        pthread_mutex_init(&newtask->mutex, NULL);

        /*add new task to task_link */
        pthread_mutex_lock(&task_queue_head->mutex);

        /*find the tail of the task link and add the new one to tail */
        temp = task_queue_head->head;

        if (temp == NULL) {
            task_queue_head->head = newtask;
        } else {
            while (temp->next != NULL) {
                temp = temp->next;
            }

            temp->next = newtask;
        }

        task_queue_head->number++;

        pthread_mutex_unlock(&task_queue_head->mutex);

        /*signal the manager thread , no task coming */
        pthread_cond_signal(&task_queue_head->cond);
    }

    return ;

clean:
    sys_clean();
}

/*
*monitor: get the system info
*ptr: not used ,only to avoid warning for pthread_create
*return: nothing.
*/
void*
monitor(void* ptr)
{
    /*in order to prevent warning. */
    ptr = ptr;

    THREAD_NODE* temp_thread = NULL;

    while (1) {
        pthread_mutex_lock(&pthread_queue_busy->mutex);

        /*output the busy thread works one by one */
        temp_thread = pthread_queue_busy->head;

        printf("\n*******************************\n");

        while (temp_thread) {
            printf("thread %ld is  execute work_number %d\n",                   \
                   temp_thread->tid, temp_thread->work->work_id);
            temp_thread = temp_thread->next;
        }

        printf("*******************************\n\n");

        pthread_mutex_unlock(&pthread_queue_busy->mutex);

        sleep(10);
    }

    return;
}

/*
*sys_clean: clean the system .
*this function code need to do to make it more stronger.
*/

void
sys_clean(void)
{
    printf("the system exit abnormally\n");
    exit(EXIT_FAILURE);
}
