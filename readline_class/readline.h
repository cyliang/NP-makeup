#pragma once
#include <sys/types.h>

#ifndef RL_MAX
#define RL_MAX 10000
#endif

class Readline {
public:
	Readline(int fd);
	ssize_t getline(char *buffer, size_t maxlen);

private:
	ssize_t _readChar(char *c);

	int fd;
	int count;
	char buf[RL_MAX];
	char *bufPtr;
};
