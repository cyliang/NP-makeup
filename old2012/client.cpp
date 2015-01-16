#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Usage: %s <host> <port>\n", argv[0]);
		exit(1);
	}

	struct hostent *host = gethostbyname(argv[1]);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	memcpy(&addr.sin_addr, host->h_addr, host->h_length);

	int stdinfd = fileno(stdin);
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));

	fd_set rset, all;
	FD_ZERO(&all);
	FD_SET(sockfd, &all);
	FD_SET(stdinfd, &all);
	int maxfd = (stdinfd > sockfd ? stdinfd : sockfd) + 1;
	char buffer[100];

	while(1) {
		rset = all;
		select(maxfd, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset)) {
			int num;
			read(sockfd, buffer, 100);

			if(strncmp(buffer, "Sum", 3) == 0) {
				sscanf(buffer, "Sum %d", &num);
				printf("%d\n", num);
			} else if(strncmp(buffer, "WARN", 4) == 0) {
				sscanf(buffer, "WARN %d", &num);
				sprintf(buffer, "%d", num);
				write(sockfd, buffer, 100);
			}
		} else if(FD_ISSET(stdinfd, &rset)) {
			if(read(stdinfd, buffer, 100) == 0) {
				close(sockfd);
				break;
			}
			write(sockfd, buffer, 100);
		}
	}
}
