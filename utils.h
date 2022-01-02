#ifndef __TELLO_UTILS_H__
#define __TELLO_UTILS_H__ 1

#include <stdlib.h>
#include <stddef.h>

#define True 1
#define False 0
#ifndef __cplusplus
typedef _Bool bool;
#endif /* __cplusplus */
#ifdef __cplusplus
#include <threads.h>
#include <mutex>
typedef std::mutex sync_t;
#else
#include <pthread.h>
typedef pthread_mutex_t sync_t;
#endif /* __cplusplus */




#ifdef __cplusplus
thrd_t thread_start(int (*runnable)(void *args), void *args);
#else
pthread_t thread_start(void * (*runnable)(void *args), void *args) ;
#endif /* __cplusplus */
void *xmalloc(size_t nbytes);
char *new_string(const char *src);
char *new_string_sz(char *src, ssize_t nbytes);
char *format(const char *fmt, ...);
int msleep(long msec);
bool endswith( const char *s1, const char *s2 );
void chomp(char *s);
bool only_digits(const char *s);
void lock(sync_t *m);
void unlock(sync_t *m);

#endif /* __TELLO_UTILS_H__ */
