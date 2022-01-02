#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"


#ifdef __cplusplus
thrd_t thread_start(int (*runnable)(void *args), void *args) 
{
  thrd_t tid;
  thrd_create(&tid, runnable, args);
  return tid;
}

void lock(sync_t *m)
{
  if(m==NULL) {
	  fprintf(stderr, "Can't lock on null ptr!\n");
	  exit(-1);
  }
  m->lock();
}

void unlock(sync_t *m)
{
  if(m==NULL) {
	  fprintf(stderr, "Can't unlock on null ptr!\n");
	  exit(-1);
  }
  m->unlock();

}

#else
pthread_t thread_start(void * (*runnable)(void *args), void *args) 
{
  pthread_t tid;
  pthread_attr_t *attr = xmalloc(sizeof(*attr));
  int rc = pthread_attr_init(attr);
  if(rc!=0) {
	  perror("pthread_attr_init()");
	  exit(-1);
  }
  pthread_create(&tid, attr, runnable, args);
  return tid;
}

void lock(sync_t *m)
{
  if(m==NULL) {
	  fprintf(stderr, "Can't lock on null ptr!\n");
	  exit(-1);
  }
  int rc = pthread_mutex_lock(m);
  if(rc != 0) {
	  perror("pthread_mutex_lock()");
	  exit(-1);
  }
}

void unlock(sync_t *m)
{
  if(m==NULL) {
	  fprintf(stderr, "Can't unlock on null ptr!\n");
	  exit(-1);
  }
  int rc = pthread_mutex_unlock(m);
  if(rc != 0) {
	  perror("pthread_mutex_lunock()");
	  exit(-1);
  }

}

#endif /* __cplusplus */




void *xmalloc(size_t nbytes)
{
	void *x = malloc(nbytes);
	if(x==NULL) {
		perror("malloc()");
		exit(-1);
	}
	return x;
}

char *new_string(const char *src)
{
	if(src==NULL) {
		fprintf(stderr, "Can't pass null as new string!\n");
		exit(-1);
	}
	char *x = (char *)xmalloc(strlen(src));
	strcpy(x, src);
	return x;
}

char *new_string_sz(char *src, ssize_t nbytes)
{
	char *s = (char *)xmalloc(nbytes+1);
	memset(s, 0, nbytes+1);
	strncpy(s, src, nbytes);
	return s;
}

char *format(const char *fmt, ...)
{
	char *s = (char *)xmalloc(1024);
	memset(s, 0, 1024);
	va_list args;
	va_start(args, fmt);
	vsnprintf(s, 1024, fmt, args);
	va_end(args);
	return s;
}


int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

bool endswith( const char *s1, const char *s2 )
{
    size_t n1 = strlen( s1 );
    size_t n2 = strlen( s2 );

    return ( n2 == 0 ) || ( !( n1 < n2 ) && memcmp( s1 + n1 - n2, s2, n2 ) == 0 );
}

void chomp(char *s)
{
	if(s==NULL) return;
	int l = strlen(s);
	for(int i=0; i<l; i++) {
		if(s[i]=='\n' || s[i]=='\r') {
			s[i]='\0';
			return;
		}
	}
}

bool only_digits(const char *s)
{
	if(s==NULL) return False;
	int l = strlen(s);
	for(int i=0; i<l; i++) {
		if(s[i]<'0' || s[i]>'9') {
			return False;
		}
	}
	return True;

}
