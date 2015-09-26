
//#include "minet_socket.h"
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>


#define BUFSIZE 1024
#define FILENAMESIZE 100

int handle_connection(int sock);
int get_content_length(char * filename);

int main(int argc, char * argv[]) {
    int server_port = -1;
    int rc          =  0;
    int sock        = -1;
    int i, j, rv;

    fd_set master;
    fd_set read_fds;
    int maxfd;
    int newsocket;

    struct sockaddr_in saddr;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

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
        fprintf(stderr, "Could not initialize socket.\n");
        exit(-1);
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
        fprintf(stderr, "Could not bind socket.\n");
        exit(-1);
    }

    /* start listening */
    printf("Start listening...\n");
    if (listen(sock, 32) < 0){
        // error processing
        fprintf(stderr, "Listen error.\n");
        exit(-1);
    }

    FD_SET(sock, &master);
    maxfd = sock;

    /* connection handling loop: wait to accept connection */
    printf("Connection handling loop...\n");
    while (1) {

    	read_fds = master; //copy the master list
    	if(select(maxfd+1, &read_fds, NULL, NULL, NULL) == -1){
    		fprintf(stderr, "Select failure.");
            exit(-1);
    	}

    	for(i = 0; i <= maxfd; i++){
    		if(FD_ISSET(i, &read_fds)){ // Socket is ready for reading
    			if(i == sock){ //This is the Listening socket
    				newsocket = accept(sock, NULL, NULL);

    				if(newsocket == -1){
    					//error handling, accept returned bad socket
    					fprintf(stderr, "Bad accept socket.\n");
                        exit(-1);
    				} else {
    					FD_SET(newsocket, &master); //add socket to master list
    					if(newsocket > maxfd){
    						maxfd = newsocket; //need to keep track of max fd for select()
    					}
    					printf("selectserver: new connection from socket %d\n", newsocket);
    				}
    			} else{ // This is an Accept socket
    				printf("Start connection handling.\n");
    				rc = handle_connection(i);
    				close(i); // Close when finished
    				FD_CLR(i, &master); //Remove from master set
    			}
    		}
    	}
	
    }
}

int handle_connection(int sock) {

		int i, c, len, res, offset, sent;
    int f_size = 0;
    char buf[FILENAMESIZE];
    bool ok = false;

    const char * ok_response_f = "HTTP/1.0 200 OK\r\n"	\
	"Content-type: text/plain\r\n"			\
	"Content-length: %d \r\n\r\n";
    
    const char * notok_response = "HTTP/1.0 404 FILE NOT FOUND\r\n"	\
	"Content-type: text/html\r\n\r\n"			\
	"<html><body bgColor=black text=white>\n"		\
	"<h2>404 FILE NOT FOUND</h2>\n"				\
	"</body></html>\n";

	char response_with_length[strlen(ok_response_f)];
    
    /* first read loop -- get request and headers*/

    if((len = read(sock, buf, sizeof(buf)-1)) <= 0){
            // error processing
        }

        buf[len] = 0;
        printf("%s", buf);

        /* parse request to get file name */
        /* Assumption: this is a GET request and filename contains no spaces*/

        char * pch;
        pch = strtok(buf, " ");
        pch = strtok(NULL, " ");

        // Trim leading forward slash
        if(pch[0] == '/'){
            pch++;
        }
        /* try opening the file */
        FILE * pFile;
        pFile = fopen(pch, "r");

        if(pFile!=NULL){
        	ok = true;
            f_size = get_content_length(pch);
        }
        else{
            ok = false;
        }
            offset = 0;
    	/* send response */
    	if (ok) { //File Exists

            /* Format OK response */
            sprintf(response_with_length, ok_response_f, f_size);

		/* send headers */
    		if((res=write(sock, response_with_length, strlen(response_with_length)-1)) <= 0){
    		// error handling
    		}
		/* send file */
			char file_data[BUFSIZE];

			size_t nbytes = 0;
			while(( nbytes = fread(file_data, sizeof(char), BUFSIZE, pFile)) >0 ){
				if(sent = send(sock, file_data, nbytes, 0) > 0){
					offset += sent;
					nbytes -= sent;
				}
			}	
    	} else { //File does not exist
    		if((res=write(sock, notok_response, strlen(notok_response)-1)) <= 0){
    		  // error processing
    		}
		// send error response
    	}

    /* close socket and free space */
  
    if (ok) {
	return 0;
    } else {
	return -1;
    }
}

int get_content_length(char * file_name){
    char ch;
    int size = 0;
    FILE * fp;

    fp = fopen(file_name, "r");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    fclose(fp);
    return size;
}
