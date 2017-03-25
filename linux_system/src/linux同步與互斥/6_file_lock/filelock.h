#ifndef SLN_FILELOCK_H_H_
#define SLN_FILELOCK_H_H_

/**
 * the api for acquire and release recorde lock
 * param fd - file descriptor
 */
int sln_file_wrlock(int fd);
int sln_file_rdlock(int fd);
int sln_file_unlock(int fd);

/**
 * the api for acquire and release recorde lock,
 * if lock is held by other process, then wait for that lock to be release
 * param fd - file descriptor
 */
int sln_file_wait_wrlock(int fd);
int sln_file_wait_rdlock(int fd);

/**
 * the api for test locks on the file
 * param fd - file descriptor
 */
int sln_file_wrlock_test(int fd);
int sln_file_rdlock_test(int fd);
;
#endif
