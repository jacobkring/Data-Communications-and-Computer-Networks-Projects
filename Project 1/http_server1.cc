/* UNCOMMENT FOR MINET 
 * #include "minet_socket.h"
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
 #include <fcntl.h>
 #include <unistd.h>


#define BUFSIZE 1024
#define FILENAMESIZE 100

int handle_connection(int sock);

int main(int argc, char * argv[]) {
    int server_port = -1;
    int rc          =  0;
    int sock        = -1;
    int i, c, len, pos, res, port;
    char buf[FILENAMESIZE];

    struct sockaddr_in saddr;

    /* parse command line args */
    if (argc != 3) {
	fprintf(stderr, "usage: http_server1 k|u port\n");
	exit(-1);
    }

    server_port = atoi(argv[2]);

    if (server_port < 1500) {
	fprintf(stderr, "INVALID PORT NUMBER: %d; can't be < 1500\n", server_port);
	exit(-1);
    }

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


    /* initialize and make socket */
    printf("Initializing socket...\n");
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
        // error processing;
    }
    /* set server address*/
    printf("Set server address...\n");
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(server_port);
    /* bind listening socket */
    printf("Bind listening socket...\n");
    if(bind(sock, (struct sockaddr *)&saddr, sizeof(saddr))<0){
        // error processing.
    }
    /* start listening */
    printf("Start listening...\n");
    if (listen(sock, 5) < 0){
        // error processing
        printf("we have a listen error\n");
    }
    /* connection handling loop: wait to accept connection */



    while (1) {
	/* handle connections */
    printf("Connection handling loop...\n");
	rc = handle_connection(sock);
    }
}

int handle_connection(int sock) {
    int i, c, len, res, offset, sent;
    char buf[FILENAMESIZE];
    

    bool ok = false;

    const char * ok_response_f = "HTTP/1.0 200 OK\r\n"	\
	"Content-type: text/plain\r\n"			\
	"Content-length: %d \r\n\r\n";
 
    const char * notok_response = "HTTP/1.0 404 FILE NOT FOUND\r\n"	\
	"Content-type: text/html\r\n\r\n"			\
	"<html><body bgColor=black text=white>\n"		\
	"<h2>404 FILE NOT FOUND</h2>\n"
	"</body></html>\n";
    /* first read loop -- get request and headers*/
    while((c = accept(sock, NULL, NULL)) >= 0){
        if((len = read(c, buf, sizeof(buf)-1)) <= 0){
            // error processing
        }

        buf[len] = 0;

        /* parse request to get file name */
        /* Assumption: this is a GET request and filename contains no spaces*/

        char * pch;
        pch = strtok(buf, " ");
        pch = strtok(NULL, " ");
        printf("%s\n", pch);
        /* try opening the file */
        FILE * pFile;
        pFile = fopen(pch, "r");
        if(pFile!=NULL){
        	ok = true;
        }
            offset = 0;
    	/* send response */
    	if (ok) { //File Exists

		/* send headers */
    		if((res=write(c, ok_response_f, sizeof(ok_response_f)-1))){
    		// error handling
    		}
		/* send file */
			char file_data[BUFSIZE];

			size_t nbytes = 0;
			while(( nbytes = fread(file_data, sizeof(char), BUFSIZE, pFile))>0){
				while(sent = send(c, file_data, nbytes, 0) > 0){
					offset += sent;
					nbytes -= sent;
				}
			}	
    	} else { //File does not exist
    		if((res=write(c, notok_response, sizeof(notok_response)-1))){
    		  // error processing
    		}
		// send error response
    	}

        close(c);
    }
    

    
    /* close socket and free space */
	close(sock);


    if (ok) {
	return 0;
    } else {
	return -1;
    }
}
