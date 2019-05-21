#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF 256					// define buffer size

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Useage: ./<exec filename> <dest ip address> <port number>\n");
		exit(1);
	}

	int dscr;	// use descripter

	if ((dscr = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error\n");
		exit(1);
	}

	struct sockaddr_in addr;	// use sockaddr
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = atoi(argv[2]);

	if (connect(dscr, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) < 0) {
		perror("connect error\n");
		exit(1);
	}

	char buf[BUF];		//receive input and send it
	//initiation
	for (int i = 0; i < BUF; i++)
		buf[i] = '\0';

	while (1) {
		printf("> ");
		if (fgets(buf, BUF, stdin) == NULL) {	//scan stdin
			perror("fgets error\n");
			exit(1);
		} else {
			int size = strlen(buf);
			buf[size - 1] = '\0';

			if (send(dscr, buf, BUF, 0) < 0) {		// send data
				perror("send error\n");
				exit(1);
			}
			
			if (!strncmp(buf, "exit\0", BUF)) {
				break;
			}

			if ((size = recv(dscr, buf, BUF, 0)) < 0) {
				perror("recv error\n");
				exit(1);
			} else {
				buf[size - 1] = '\0';
				// print data
				printf("echo: %s\n", buf);
			}
		}
	}

	close(dscr);	// close the socket

	return 0;
}
