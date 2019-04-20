/**
Local Server-Client Subscription Model of the DADAO Network Communication Libraries.
Mar 2018, Jianrui Long @ DADAOII

In this Server-Client (Publisher-Subscriber) model, most content-carrying data flow from the
server side to the client side. Following are important properties.

**************************************** CONNECTION TYPE ****************************************
======================================== TCP connections ========================================
1. The server binds to an available IP and port and starts listening.
2. Clients try to connect to the server.
3. Once a connection is accepted, additional authentication may be required. Read Authentication
(RA) is sent from CLIENT to SERVER; Write Authentication (WA) is sent from SERVER to CLIENT.
These two authentication are mutual-exclusive. If authentication is required but failed to be
received or validated, the connection is immediately closed.
4. The server starts publishing data to clients. Heart-beat signals are sent periodically BOTH
WAYS to keep the connection alive. If the receiving end for any peer has been inactive for too
long, the connection will be closed and retried later.
======================================== UDP connections ========================================
1. The server binds to a fixed multicast IP with an available port and starts sending datagrams
right away.
2. Clients subscribe to this multicast IP and port to receive datagrams. No authentication is
feasible.
3. Heart-beat is sent unidirectionally (S->MC->C). If no content and HB have been absent for too
long, the clients will unsubscribe and try later.
4. Note that a complete frame may comprise of multiple slices so that the size of each slice is
kept strictly smaller that a certain threshold (e.g, 1472 bytes). This is to cope with diffrent
frame size limitations among network adapters.

****************************************** DATA FORMAT ******************************************
========================================= Byte streams ==========================================
Data are in form of byte streams and need to be interpreted by specific implementation. Two forms
are present from S->C:
1. Data frame. "DF\0" + encoded # of following bytes (N) + N bytes of data.
2. Heartbeat. "HB\0".
One type of instruction is effective from C->S (TCP only):
1. Heartbeat. "HB\0".
======================================= Structured data =========================================
Data are in form of structured database language. Three forms are present:
1. Data frame. "DF\0" + encoded # of following bytes (N) + N bytes of data (in form of REQ
SENTENCE + '\0' + encoded # of content in bytes (N2) + content (N2 bytes). Example: "DF\0"
+ [0x21 0x00 0x00 0x21] + "ROBOTSTATUS.ROBOTID\0" + [0x09 0x00 0x00 0x09] + "DDRSZ001\0")
2. Protocol frame. "PF\0" + encoded # of following bytes (N) + N bytes of structure protocol
texts.
3. Heart-beat. "HB\0"
Two types of instruction are effective from C->S (TCP only):
1. Heartbeat. "HB\0".
2. Protocol inquiry. "PI\0".

**************************************** AUTHENTICATION *****************************************
Authorization is sent in form of text strings. For write authorization (WA), the server sends a
string to the client, and if the client can validate it within certain time window, the client
will start receiving data from the server and behave accordingly; otherwise, the connection will
be closed from the client's peer. For read authorization (RA), the client sends a string to the
server, and if the server can validate it within certain time window, the server will start
sending data to the client; otherwise, the connection will be close from the server's peer.
*/

#ifndef __DDR_LOCAL_NODE_H_INCLUDED__
#define __DDR_LOCAL_NODE_H_INCLUDED__

#include "CommLib/CommLibInterface.h"

#ifndef BYTE
typedef unsigned char BYTE;
#endif

namespace DDRLNN
{
	const char DDRLNB_NODEINFO_BROADCASTING_ADDRESS_IP[] = "239.0.0.127";
	const char DDRLNB_UDP_CONTENT_BROADCASTING_ADDRESS_IP[] = "239.0.0.128";
	const unsigned short DDRLNB_NODEINFO_BROADCASTING_ADDRESS_PORT = 28888;
	const BYTE DDRLNB_DATATYPE_ID_STREAM = (BYTE)0x88;
	//const BYTE DDRLNB_DATATYPE_ID_STRUCTURED = (BYTE)0x3F;
	const BYTE DDRLNB_PROTO_ID_TCP = (BYTE)0x5F;
	const BYTE DDRLNB_PROTO_ID_UDP = (BYTE)0x99;
	const BYTE DDRLNB_AUTHTYPE_NOAUTH = (BYTE)0;
	const BYTE DDRLNB_AUTHTYPE_READAUTH = (BYTE)1; // once connected, the client sends to the server authentication info
	const BYTE DDRLNB_AUTHTYPE_WRITEAUTH = (BYTE)2; // once connected, the server sends to the client authentication info
	
	const int UDP_MULTICAST_MAX_SLICE_SIZE = 1450;
}

#endif
