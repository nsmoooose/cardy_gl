#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

/* TODO: Handle port argument and default to 6666 */
/* TODO: Handle address argument and default to 0.0.0.0 */
/* TODO: Handle kill signal and shutdown nicely */
/* TODO: Create thread pool for client requests */
/* TODO: Take thread from pool and do send/receive */

int main(int argc, char* argv[]) {
	struct sockaddr_in addr;
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1) {
		fprintf(stderr, "failed to create socket.\n");
		return -1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6666);
	if(inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr) != 1) {
		fprintf(stderr, "failed to parse address.\n");
		return -1;
	}
	if(bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
		fprintf(stderr, "failed to bind socket.\n");
		return -1;
	}

	if(listen(fd, 3) == -1) {
		fprintf(stderr, "failed to listen on incoming connections.\n");
		return -1;
	}

	for(;;) {
		int sock_fd = accept(fd, 0, 0);
		if(sock_fd == -1) {
			fprintf(stderr, "failed to accept socket\n");
			return -1;
		}
		else {
			fprintf(stdout, "Client connected. Time to start a thread\n");
		}
	}

	return 0;
}
