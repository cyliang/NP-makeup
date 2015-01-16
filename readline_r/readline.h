#pragma once
#include <sys/types.h> // For size_t, ssize_t

ssize_t readline(int fd, char *buffer, size_t maxlen);
