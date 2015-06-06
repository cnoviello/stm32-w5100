/*
Copyright (c) 2015 Carmine Noviello

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _W5100_STM32_SOCKET_H__
#define _W5100_STM32_SOCKET_H__

#include "w5100.h"
#include "socket.h"

#define SOCK_MAX_NUM 4

#define SOCKn_REG_SIZE	0x0100
#define SOCK0_BASE_ADDR	0x0400
#define SOCK1_BASE_ADDR	0x0500
#define SOCK2_BASE_ADDR	0x0600
#define SOCK3_BASE_ADDR	0x0700

#define SOCK_CMD_OPEN		0x01
#define SOCK_CMD_LISTEN		0x02
#define SOCK_CMD_CONNECT	0x04
#define SOCK_CMD_DISCONNECT	0x08
#define SOCK_CMD_CLOSE		0x10
#define SOCK_CMD_SEND		0x20
#define SOCK_CMD_SEND_MAC	0x21
#define SOCK_CMD_SEND_KEEP	0x22
#define SOCK_CMD_RECV		0x40

#define Sn_MR	 	0x0000 //MODE
#define Sn_CR	 	0x0001 //COMMAND
#define Sn_IR	 	0x0002 //INTERRUPT
#define Sn_SR	 	0x0003 //STATUS
#define Sn_PORT0 	0x0004 //SOURCE PORT
#define Sn_PORT1 	0x0005
#define Sn_DHAR0 	0x0006 //DEST MAC ADDR
#define Sn_DHAR1 	0x0007
#define Sn_DHAR2 	0x0008
#define Sn_DHAR3 	0x0009
#define Sn_DHAR4 	0x000A
#define Sn_DHAR5 	0x000B
#define Sn_DIPR0 	0x000C //DEST IP
#define Sn_DIPR1 	0x000D
#define Sn_DIPR2 	0x000E
#define Sn_DIPR3 	0x000F
#define Sn_DPORT0 	0x0010 //DEST PORT
#define Sn_DPORT1 	0x0011


typedef enum _W5100_SocketStatus {
	BUS_ERROR			= -1,
	SOCK_CLOSED 		= 0x00,
	SOCK_ARP			= 0x01,
	SOCK_INIT			= 0x13,
	SOCK_LISTEN			= 0x14,
	SOCK_SYNSENT		= 0x15,
	SOCK_SYNRECV		= 0x16,
	SOCK_ESTABLISHED	= 0x17,
	SOCK_FIN_WAIT		= 0x18,
	SOCK_CLOSING		= 0x1A,
	SOCK_TIME_WAIT		= 0x1B,
	SOCK_LAST_ACK		= 0x1D,
	SOCK_CLOSE_WAIT		= 0x1C,
	SOCK_UDP			= 0x22,
	SOCK_IPRAW			= 0x32,
	SOCK_MACRAW			= 0x42,
	SOCK_PPPOE			= 0x5F
} W5100_SocketStatus_TypeDef;

typedef enum _W5100_SockConnMode_TypeDef {
	SOCK_MODE_CLIENT	= 0x0,
	SOCK_MODE_SERVER	= 0x1,
} W5100_SockConnMode_TypeDef;

typedef struct _W5100_Socket_TypeDef {
	W5100_Handle_TypeDef *hw5100;
	uint8_t _socketnum; 					// W5100 socket number. This automatically computer by library
	W5100_SockConnMode_TypeDef connmode; 	// Socket connection mode (CLIENT/SERVER)
	uint16_t destportnum;
	uint16_t srcportnum;
	uint8_t *destip;

} W5100_Socket_TypeDef;

/* Private functions */
W5100_SocketStatus_TypeDef __W5100_SocketStatus(W5100_Handle_TypeDef *hw5100, uint8_t socknum);

W5100_StatusTypeDef W5100_SocketInit(W5100_Socket_TypeDef *hsock);
W5100_StatusTypeDef W5100_SocketGetStatus(W5100_Socket_TypeDef *hsock, W5100_SocketStatus_TypeDef *status);
W5100_StatusTypeDef W5100_SocketOpen(W5100_Socket_TypeDef *hsock);

#endif //#ifndef _W5100_STM32_SOCKET_H__
