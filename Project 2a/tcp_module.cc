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

void createPacket(ConnectionToStateMapping<TCPState> &CTSM, int TCPHeaderType, int size, bool is_timeout, Packet &packet);


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

	if ( (mux == MINET_NOHANDLE) && (MinetIsModuleInConfig(MINET_IP_MUX)) ) {

			MinetSendToMonitor(MinetMonitoringEvent("Can't connect to ip_mux"));

			return -1;
    }

   if ( (sock == MINET_NOHANDLE) && (MinetIsModuleInConfig(MINET_SOCK_MODULE)) ) {

			MinetSendToMonitor(MinetMonitoringEvent("Can't accept from sock_module"));

			return -1;
    }
    
    cerr << "tcp_module STUB VERSION handling tcp traffic.......\n";

    MinetSendToMonitor(MinetMonitoringEvent("tcp_module STUB VERSION handling tcp traffic........\n"));

    MinetEvent event;
    double timeout = -1;

    while (MinetGetNextEvent(event, timeout) == 0) {

		if ((event.eventtype == MinetEvent::Dataflow) && (event.direction == MinetEvent::IN)) {
	
		    if (event.handle == mux) {
		    	// ip packet has arrived!
		    	cerr << "packets arriving\n";
		    	SockRequestResponse request, response;
		    	Packet rcv_packet;
		    	Packet snd_packet;
		    	Buffer rcv_buffer;
		    	unsigned int state;
		    	MinetReceive(mux, rcv_packet);

		    	Connection c;
		    	TCPHeader tcp_header;
		    	IPHeader ip_header;

				unsigned int ack, seq;
				unsigned short total_len;
				unsigned char tcpheader_len, ipheader_len, flags;
				unsigned short window, isUrg;

		    	rcv_packet.ExtractHeaderFromPayload<TCPHeader>(TCPHeader::EstimateTCPHeaderLength(rcv_packet));
		    	tcp_header = rcv_packet.FindHeader(Headers::TCPHeader);
		    	ip_header = rcv_packet.FindHeader(Headers::TCPHeader);

		    	if(!tcp_header.IsCorrectChecksum(rcv_packet)){
		    		cerr << "Checksum didn't match!\n";
		    	}

		    	//Get the header data
		    	ip_header.GetProtocol(c.protocol);
		    	ip_header.GetSourceIP(c.dest);
		    	ip_header.GetDestIP(c.src);
		    	ip_header.GetFlags(flags);
		    	ip_header.GetHeaderLength(ipheader_len);
		    	ip_header.GetTotalLength(total_len);
		    	tcp_header.GetSourcePort(c.destport);
		    	tcp_header.GetDestPort(c.srcport);
		    	tcp_header.GetAckNum(ack);
		    	tcp_header.GetSeqNum(seq);
		    	tcp_header.GetFlags(flags);
		    	tcp_header.GetWinSize(window);
		    	tcp_header.GetHeaderLen(tcpheader_len);
		    	tcp_header.GetUrgentPtr(isUrg);

		    	cerr << "This is the connection received: \n" << c << endl;


		    	total_len = total_len - ipheader_len - tcpheader_len;
		    	rcv_buffer = rcv_packet.GetPayload().ExtractFront(total_len);

		    	ConnectionList<TCPState>::iterator list_iterator = clist.FindMatching(c);
		    	if(list_iterator == clist.end()){
		    		cerr << "Connection not found";
		    	}

		    	switch(state){
		    		case LISTEN:{
		    			if(IS_SYN(flags)){
		    				list_iterator->connection = c;
		    				list_iterator->state.last_acked = list_iterator->state.last_sent;
		    				list_iterator->state.SetLastRecvd(seq + 1);
		    				list_iterator->state.last_sent = list_iterator->state.last_sent + 1;
		    				list_iterator->timeout = Time() + 3; //3 second timeout
		    				list_iterator->bTmrActive = true;
		    				list_iterator->state.SetState(SYN_RCVD);

		    				createPacket(*list_iterator, 1, 0, false, snd_packet);
		    				MinetSend(mux, snd_packet);
		    				sleep(2);
		    				MinetSend(mux, snd_packet);

		    			}
		    			break;
		    		}
		    	}
		    }

		    if (event.handle == sock) {
		    	cerr << "Socket request...\n";
			// socket request or response has arrived
				MinetSendToMonitor(MinetMonitoringEvent("Socket request or response has arrived.\n"));
				SockRequestResponse request;
				SockRequestResponse response;
				MinetReceive(sock,request);

				ConnectionList<TCPState>::iterator list_iterator = clist.FindMatching(request.connection);

				if(list_iterator == clist.end()){

					switch(request.type){
						case CONNECT:
							cerr << "CONNECT\n";
							//TCPState client(1, SYN_SENT, 10);
						case ACCEPT: {
							cerr << "ACCEPT\n";

							TCPState tcp_server(1, LISTEN, 10);
							//cerr << "Tcp_server started\n";
							ConnectionToStateMapping<TCPState> map(request.connection, Time(), tcp_server, false);
							clist.push_back(map);
							//cerr << "Connection to state mapped and added to connection state list\n";
							response.type=STATUS;
		    				response.connection=request.connection;
						    // buffer is zero bytes
						    response.bytes=0;
						    response.error=EOK;
						    MinetSend(sock,response);
						    cerr << "Sent response\n";
					    
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
				else{
					cerr << "Found this connection";

					unsigned int state;
					state = list_iterator->state.GetState();
					switch(request.type){
			    		case CONNECT:
			    			break;
			    		case ACCEPT:
			    			break;
			    		case WRITE:
			    			break;
			    		case FORWARD:
			    			break;
			    		case CLOSE:
			    			break;
			    		case STATUS:{
			    // 			if (my_state == ESTABLISHED) {
							// 	unsigned datasend = request.bytes; // number of bytes send
							// 	list_iterator->state.RecvBuffer.Erase(0,datasend);
							// 	if(0 != CL_iterator->state.RecvBuffer.GetSize()) { // Didn't finish writing
							// 		SockRequestResponse write (WRITE, list_iterator->connection, list_iterator->state.RecvBuffer, list_iterator->state.RecvBuffer.GetSize(), EOK);
							// 		MinetSend(sock, write);
							// 	}
							// }
						break;
			    		}
		    		}
				}
		    }
		}

		if (event.eventtype == MinetEvent::Timeout) {
			    // timeout ! probably need to resend some packets
				//cerr << "Timeout...\n";
		}

    }

    MinetDeinit();

    return 0;
}

void createPacket(ConnectionToStateMapping<TCPState> &CTSM, int TCPHeaderType, int size, bool is_timeout, Packet &packet){

	int pkt_size = size + IP_HEADER_BASE_LENGTH + TCP_HEADER_BASE_LENGTH;
	unsigned char flags;
	IPHeader ip_header;
	TCPHeader tcp_header;

	//Create and add the IP header to the packet
	ip_header.SetProtocol(IP_PROTO_TCP);
	ip_header.SetSourceIP(CTSM.connection.src);
	ip_header.SetDestIP(CTSM.connection.dest);
	ip_header.SetTotalLength(pkt_size);
	packet.PushFrontHeader(ip_header);

	//What kind of packet is it?
	switch(TCPHeaderType) {
		case 1: { //SYN
			SET_SYN(flags);
			cerr << "Received SYN packet\n";
			break;
		} 
		case 2: { //ACK
			break;
		}
		case 3: { //SYNACK
			break;
		}
		case 4: { //PSACK

		}
		case 5: { //FIN

		}
		case 6: { //FINACK

		}
		case 7: { //RST
			break;
		}
	}
	tcp_header.SetHeaderLen(TCP_HEADER_BASE_LENGTH, packet);
	tcp_header.SetAckNum(CTSM.state.GetLastRecvd(), packet);
	tcp_header.SetSourcePort(CTSM.connection.srcport, packet);
	tcp_header.SetDestPort(CTSM.connection.destport, packet);
	tcp_header.SetWinSize(CTSM.state.GetRwnd(), packet);
	tcp_header.SetUrgentPtr(0, packet);
	tcp_header.SetFlags(flags, packet);
	
	if(is_timeout) {
		tcp_header.SetSeqNum(CTSM.state.GetLastSent() + 1, packet);
	}
	else {
		tcp_header.SetSeqNum(CTSM.state.GetLastSent(), packet);
	}

	tcp_header.RecomputeChecksum(packet);
	packet.PushBackHeader(tcp_header);
}

