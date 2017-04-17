#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "buffer.h"

#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

/* The pre-allocated memory per packet */

/* packet sizes */
#ifndef BUFFER_PACKET_SIZE
#define BUFFER_PACKET_SIZE (1024 * 64)
#endif
#ifndef BUFFER_MAX_PACKET_POOL
#define BUFFER_MAX_PACKET_POOL 127
#endif

/* Buffer packets */
struct Packet {
    ssize_t length;
    struct Packet* next;
    void* data;
    char mem[BUFFER_PACKET_SIZE];
    struct {
        unsigned can_interrupt : 1;
        unsigned close_after : 1;
        unsigned rsrv : 6;
    } metadata;
};

/* The global packet container pool */
static struct {
    int ref_count;
    int pool_count;
    struct Packet* pool;
} ContainerPool = { 0, 0 };

/* the packet pool mutex */
static pthread_mutex_t container_pool_locker = PTHREAD_MUTEX_INITIALIZER;

/* register a buffer in the pool - the pool will self-distruct when the last
 * buffer unregisters.
 */
static void register_buffer(void)
{
    pthread_mutex_lock(&container_pool_locker);
    ContainerPool.ref_count++;
    pthread_mutex_unlock(&container_pool_locker);
}

/* unregister a buffer in the pool */
static void unregister_buffer(void)
{
    pthread_mutex_lock(&container_pool_locker);
    ContainerPool.ref_count--;

    if (ContainerPool.ref_count <= 0) {
        ContainerPool.ref_count = 0;  // never fall from 0
        struct Packet* to_free;

        while ((to_free = ContainerPool.pool)) {
            ContainerPool.pool = to_free->next;
            free(to_free);
        }
    }

    pthread_mutex_unlock(&container_pool_locker);
}

/* grab a packet from the pool */
static struct Packet* get_packet(void)
{
    struct Packet* packet;
    pthread_mutex_lock(&container_pool_locker);
    packet = ContainerPool.pool;

    if (packet) {
        ContainerPool.pool = packet->next;
        ContainerPool.pool_count--;
    } else {
        packet = malloc(sizeof(struct Packet));
    }

    pthread_mutex_unlock(&container_pool_locker);

    if (!packet) {
        return 0;
    }

    packet->data = packet->mem;
    packet->next = 0;
    packet->length = 0;
    *((char*) &packet->metadata) = 0;
    return packet;
}

/* return a packet to the pool, or free it (when the pool is full) */
static void free_packet(struct Packet* packet)
{
    if (packet->data != packet->mem && packet->data) {
        if (packet->length) {
            free(packet->data);
        } else {
            fclose(packet->data);
        }
    }

    pthread_mutex_lock(&container_pool_locker);

    if (ContainerPool.pool_count <= BUFFER_MAX_PACKET_POOL) {
        packet->next = ContainerPool.pool;
        ContainerPool.pool = packet;
        ContainerPool.pool_count++;
    } else {
        free(packet);
    }

    pthread_mutex_unlock(&container_pool_locker);
}

// The buffer structure
struct Buffer {
    void* id;
    struct Packet* packet; /**< pointer to the actual data */
    size_t sent; /**< the amount of data sent from the first packet */
    pthread_mutex_t lock; /**< a mutex preventing buffer corruption */

    /** a writing hook, allowing for SSL sockets or other extensions. */
    ssize_t (*writing_hook)(server_pt srv, int fd, void* data, size_t len);
    server_pt owner; /**< the buffer's owner */
};

/* validates that this is an actual buffer object. */
static inline
int is_buffer(struct Buffer* object)
{
    return object->id == is_buffer;
}

/* create a new buffer object */
static inline void* create_buffer(server_pt owner)
{
    struct Buffer* buffer = malloc(sizeof(struct Buffer));

    if (!buffer) {
        return 0;
    }

    *buffer = (struct Buffer) {
        .id = is_buffer,
         .sent = 0, .packet = NULL,
          .owner = owner,
    };

    if (pthread_mutex_init(&buffer->lock, NULL)) {
        free(buffer);
        return 0;
    }

    register_buffer();
    return buffer;
}

/* clears all the buffer data */
static inline
void clear_buffer(void* buf)
{
    struct Buffer* buffer = buf;

    if (is_buffer(buffer)) {
        pthread_mutex_lock(&buffer->lock);
        struct Packet* to_free = NULL;

        while ((to_free = buffer->packet)) {
            buffer->packet = buffer->packet->next;
            free_packet(to_free);
        }

        buffer->writing_hook = NULL;
        pthread_mutex_unlock(&buffer->lock);
    }
}

