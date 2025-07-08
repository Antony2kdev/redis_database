#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static void do_something(int connfd){
	char rbuf[64]={};
	ssize_t n = read(connfd, rbuf, sizeof(rbuf));

	if(n<0){
		msg("read() error");
		return ;
	}

	printf("the client says : %s\n", rbuf);

	char wbuf[]= "hello world";
	write(connfd, wbuf, strlen(wbuf));
}

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	
	printf("Logs from your program will appear here!\n");
	//obtain socket handle
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	//set socket options before binding
	int val = 1;
	//reuse already open sockets if in TIME_WAIT
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);
	addr.sin_addr.s_addr = htonl(0);

	int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
	if(rv){
		die("bind()");
	}
    //binded now listen

	rv = listen(fd, SOMAXCONN);
	if(rv){
		die("listen()");
	}

	//keep listening to client connection requests in a loop

	while(true){
		//accept
		struct sockaddr_in client_addr={};
		socklen_t addrlen = sizeof(client_addr);
		int connfd = accept(fd, (struct sockaddr*)&client_addr, &addrlen);

		if(connfd<0){
			continue;
		}

		do_something(connfd);
		close(connfd);
	}
    

	return 0;
}
