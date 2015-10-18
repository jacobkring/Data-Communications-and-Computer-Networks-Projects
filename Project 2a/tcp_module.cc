// You will build this in project part B - this is merely a
// stub that does nothing but integrate into the stack

// For project parts A and B, an appropriate binary will be 
// copied over as part of the build process



#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#include <iostream>

#include "Minet.h"
#include "tcpstate.h"

using namespace std;

// struct TCPState {
//     // need to write this
//     std::ostream & Print(std::ostream &os) const { 
// 	os << "TCPState()" ; 
// 	return os;
//     }
// };


int main(int argc, char * argv[]) {
    MinetHandle mux;
    MinetHandle sock;
    
    ConnectionList<TCPState> clist;

    MinetInit(MINET_TCP_MODULE);

    mux = MinetIsModuleInConfig(MINET_IP_MUX) ?  
	MinetConnect(MINET_IP_MUX) : 
	MINET_NOHANDLE;
    
    sock = MinetIsModuleInConfig(MINET_SOCK_MODULE) ? 
	MinetAccept(MINET_SOCK_MODULE) : 
	MINET_NOHANDLE;

    if ( (mux == MINET_NOHANDLE) && 
	 (MinetIsModuleInConfig(MINET_IP_MUX)) ) {

	MinetSendToMonitor(MinetMonitoringEvent("Can't connect to ip_mux"));

	return -1;
    }

    if ( (sock == MINET_NOHANDLE) && 
	 (MinetIsModuleInConfig(MINET_SOCK_MODULE)) ) {

	MinetSendToMonitor(MinetMonitoringEvent("Can't accept from sock_module"));

	return -1;
    }
    
    cerr << "tcp_module STUB VERSION handling tcp traffic.......\n";

    MinetSendToMonitor(MinetMonitoringEvent("tcp_module STUB VERSION handling tcp traffic........\n"));

    MinetEvent event;
    double timeout = 1;

    while (MinetGetNextEvent(event, timeout) == 0) {

			if ((event.eventtype == MinetEvent::Dataflow) && 
	    (event.direction == MinetEvent::IN)) {
	
		    if (event.handle == mux) {
		    	cerr << "packets arriving\n";
			// ip packet has arrived!
				MinetSendToMonitor(MinetMonitoringEvent("IP packet has arrived.\n"));
		    }

		    if (event.handle == sock) {
		    	cerr << "Socket request...\n";
			// socket request or response has arrived
					MinetSendToMonitor(MinetMonitoringEvent("Socket request or response has arrived.\n"));
					SockRequestResponse request;
					SockRequestResponse response;
					MinetReceive(sock,request);

					switch(request.type){
						case CONNECT:
							cerr << "CONNECT\n";
						case ACCEPT: {
							cerr << "ACCEPT\n";

							TCPState tcp_server(rand() % 5000, LISTEN, 5);
							ConnectionToStateMapping<TCPState> map(request.connection, Time(), tcp_server, false);
							clist.push_back(map);
							response.type=STATUS;
	    				response.connection=request.connection;
					    // buffer is zero bytes
					    response.bytes=0;
					    response.error=EOK;
					    MinetSend(sock,response);
					    
					  }
					  break;
						case WRITE:
							cerr << "WRITE\n";
						case FORWARD:
							cerr << "FORWARD\n";
						case CLOSE:
							cerr << "CLOSE\n";
						case STATUS:
							cerr << "STATUS\n";
						break;
					}
		    }
			}

			if (event.eventtype == MinetEvent::Timeout) {
			    // timeout ! probably need to resend some packets
				cerr << "Timeout...\n";
			}

    }

    MinetDeinit();

    return 0;
}
