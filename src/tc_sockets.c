#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "lib/tc_util.h"

int
fd_unblock (int fd)
{
	// set socket to non blocking
	// return status of operation
    int flags;

    /* If they have O_NONBLOCK, use the Posix way to do it */
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int
fd_write (int fd, char *buf, size_t len)
{
	int rc = write (fd, buf, len);
	if (rc <= 0) perror ("write");
	return rc;
}

int
fd_read (int fd)
{
	char buf[64] = {0};
	int rc = read (fd, buf, 64);
	if (rc == -1) perror ("read");
	if (rc != 0) {
		buf[64] = '\0';
		fprintf (stdout, "%d: %s\n", fd, buf);
	}
	return rc;
}

int
bind_and_listen (const int portno, int n)
{
	// this functions simply wraps
	// bind
	// listen
	//
	// n is the listen parameter
    struct sockaddr_in servAddr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { 
        exit_error("ERROR opening socket");
    }
    bzero((char *) &servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY; // TODO: bind to a selected interface
    servAddr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        exit_error("ERROR on binding");
    }
    int rc = fd_unblock (sockfd);
    assert (rc >= 0);
    if (listen(sockfd, n) != -1) {
    	return sockfd;
    } else {
    	// error on listen, check errno
    	return -1;
    }
}
