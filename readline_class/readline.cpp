#include "readline.h"
#include <unistd.h>
#include <errno.h>

Readline::Readline(int _fd):
	fd(_fd), count(0), bufPtr(buf)
{
}

ssize_t Readline::getline(char *buffer, size_t maxlen) {
	int n;
	for(n=1; n<maxlen; n++) {
		char c;

		switch(_readChar(&c)) {
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

ssize_t Readline::_readChar(char *c) {
	if(count <= 0) {
		while(1) {
			if((count = read(fd, buf, RL_MAX)) < 0) {
				if(errno == EINTR)
					continue;
				return -1;
			} else if(count == 0)
				return 0;

			bufPtr = buf;
			break;
		}
	}

	count--;
	*c = *bufPtr++;
	return 1;
}
