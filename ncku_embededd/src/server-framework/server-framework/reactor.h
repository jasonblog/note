#ifndef _REACTOR_H
#define _REACTOR_H

#ifndef REACTOR_MAX_EVENTS
#define REACTOR_MAX_EVENTS 64
#endif
#ifndef REACTOR_TICK
#define REACTOR_TICK 1000
#endif

#include <sys/time.h>
#include <sys/types.h>

/** \file
 * @example: test-reactor.c
 */

/**
 * \brief [Reactor pattern](https://en.wikipedia.org/wiki/Reactor_pattern)
 *        implementation using callbacks as Linux epoll abstraction
 *
 * Supported events (and corresponding callbacks):
 *  - Ready to Read (`on_data` callback).
 *  - Ready to Write (`on_ready` callback).
 *  - Closed (`on_close` callback).
 *  - Reactor Exit (`on_shutdown` callback - will be called before the file
 *    descriptor is closed and `on_close` callback is fired).
 */
struct Reactor {
    /** @name File Descriptor Callbacks */
    //@{

    /**
     * \brief Called when the file descriptor has incoming data.
     *
     * This is edge triggered and will not be called again unless all the
     * previous data was consumed.
     */
    void (*on_data)(struct Reactor* reactor, int fd);

    /**
     * Called when the file descriptor is ready to send data (outgoing).
     */
    void (*on_ready)(struct Reactor* reactor, int fd);

    /**
     * Called for any open file descriptors when the reactor is shutting down.
     */
    void (*on_shutdown)(struct Reactor* reactor, int fd);

    /**
     * \brief Called when a file descriptor was closed REMOTELY.
     *
     * `on_close` will NOT get called when a connection is closed locally,
     * unless using `reactor_close` function.
     */
    void (*on_close)(struct Reactor* reactor, int fd);
    //@}

    /** @name global data and settings */
    //@{

    /** the time (seconds since epoch) of the last "tick" (event cycle). */
    time_t last_tick;

    /**
     * the maximum value for a file descriptor that the reactor will
     * be required to handle (the capacity -1).
     */
    int maxfd;
    //@}

    /** \cond private data */
    void* priv;
    /** \endcond */
};

/**
 * \brief Initialize the reactor, making the reactor "live".
 * \public \memberof Reactor
 *
 * Once initialized, the reactor CANNOT be forked, so do not fork
 * the process after calling `reactor_init`, or data corruption will
 * be experienced.
 *
 * @return -1 on error.
 * @return 0  otherwise.
 */
int reactor_init(struct Reactor*);

/**
 * \brief Review any pending events (up to REACTOR_MAX_EVENTS).
 * \public \memberof Reactor
 *
 * @return -1 on error.
 * @return the number of events handled by the reactor.
 */
int reactor_review(struct Reactor*);

/**
 * \brief Close the reactor, releasing its resources.
 * \public \memberof Reactor
 *
 * The resources to be released do not include the actual struct Reactor,
 * which might have been allocated on the stack and should be handled by
 * the caller).
 */
void reactor_stop(struct Reactor*);

/**
 * \brief Add a file descriptor to the reactor.
 * \public \memberof Reactor
 * Callbacks will be called for its events.
 * @return -1 on error.
 */
int reactor_add(struct Reactor*, int fd);

/**
 * \brief Remove a file descriptor from the reactor.
 * \public \memberof Reactor
 * Further callbacks will not be called.
 * @return -1 on error.
 * @return other.  If the file descriptor was not owned by the reactor,
 *         it is not an error.
 */
int reactor_remove(struct Reactor*, int fd);

/**
 * \brief Close a file descriptor.
 * \public \memberof Reactor
 *
 * Execute its callback if it was registered with the reactor.
*/
void reactor_close(struct Reactor*, int fd);

/**
 * \brief Add a file descriptor as a timer object.
 * \public \memberof Reactor
 *
 * @return -1 on error.
 */
int reactor_add_timer(struct Reactor*, int fd, long milliseconds);

/**
 * \brief the timer will be repeated when running on epoll.
 * \public \memberof Reactor
 */
void reactor_reset_timer(int fd);

/**
 * \brief Open a new file decriptor for creating timer events.
 * \public \memberof Reactor
 *
 * @return -1 on error.
 * @return the file descriptor.
 */
int reactor_make_timer(void);

#endif