void set_whook(void* buf,
               ssize_t (*writing_hook)(server_pt srv, int fd,
                                       void* data, size_t len))
{
    struct Buffer* buffer = buf;

    if (is_buffer(buffer)) {
        buffer->writing_hook = writing_hook;
    }
}

static inline
void destroy_buffer(void* buf)
{
    struct Buffer* buffer = buf;

    if (is_buffer(buffer)) {
        clear_buffer(buffer);
        pthread_mutex_destroy(&buffer->lock);
        free(buffer);
        unregister_buffer();
    }
}

/* applys the move logic for either urgent or non urgent packets */
static void
insert_packets_to_buffer(struct Buffer* buffer, struct Packet* packet,
                         char urgent)
{
    pthread_mutex_lock(&buffer->lock);
    struct Packet* tail, **pos = &(buffer->packet);

    if (urgent) {
        while (*pos && (!(*pos)->next ||
                        !(*pos)->next->metadata.can_interrupt)) {
            pos = &((*pos)->next);
        }
    } else {
        while (*pos) {
            pos = &((*pos)->next);
        }
    }

    tail = (*pos);
    *pos = packet;

    if (tail) {
        pos = &(packet->next);

        while (*pos) {
            pos = &((*pos)->next);
        }

        *pos = tail;
    }

    pthread_mutex_unlock(&buffer->lock);
}

static inline
size_t buffer_move_logic(void* buf, void* data,
                         size_t length, char urgent)
{
    struct Buffer* buffer = buf;

    if (!is_buffer(buffer)) {
        return 0;
    }

    if (!length || !data) {
        /* FIXME: warn the messages:
         * "Buffer: Canot move data because either length (%lu) or
         *  data (%p) are "invalid\n", length, data
        */
        return 0;
    }

    struct Packet* np = get_packet();

    if (!np) {
        return 0;
    }

    np->data = data;
    np->length = length;
    np->next = NULL;
    *((char*) &np->metadata) = 0;
    np->metadata.can_interrupt = 1;
    insert_packets_to_buffer(buffer, np, urgent);

    return length;
}

static size_t buffer_move(void* buf, void* data, size_t length)
{
    struct Buffer* buffer = buf;
    return buffer_move_logic(buffer, data, length, 0);
}

static size_t buffer_move_next(void* buf, void* data, size_t length)
{
    struct Buffer* buffer = buf;
    return buffer_move_logic(buffer, data, length, 1);
}

static size_t
buffer_copy_logic(void* buf, void* data,
                  size_t length, char urgent)
{
    struct Buffer* buffer = buf;

    if (!length || !data) {
        /* FIXME: warn the message
         * "Buffer: Canot copy data because either length (%lu) or
         * data (%p) are invalid\n", length, data
         */
        return 0;
    }

    size_t to_copy = length;
    struct Packet* np = get_packet();

    if (!np) {
        /* FIXME: warn the message
         * "Couldn't allocate memory for the buffer (on copy)"
         */
        return 0;
    }

    /* set marker for packet interrupt */
    np->metadata.can_interrupt = 1;
    struct Packet* tmp = np;

    while (to_copy) {
        if (to_copy > BUFFER_PACKET_SIZE) {
            memcpy(tmp->mem, data, BUFFER_PACKET_SIZE);
            tmp->data = tmp->mem;
            data += BUFFER_PACKET_SIZE;
            to_copy -= BUFFER_PACKET_SIZE;
            tmp->length = BUFFER_PACKET_SIZE;
            tmp->next = get_packet();

            if (!(tmp->next)) {
                /* FIXME: warn the message
                 * "Couldn't allocate memory for the buffer (on copy)"
                 */
                tmp = np;

                while (tmp) {
                    np = tmp;
                    tmp = np->next;
                    free_packet(np);
                }

                return 0;
            }

            tmp = tmp->next;
        } else {
            memcpy(tmp->mem, data, to_copy);
            tmp->data = tmp->mem;
            tmp->length = to_copy;
            to_copy = 0;
        }
    }

    insert_packets_to_buffer(buffer, np, urgent);
    return length;
}

static size_t buffer_copy(void* buf, void* data, size_t length)
{
    struct Buffer* buffer = buf;
    return buffer_copy_logic(buffer, data, length, 0);
}

static size_t buffer_copy_next(void* buf, void* data, size_t length)
{
    struct Buffer* buffer = buf;
    return buffer_copy_logic(buffer, data, length, 1);
}

