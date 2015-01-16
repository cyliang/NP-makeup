#include "readline.h"
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

/* Public iterface */
ssize_t readline(int fd, char *buffer, size_t maxlen);


/* Internal implementation methods & data */
#define RL_MAX 100000

static pthread_key_t _rl_key;
static pthread_once_t _rl_once = PTHREAD_ONCE_INIT;

typedef struct {
	int count;
	char *bufptr;
	char buf[RL_MAX];
} _RLData;

static ssize_t _read_char(_RLData *, int fd, char *ptr);
static void _rl_destructor(void *);
static void _rl_first();

ssize_t readline(int fd, char *buffer, size_t maxlen) {
	pthread_once(&_rl_once, _rl_first);
	
	_RLData *rl_data;
	if((rl_data = pthread_getspecific(_rl_key)) == NULL) {
		rl_data = malloc(sizeof(_RLData));
		rl_data->count = 0;
		pthread_setspecific(_rl_key, rl_data);
	}

	int n;
	for(n = 1; n < maxlen; n++) {
		char c;
		switch(_read_char(rl_data, fd, &c)) {
		case 1:
			*buffer++ = c;

			if(c == '\n') {
				*buffer = '\0';
				return n;
			}
			break;

		case 0:
			*buffer = 0;
			return n-1;
			break;

		default:
			return -1;
		}
	}

	*buffer = '\0';
	return n;
}

void _rl_destructor(void *ptr) {
	free(ptr);
}

void _rl_first() {
	pthread_key_create(&_rl_key, _rl_destructor);
}

ssize_t _read_char(_RLData *rl_data, int fd, char *ptr) {
	if(rl_data->count <= 0) {
		while(1) {
			if((rl_data->count = read(fd, rl_data->buf, RL_MAX)) < 0) {
				if(errno == EINTR)
					continue;
				return -1;
			} else if(rl_data->count == 0)
				return 0;

			rl_data->bufptr = rl_data->buf;
			break;
		}
	}

	rl_data->count--;
	*ptr = *rl_data->bufptr++;
	return 1;
}
