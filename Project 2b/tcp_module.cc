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
int SendData(const MinetHandle &mux, const MinetHandle &sock, ConnectionToStateMapping<TCPState> &theCTSM, Buffer data);


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
		    	cerr << "Packets arriving...\n";
		    	SockRequestResponse request, response;
		    	static SockRequestResponse *writer = NULL;
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
		    	ip_header = rcv_packet.FindHeader(Headers::IPHeader);

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
		    		cerr << "Connection not found in connection list.\n";
		    	}

		    	state = list_iterator->state.GetState();

		    	switch(state){
		    		case LISTEN:{
		    			if(IS_SYN(flags)){
		    				list_iterator->connection = c;
		    				cerr << "Listening...\n";
		    				list_iterator->state.last_acked = list_iterator->state.last_sent;
		    				list_iterator->state.SetLastRecvd(seq + 1);
		    				list_iterator->state.last_sent = list_iterator->state.last_sent + 1;
		    				list_iterator->timeout = Time() + 3; //3 second timeout
		    				list_iterator->bTmrActive = true;
		    				list_iterator->state.SetState(SYN_RCVD);

		    				createPacket(*list_iterator, 3, 0, false, snd_packet);
		    				MinetSend(mux, snd_packet);
		    				sleep(2);
		    				MinetSend(mux, snd_packet);

		    			} break;
		    		}
		    		case SYN_RCVD:{
		    			cerr << "Received SYN, sending SYN_ACK...\n";
		    			list_iterator->state.SetState(ESTABLISHED);
		    			list_iterator->state.SetSendRwnd(window);
		    			list_iterator->state.SetLastAcked(ack);
		    			list_iterator->state.last_sent = list_iterator->state.last_sent + 1;
		    			list_iterator->bTmrActive = false;
		    			writer = new SockRequestResponse(WRITE, list_iterator->connection, rcv_buffer, 0, EOK);
		    			MinetSend(sock, *writer);
		    			delete writer;
		    			break;
		    		}

		    		case SYN_SENT:{
		    			cerr << "Syn sent but did not receive synack\n";
		    			if(IS_ACK(flags) && IS_SYN(flags)){
		    				cerr << "Sent SYN, received SYN_ACK...\n";
		    				list_iterator->state.SetLastRecvd(seq + 1);
		    				list_iterator->state.last_acked = ack;
		    				list_iterator->state.last_sent = list_iterator->state.last_sent + 1;
		    				
		    				createPacket(*list_iterator, 2, 0, false, snd_packet);
		    				MinetSend(mux, snd_packet);

		    				list_iterator->bTmrActive = false;
		    				list_iterator->state.SetState(ESTABLISHED);
		    				SockRequestResponse writer(WRITE, list_iterator->connection, rcv_buffer, 0, EOK);
		    				MinetSend(sock, writer);
		    			} break;
		    		}

		    		case ESTABLISHED: {
		    			// As the client, the server has acked our packet
		    			if(IS_ACK(flags)){
		    				cerr << "Last packet has been ACKed\n...";
		    			}
		    		}
		    	}
		    }

		    if (event.handle == sock) {
		    	cerr << "Socket request...\n";
			// socket request or response has arrived
				MinetSendToMonitor(MinetMonitoringEvent("Socket request or response has arrived.\n"));
				SockRequestResponse request;
				SockRequestResponse response;
				Packet snd_packet;
				MinetReceive(sock,request);

				ConnectionList<TCPState>::iterator list_iterator = clist.FindMatching(request.connection);

				if(list_iterator == clist.end()){

					switch(request.type){
						case CONNECT:{
							cerr << "CONNECT\n";
							TCPState tcp_client(rand() % 500, SYN_SENT, 5);
							ConnectionToStateMapping<TCPState> mapping(request.connection, Time()+2, tcp_client, true);
							clist.push_back(mapping);

							createPacket(mapping, 1, 0, false, snd_packet);
							MinetSend(mux, snd_packet);
							sleep(3);
							MinetSend(mux, snd_packet);

							response.type = STATUS;
							response.connection = request.connection;
							response.error = EOK;
							response.bytes = 0;
							MinetSend(sock, response);

							cerr << "End CONNECT!\n";

							break;
						}
						case ACCEPT: {

							TCPState tcp_server(rand() % 500, LISTEN, 5);
							//cerr << "Tcp_server started\n";
							ConnectionToStateMapping<TCPState> mapping(request.connection, Time(), tcp_server, false);
							clist.push_back(mapping);
							//cerr << "Connection to state mapped and added to connection state list\n";
							response.type=STATUS;
		    				response.connection=request.connection;
						    // buffer is zero bytes
						    response.bytes=0;
						    response.error=EOK;
						    MinetSend(sock,response);
						    cerr << "ACCEPT...\n";
					    	break;
					  	}
						case WRITE: {
							cerr << "WRITE\n";
							response.connection = request.connection;
							response.error = ENOMATCH;
							response.bytes = 0;
							response.type = STATUS;
							MinetSend(sock, response);
						}
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
					cerr << "Connection found in connection list.\n";

					unsigned int state;
					state = list_iterator->state.GetState();
					switch(request.type){
			    		case CONNECT:
			    			break;
			    		case ACCEPT:
			    			break;
			    		case WRITE: {
			    			cerr << "WRITE\n";
							if (state == ESTABLISHED) {
								if(!(list_iterator->state.SendBuffer.GetSize()+request.data.GetSize() > list_iterator->state.TCP_BUFFER_SIZE)) {
									// If there is enough space in the buffer
									//list_iterator->state.SendBuffer.AddBack(request.data); // Push data to the back of the state
								
								
									Buffer copy_buffer = request.data; // Dupe the buffer
									
									
									int return_value = SendData(mux, sock, *list_iterator, copy_buffer);
									
									if (return_value == 0) {
										response.type = STATUS;
										response.connection = request.connection;
										response.bytes = copy_buffer.GetSize();
										response.error = EOK;
										MinetSend(sock, response);
									}
									
								}
								else {
									// If there isn't enough space in the buffer, send the appropriate error
									response.type = STATUS;
									response.connection = request.connection;
									response.bytes = 0;
									response.error = EBUF_SPACE;
									MinetSend(sock, response);
								}
							}
						}	
							break;
			    		case FORWARD:
			    			break;
			    		case CLOSE:
			    			break;
			    		case STATUS:{
			    			if (state == ESTABLISHED) {
								unsigned datasend = request.bytes; // number of bytes send
								list_iterator->state.RecvBuffer.Erase(0,datasend);
								if(0 != list_iterator->state.RecvBuffer.GetSize()) { // Didn't finish writing
									SockRequestResponse writer (WRITE, list_iterator->connection, list_iterator->state.RecvBuffer, list_iterator->state.RecvBuffer.GetSize(), EOK);
									MinetSend(sock, writer);
								}
							}
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

	cerr << "Creating a packet...\n";
	int pkt_size = size + IP_HEADER_BASE_LENGTH + TCP_HEADER_BASE_LENGTH;
	unsigned char flags = 0;
	IPHeader ip_header;
	TCPHeader tcp_header;

	//Create and add the IP header to the packet
	ip_header.SetProtocol(IP_PROTO_TCP);
	ip_header.SetSourceIP(CTSM.connection.src);
	ip_header.SetDestIP(CTSM.connection.dest);
	ip_header.SetTotalLength(pkt_size);
	packet.PushFrontHeader(ip_header);

	tcp_header.SetSourcePort(CTSM.connection.srcport, packet);
	tcp_header.SetDestPort(CTSM.connection.destport, packet);
	tcp_header.SetHeaderLen(5, packet);
	tcp_header.SetAckNum(CTSM.state.GetLastRecvd(), packet);
	tcp_header.SetWinSize(CTSM.state.GetRwnd(), packet);
	tcp_header.SetUrgentPtr(0, packet);

	//What kind of packet is it?
	switch(TCPHeaderType) {
		case 1: { //SYN
			cerr << "Creating a SYN packet.\n";
			SET_SYN(flags);
			break;
		} 
		case 2: { //ACK
			SET_ACK(flags);
			cerr << "Creating an ACK packet.\n";
			break;
		}
		case 3: { //SYNACK
			cerr << "Creating a SYN_ACK packet.\n";
			SET_SYN(flags);
			SET_ACK(flags);
			break;
		}
		case 4: { //PSH_ACK
			cerr << "Creating a PSH_ACK packet.\n";
			SET_PSH(flags);
			SET_ACK(flags);
		}
		case 5: { //FIN
			cerr << "Creating a FIN packet.\n";
			SET_FIN(flags);
		}
		case 6: { //FINACK
			cerr << "Creating a FIN_ACK packet.\n";
			SET_FIN(flags);
			SET_ACK(flags);
		}
		case 7: { //RST
			cerr << "Creating a RST packet.\n";
			SET_RST(flags);
			break;
		}
	}

	tcp_header.SetFlags(flags, packet);
	
	if(is_timeout) {
		tcp_header.SetSeqNum(CTSM.state.GetLastSent() + 1, packet);
	}
	else {
		tcp_header.SetSeqNum(CTSM.state.GetLastSent(), packet);
	}

	tcp_header.RecomputeChecksum(packet);
	packet.PushBackHeader(tcp_header);

	cerr << "Finished creating the packet!\n";
}

int SendData(const MinetHandle &mux, const MinetHandle &sock, ConnectionToStateMapping<TCPState> &theCTSM, Buffer data) {
                                        /*CL_iterator->state.SendBuffer.AddBack(Buffer(data.c_str(), data.length()));
                                        packet_tosend = CL_iterator->state.SendBuffer;
                                        ForgePacket (packet_tosend, *CL_iterator, HEADERTYPE_PSHACK, data.length(), false);
                                        MinetSend(mux, packet_tosend);*/
        cerr << "Sending the data...\n";
		Packet p;
        theCTSM.state.SendBuffer.AddBack(data);
        unsigned int bytesLeft = data.GetSize();
        //theCTSM.state.SetLastSent(theCTSM.state.GetLastAcked());
		//theCTSM.state.last_sent = theCTSM.state.last_sent + 1;
        while(bytesLeft != 0) {
                unsigned int bytesToSend = min(bytesLeft, TCP_MAXIMUM_SEGMENT_SIZE);
                p = theCTSM.state.SendBuffer.Extract(0, bytesToSend);
                createPacket (theCTSM, 4, bytesToSend, false, p);
                MinetSend(mux, p);
                
                //theCTSM.state.SetLastSent(theCTSM.state.GetLastSent()+bytesToSend);
                theCTSM.state.last_sent = theCTSM.state.last_sent + bytesToSend;
				
                bytesLeft -= bytesToSend;
        }
	cerr << "Done sending the data!\n";
	return bytesLeft;
}