static ssize_t buffer_flush(void* buf, int fd)
{
    struct Buffer* buffer = buf;

    if (!is_buffer(buffer)) {
        return -1;
    }

    ssize_t sent = 0;
    struct Packet* packet;

    pthread_mutex_lock(&buffer->lock);
start_flush:

    /* no packets to send */
    if (!buffer->packet) {
        pthread_mutex_unlock(&buffer->lock);
        return 0;
    }

    /* packet is a file */
    if (!buffer->packet->length) {
        /* make sure file sending isn't interrupted. */
        buffer->packet->metadata.can_interrupt = 0;
        /* grab a packet from the pool */
        packet = get_packet();
        /* read the data */
        packet->length =
            fread(packet->data, 1, BUFFER_PACKET_SIZE, buffer->packet->data);

        /* read less? done sending file */
        if (packet->length < BUFFER_PACKET_SIZE) {
            if (packet->length <= 0) {  /* no more data */
                free_packet(packet);
                /* move the buffer one step forward */
                packet = buffer->packet;
                buffer->packet = buffer->packet->next;
                free_packet(packet);
                packet = NULL;
            } else {  /* this will be the last the file will offer */
                /* set the next packet */
                packet->next = buffer->packet->next;
                free_packet(buffer->packet);
                /* set the data packet as the buffer's packet */
                buffer->packet = packet;
            }
        } else {
            packet->next = buffer->packet;
            /* set the data packet as the buffer's packet,
             * the file packet is next.
             */
            buffer->packet = packet;
        }

        /* make sure the sent property is reset */
        buffer->sent = 0;
        goto start_flush;
    }

    /* the packet, at this point, is always a data packet. send the data */

    /* write using the writing hook if available. */
    if (buffer->writing_hook) {
        sent = buffer->writing_hook(buffer->owner, fd,
                                    buffer->packet->data + buffer->sent,
                                    buffer->packet->length - buffer->sent);
    } else {
        sent = write(fd, buffer->packet->data + buffer->sent,
                     buffer->packet->length - buffer->sent);

        if (sent < 0 && (errno & (EWOULDBLOCK | EAGAIN | EINTR))) {
            sent = 0;
        }
    }

    if (sent < 0) {
        pthread_mutex_unlock(&buffer->lock);
        return -1;
    } else if (sent > 0) {
        buffer->sent += sent;
    }

    if (buffer->sent >= buffer->packet->length) {
        /* review the close connection flag means: "Close the connection" */
        if (buffer->packet->metadata.close_after) {
            pthread_mutex_unlock(&(buffer->lock));
            Server.close(buffer->owner, fd);
            return sent;
            /* buffer clearing should be performed by the Buffer's owner. */
        }

        packet = buffer->packet;
        buffer->sent = 0;
        buffer->packet = buffer->packet->next;
        free_packet(packet);
    }

    pthread_mutex_unlock(&(buffer->lock));
    return sent;
}

static int buffer_sendfile(void* buf, FILE* file)
{
    struct Buffer* buffer = buf;

    if (!is_buffer(buffer)) {
        return -1;
    }

    struct Packet* np = get_packet();

    if (!np) {
        return -1;
    }

    np->data = file;
    np->metadata.can_interrupt = 1;
    insert_packets_to_buffer(buffer, np, 0);
    return 0;
}

static void buffer_close_w_d(void* buf, int fd)
{
    struct Buffer* buffer = buf;

    if (!is_buffer(buffer)) {
        return;
    }

    if (!buffer->packet) {
        close(fd);
        return;
    }

    pthread_mutex_lock(&buffer->lock);
    struct Packet* packet = buffer->packet;

    if (!packet) {
        goto finish;
    }

    while (packet->next) {
        packet = packet->next;
    }

    packet->metadata.close_after = 1;
finish:
    pthread_mutex_unlock(&buffer->lock);
}

size_t buffer_pending(struct Buffer* buffer)
{
    if (!is_buffer(buffer)) {
        return 0;
    }

    size_t len = 0;
    struct Packet* p;
    pthread_mutex_lock(&buffer->lock);
    p = buffer->packet;

    while (p) {
        if (p->data && p->length) {
            len += p->length;
        } else if (p->data) {
            len += 1;    /* if it is a file - can we check it's size? */
        } else {
            break;    /* no need to move beyond a close connection packet */
        }

        p = p->next;
    }

    len -= buffer->sent;
    pthread_mutex_unlock(&buffer->lock);
    return len;
}

char buffer_is_empty(void* buf)
{
    struct Buffer* buffer = buf;
    return (!is_buffer(buffer)) ? 1 : (buffer->packet == NULL);
}

/* API gateway */
const struct BufferClass Buffer = {
    .create = create_buffer,
    .destroy = destroy_buffer,
    .clear = clear_buffer,
    .set_whook = set_whook,
    .sendfile = buffer_sendfile,
    .write = buffer_copy,
    .write_move = buffer_move,
    .write_next = buffer_copy_next,
    .write_move_next = buffer_move_next,
    .flush = buffer_flush,
    .close_when_done = buffer_close_w_d,
    .is_empty = buffer_is_empty,
};
