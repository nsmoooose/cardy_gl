#ifdef linux
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#ifdef WIN32
#include <Winsock2.h>
#include <ws2tcpip.h>
#endif
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/* TODO: Handle port argument and default to 6666 */
/* TODO: Handle address argument and default to 0.0.0.0 */
/* TODO: Handle kill signal and shutdown nicely */
/* TODO: Create thread pool for client requests */
/* TODO: Take thread from pool and do send/receive */

struct client_args {
	int fd;
};

void *client_handler(void *arg) {
	printf("A thread created.\n");

	return 0;
}

int main(int argc, char *argv[]) {
	struct sockaddr_in addr;
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		fprintf(stderr, "failed to create socket.\n");
		return 1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6666);
	if (inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr) != 1) {
		perror("failed to parse address.\n");
		return 1;
	}
	if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
		perror("failed to bind socket.\n");
		return 1;
	}

	if (listen(fd, 3) == -1) {
		perror("failed to listen on incoming connections.\n");
		return 1;
	}

	for (;;) {
		int sock_fd = accept(fd, 0, 0);
		if (sock_fd == -1) {
			perror("failed to accept socket\n");
			return 1;
		} else {
			pthread_t thread;
			struct client_args *args = calloc(1, sizeof(struct client_args));
			args->fd = sock_fd;
			printf("Client connected. Time to start a thread\n");
			if (pthread_create(&thread, NULL, client_handler, (void *)args) !=
			    0) {
				perror("Failed to create thread.");
				return 1;
			}
		}
	}

	return 0;
}
