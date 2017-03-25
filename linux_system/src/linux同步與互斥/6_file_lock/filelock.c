#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

/**
 * \brief     acquire, release or test for the existence of record locks
 * \details   if a conficling is held on the file, then return -1.
 *
 * \param     fd - file descriptor
 * \param     locktype - lock type: F_RDLCK, F_WRLCK, F_UNLCK.
 *
 * \return    0 is success, < 0 is failed.
 */
static int sln_filelock_set(int fd, short locktype)
{
    struct flock    lock;

    lock.l_type = locktype;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLK, &lock) < 0) {
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

/**
 * \brief     acquire, release or test for the existence of record locks
 * \details   if a conficling is held on the file, then wait for that
 *            lock to be release
 *
 * \param     fd - file descriptor
 * \param     locktype - lock type: F_RDLCK, F_WRLCK, F_UNLCK.
 *
 * \return    0 is success, < 0 is failed.
 */
int sln_filelock_wait_set(int fd, short locktype)
{
    struct flock    lock;

    lock.l_type = locktype;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) < 0) {
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int sln_file_wrlock(int fd)
{
    return sln_filelock_set(fd, F_WRLCK);
}

int sln_file_rdlock(int fd)
{
    return sln_filelock_set(fd, F_RDLCK);
}

int sln_file_wait_wrlock(int fd)
{
    return sln_filelock_wait_set(fd, F_WRLCK);
}

int sln_file_wait_rdlock(int fd)
{
    return sln_filelock_wait_set(fd, F_RDLCK);
}

int sln_file_unlock(int fd)
{
    return sln_filelock_set(fd, F_UNLCK);
}

/**
 * \brief     test for the existence of record locks on the file
 * \details   none
 *
 * \param     fd - file descriptor
 * \param     locktype - lock type: F_RDLCK, F_WRLCK.
 *
 * \return    0 is success, < 0 is failed.
 */
static int sln_filelock_test(int fd, short locktype)
{
    struct flock    lock;

    lock.l_type = locktype;
    lock.l_whence = 0;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = 0;

    if (fcntl(fd, F_GETLK, &lock) < 0) {
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
        return -1;
    }

    if (lock.l_type != F_UNLCK) { //file is locked
        if (F_WRLCK == lock.l_type) {
            printf("write lock hold by process <%d>, lock_type: %d\n", lock.l_pid,
                   lock.l_type);
        } else if (F_RDLCK == lock.l_type) {
            printf("read lock hold by process <%d>, lock_type: %d\n", lock.l_pid,
                   lock.l_type);
        }

        return lock.l_pid; //return the pid of process holding that lock
    } else {
        printf("Unlock, lock type: %d\n", lock.l_type);
        return 0;
    }
}

int sln_file_wrlock_test(int fd)
{
    return sln_filelock_test(fd, F_WRLCK);
}

int sln_file_rdlock_test(int fd)
{
    return sln_filelock_test(fd, F_RDLCK);
}
