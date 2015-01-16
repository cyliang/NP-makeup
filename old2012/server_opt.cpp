#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[1]));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		puts("Cannot bind.");
		exit(2);
	}

	listen(sockfd, 1024);

	struct sockaddr recvaddr;
	socklen_t recvlen = sizeof(recvaddr);

	int sum = 0;
	char buffer[100];
	int n;
	while(n = recvfrom(sockfd, buffer, 100, 0, &recvaddr, &recvlen)) {
		if(n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			sprintf(buffer, "WARN %d", 100 - sum);
			sendto(sockfd, buffer, 100, 0, &recvaddr, recvlen);
		} else if(n > 0) {
			int num;
			sscanf(buffer, "%d", &num);

			sum += num;

			if(sum > 99) {
				sprintf(buffer, "Sum %d", sum);
				sendto(sockfd, buffer, 100, 0, &recvaddr, recvlen);
				sum = 0;
			}

			struct timeval tv;
			tv.tv_sec = 5;
			tv.tv_usec = 0;
			setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		}
	}
}
