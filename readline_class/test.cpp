#include "readline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void test_interrupt(int i) {
	return;
}

void *thr_fn(void *arg) {
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(12345);

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		puts("Cannot bind.");
		pthread_exit(NULL);
	}

	listen(fd, 2222);
	int newfd = accept(fd, NULL, NULL);
	Readline rl(newfd);

	char buf[1000];
	while(rl.getline(buf, 1000) > 0) {
		fputs(buf, stdout);
		fflush(stdout);
	}

	close(newfd);
	close(fd);
	puts("Thread exit");
}

int main() {
	char buf[1000];
	int stdin_fd = fileno(stdin);

	pthread_t t;
	pthread_create(&t, NULL, thr_fn, NULL);

	/* Test interrupt */
	signal(SIGALRM, test_interrupt);
	siginterrupt(SIGALRM, 1);
	ualarm(100, 100);

	Readline rl(stdin_fd);
	while(rl.getline(buf, 1000) > 0) {
		fputs(buf, stdout);
		fflush(stdout);
	}
}
