#include "pthread_pool.h"

PTHREAD_QUEUE_T*
pthread_queue_idle;     /* the idle thread double link queue. */
PTHREAD_QUEUE_T*
pthread_queue_busy;      /* the work thread double link queue. */
TASK_QUEUE_T*
task_queue_head;            /* the task queuee single link list. */

int
main(int argc, char* argv[])
{
    pthread_t thread_manager_tid, task_manager_tid, monitor_id;

    init_system();

    pthread_create(&thread_manager_tid, NULL, thread_manager,
                   NULL);   /* create thread to manage the thread pool. */
    pthread_create(&task_manager_tid, NULL, task_manager,
                   NULL);     /* create thread recive task from client. */
    pthread_create(&monitor_id, NULL, monitor,
                   NULL);                /* create thread to monitor the system info. */

    pthread_join(thread_manager_tid, NULL);
    pthread_join(task_manager_tid, NULL);
    pthread_join(monitor_id, NULL);

    sys_clean();

    return 0;
}



