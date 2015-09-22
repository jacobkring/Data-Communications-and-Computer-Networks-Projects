/* UNCOMMENT FOR MINET 
 * #include "minet_socket.h"
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


#define BUFSIZE 1024

int main(int argc, char * argv[]) {

    char * server_name = NULL;
    int server_port    = -1;
    char * server_path = NULL;
    char * req         = NULL;
    bool ok            = false;

    /*parse args */
    if (argc != 5) {
	fprintf(stderr, "usage: http_client k|u server port path\n");
	exit(-1);
    }

    server_name = argv[2];
    server_port = atoi(argv[3]);
    server_path = argv[4];

    req = (char *)malloc(strlen("GET  HTTP/1.0\r\n\r\n") 
			 + strlen(server_path) + 1);  

    /* initialize */
    if (toupper(*(argv[1])) == 'K') { 
	/* UNCOMMENT FOR MINET 
	 * minet_init(MINET_KERNEL);
         */
    } else if (toupper(*(argv[1])) == 'U') { 
	/* UNCOMMENT FOR MINET 
	 * minet_init(MINET_USER);
	 */
    } else {
	fprintf(stderr, "First argument must be k or u\n");
	exit(-1);
    }

    /* make socket */
    int s, port, len, res;
    char buf[BUFSIZE];
    struct hostent *hp;
    struct sockaddr_in saddr;
    fd_set readfds;

    port = server_port;

    if((s=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
    	// error processing
    }


    /* get host IP address  */
    /* Hint: use gethostbyname() */

    if((hp=gethostbyname(server_name))==NULL){
		// error processing
    }

    /* set address */
    saddr.sin_family = AF_INET;
	memcpy(&saddr.sin_addr.s_addr, hp->h_addr, hp->h_length);
	saddr.sin_port = htons(port);
    /* connect to the server socket */
	if(connect(s, (struct sockaddr *)&saddr, sizeof(saddr))<0){
		// error processing
	}
    /* send request message */
    sprintf(req, "GET %s HTTP/1.0\r\n\r\n", server_path);

    /* wait till socket can be read. */
    /* Hint: use select(), and ignore timeout for now. */
    FD_ZERO(&readfds);
    select(s+1, &readfds, NULL, NULL, NULL);
    /* first read loop -- read headers */

    /* examine return code */   

    //Skip "HTTP/1.0"
    //remove the '\0'

    // Normal reply has return code 200

    /* print first part of response: header, error code, etc. */

    /* second read loop -- print out the rest of the response: real web content */

    /*close socket and deinitialize */
    close(s);

    if (ok) {
	return 0;
    } else {
	return -1;
    }
}
