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

#include "socket.h"
#include <stdlib.h>
#include <string.h>
#include "cortexm/ExceptionHandlers.h"

int8_t accept(uint8_t sock) {
	W5100_StatusTypeDef retval;
	int8_t sockstatus, old;
	uint8_t im;

	while (1) {
		sockstatus |= W5100_SocketStatus(sock);
		HAL_Delay(10);
		if(sockstatus != old) {
			char msg[20];
			sprintf(msg, "STATUS: %x\r\n", sockstatus);
			W5100_UART_Debug_Print(msg);
			old = sockstatus;
		}
		if(sockstatus != SOCK_LISTEN) {
			if(sockstatus == SOCK_ESTABLISHED) {
				return 2;
				do  {
					retval |= W5100_Read(Sn_IR(sock), &im);
				} while(retval == W5100_OK && !SOCK_DATA_RECV(im));

				W5100_Write(Sn_IR(sock), 0x1); //Clears IR register
				return retval == W5100_OK && SOCK_DATA_RECV(im) ? 0 : -1;
			}
			else if(sockstatus == SOCK_CLOSED ||
					sockstatus == SOCK_CLOSE_WAIT ||
					sockstatus == SOCK_CLOSING ||
					sockstatus == SOCK_TIME_WAIT ||
					sockstatus == SOCK_LAST_ACK ||
					sockstatus == SOCK_FIN_WAIT)
				return -1;
//			else
//				__DEBUG_BKPT();
		}
	}

	/* FIXME gestire i casi di incoerenza della socket */
}

int8_t bind(uint8_t sock, uint8_t *ip, uint16_t port) {
	W5100_StatusTypeDef retval;
	uint8_t sockstatus;

	if(sock <= 3) { // Found a free socket
		W5100_StatusTypeDef retval = W5100_OK;

		retval |= W5100_Write(Sn_IR(sock), 0xFF); //Resets Interrupt status register

//		if (retval == W5100_OK) { //CLIENT
//			//Set source port number
//			retval |= W5100_Write(Sn_PORT0(sock), port >> 8);
//			retval |= W5100_Write(Sn_PORT0(sock) + 1, port);
//
//			//Set destination IP
//			retval |= W5100_Write(Sn_DIPR0(sock), ip[0]);
//			retval |= W5100_Write(Sn_DIPR0(sock) + 1, ip[1]);
//			retval |= W5100_Write(Sn_DIPR0(sock) + 2, ip[2]);
//			retval |= W5100_Write(Sn_DIPR0(sock) + 3, ip[3]);
//
////			//Set destination port number
////			retval |= W5100_Write(Sn_DPORT0(sock), hsock->destportnum >> 8);
////			retval |= W5100_Write(Sn_DPORT0(sock) + 1, hsock->destportnum);
//		}

		if (retval == W5100_OK) {
			uint8_t sockmode = 0x0;

			//Set source port number
			retval |= W5100_Write(Sn_PORT0(sock), port  >> 8);
			retval |= W5100_Write(Sn_PORT0(sock) + 1, port);

			if(W5100_Write(Sn_CR(sock), SOCK_CMD_OPEN) == W5100_OK) {
				while(1) { /* Waits until socket is not opened */
					sockstatus = W5100_SocketStatus(sock);
					if(sockstatus == SOCK_INIT ||
					   sockstatus == SOCK_UDP  ||
					   sockstatus == SOCK_IPRAW)
						break;
				}
			}

		}

		if(retval == W5100_OK)
			return 0;
	}
	return -1;
}

int close(int sock ) {
	uint8_t cr, im;

	if(sock < 0)
		return -1;

	if(W5100_SocketStatus(sock) != 0) {
		W5100_Write(Sn_CR(sock), SOCK_CMD_DISCONNECT);

		do { /* Wait command to complete */
			W5100_Read(Sn_IR(sock), &im);
		} while(!SOCK_DISCONNECTED(im) && !SOCK_TIMEOUT(im));

		W5100_Write(Sn_IR(sock), 0x1); //Clears IR register

		do { /* Wait command to complete */
			W5100_Read(Sn_CR(sock), &cr);
		} while(cr);
	}
}

int8_t connect(uint8_t sock) {
//	W5100_SocketStatus_TypeDef 				sockstatus;
//	W5100_SocketInterruptStatusMask_TypeDef sockintmask;
//
//#ifdef DEBUG
//	W5100_SocketStatus_TypeDef oldsockstatus;
//	W5100_SocketInterruptStatusMask_TypeDef oldsockintmask;
//	char msg[40];
//#endif
//
//	/* Let's try connecting to remote application */
//	if(W5100_Write(Sn_CR(sock), SOCK_CMD_CONNECT) == W5100_OK) {
//		uint8_t cr = 0;
//		do { /* Wait command to complete */
//			W5100_Read(Sn_CR(sock), &cr);
//			HAL_Delay(1);
//		} while(cr);
//
//		while (1) { /* Let's check if connection is established */
//			W5100_SocketStatus(hsock, &sockstatus);
//			W5100_SocketInterruptStatus(hsock, &sockintmask);
//
//			#ifdef DEBUG
//			if(oldsockstatus != sockstatus) {
//				sprintf(msg, "SocketConnect::SOCK STATUS: %x\r\n", sockstatus);
//				W5100_UART_Debug_Print(msg);
//				oldsockstatus = sockstatus;
//			}
//
//			if(oldsockintmask != sockintmask) {
//				sprintf(msg, "SocketConnect::SOCK INTERRUPT MASK: %x\r\n", sockintmask);
//				W5100_UART_Debug_Print(msg);
//				sprintf(msg, "Socket Number:: %x\r\n", sock);
//				W5100_UART_Debug_Print(msg);
//
//				oldsockintmask = sockintmask;
//			}
//#endif
//			if(sockstatus == SOCK_CLOSED || sockstatus == SOCK_CLOSE_WAIT) { /* Trying to understand why socket is closed */
//				if((sockintmask & DISCONNECTED) == DISCONNECTED) /* Connection disconnected or remote app not running */
//					return W5100_CONN_RESET;
//
//				if((sockintmask & TIMEOUT) == TIMEOUT) /* Timeout trying to connect to remote application */
//					return W5100_TIMEOUT;
//
//			} else if(sockstatus == SOCK_ESTABLISHED)
//				return W5100_OK;
//
//			HAL_Delay(1);
//		}
//		return W5100_OK;
//	}
	return W5100_ERROR;
}

int8_t socket(uint8_t domain, uint8_t type, uint8_t protocol) {
	/* FIXME: aggiungere i controlli, tipo vedere il protocollo, ecc */
	int8_t sockstatus;
	int8_t sock = -1;

	for(uint8_t socknum = 0; socknum < SOCK_MAX_NUM; socknum++) {
		if(W5100_SocketStatus(socknum) == SOCK_CLOSED) {
			sock = socknum;
			break;
		}
	}

	if(sock >= 0) {
		uint8_t sockmode = 0x0;

		//Set protocol type
		sockmode |= protocol;

		return W5100_Write(Sn_MR(sock), sockmode) == W5100_OK ? sock : -1;
	}

	return sock;
}

int8_t listen(uint8_t sock, uint8_t backlog) {
	W5100_StatusTypeDef retval = W5100_OK;
	uint8_t cr;

	if(W5100_Write(Sn_CR(sock), SOCK_CMD_LISTEN) == W5100_OK) {
		uint8_t cr = 0;
		do { /* Wait command to complete */
			retval |= W5100_Read(Sn_CR(sock), &cr);
		} while(cr);

		if (retval == W5100_OK)
			return 0;
	}
	return -1;
}

int16_t recv(uint8_t sock, uint8_t *buf, uint16_t len, uint8_t flags) {
	uint16_t Sn_RX_RD = 0,
			 offset   = 0,
			 memaddr  = 0,
			 freetx   = 0,
			 sendlen  = 0;

	uint16_t mylen;
	uint8_t im=0;
	char msg[20];

	do {
		if(W5100_SocketStatus(sock) != SOCK_ESTABLISHED)
			return 0;

		while(1) {
			W5100_Read(Sn_IR(sock), &im);
			if(SOCK_DATA_RECV(im))
				break;
		}

		W5100_Write(Sn_IR(sock), 0x1); //Clears IR register

		sprintf(msg, "DATA ARRIVED\n\r");
		W5100_UART_Debug_Print(msg);

		W5100_Read(Sn_RX_RSR0(sock), (uint8_t*)&mylen);
		mylen <<= 8;
		W5100_Read(Sn_RX_RSR0(sock) + 1, (uint8_t*)&mylen);

		W5100_Read(Sn_RX_RD0(sock), (uint8_t*)&Sn_RX_RD);
		Sn_RX_RD <<= 8;
		W5100_Read(Sn_RX_RD0(sock) + 1, (uint8_t*)&Sn_RX_RD);

		if(mylen > 0)
		{
			for(int i=0; i < mylen; i++) {
				memaddr = 0x6000 + (Sn_RX_RD++ & 0x7ff);
				W5100_Read(memaddr++, buf);
				buf++;
			}

			W5100_Write(Sn_RX_RD0(sock), Sn_RX_RD >> 8);
			W5100_Write(Sn_RX_RD0(sock) + 1, Sn_RX_RD);
			W5100_Write(Sn_CR(sock), SOCK_CMD_RECV);
		}
		sprintf(msg, "DATA PROCESSED\n\r");
		W5100_UART_Debug_Print(msg);

		return mylen;
	} while(0);
}

int8_t send(uint8_t sock, uint8_t *buf, uint16_t len) {
	uint16_t Sn_TX_WR = 0,
			 offset   = 0,
			 memaddr  = 0,
			 freetx   = 0,
			 sendlen  = 0;

	uint8_t im;

	do {
//		W5100_SocketStatus(hsock, &sst);
//		W5100_SocketInterruptStatus(hsock, &im);
//		if(sst != SOCK_ESTABLISHED) {
//			__DEBUG_BKPT();
//			return W5100_SOCK_CLOSED;
//
//		}
//		do {
//			W5100_SocketStatus(hsock, &sst);
//			HAL_Delay(1);
//		} while(sst != SOCK_ESTABLISHED);
		char msg[20];

		sprintf(msg, "Connect: %x", W5100_SocketStatus(sock));
		W5100_UART_Debug_Print(msg);

//		if((im & DISCONNECTED == DISCONNECTED) ||(im & TIMEOUT == TIMEOUT))
//			return W5100_SOCK_CLOSED;

		freetx = __W5100_SocketFreeTXMEM(sock);

		if(len > freetx)
			sendlen = freetx;
		else
			sendlen = len;

		Sn_TX_WR = 0;
		W5100_Read(Sn_TX_WR0(sock), (uint8_t*)&Sn_TX_WR);
		Sn_TX_WR <<= 8;
		W5100_Read(Sn_TX_WR0(sock) + 1, (uint8_t*)&Sn_TX_WR);

		/* Sn_TX_WR now contains the point where starting to place data in the circular buffer */
		for(int i=0; i < sendlen; i++) {
			/* Modular math ensure that we fill the right memory locations on the circular buffer */
			memaddr = 0x4000 + (Sn_TX_WR++ & 0x7ff);
			W5100_Write(memaddr++, *buf);
			buf++;
		}
		len -=sendlen;

		W5100_Write(Sn_TX_WR0(sock), Sn_TX_WR >> 8);
		W5100_Write(Sn_TX_WR0(sock) + 1, Sn_TX_WR);
		W5100_Write(Sn_CR(sock), SOCK_CMD_SEND);

		do {
			W5100_Read(Sn_IR(sock), &im);
		} while(!SOCK_DATA_SENT(im));

		W5100_Write(Sn_IR(sock), 0x1); //Clears IR register

	} while(len > 0);
}

int8_t W5100_SocketStatus(uint8_t sock) {
	int8_t sockstatus;

	if(W5100_Read(Sn_SR(sock), (uint8_t*)&sockstatus) == W5100_OK)
		return sockstatus;

	return -1;
}

uint16_t __W5100_SocketFreeTXMEM(uint8_t sock){
	W5100_StatusTypeDef retval = W5100_OK;
	uint16_t fmem;

	retval |= W5100_Read(Sn_TX_FSR0(sock),  (uint8_t*)&fmem);
	fmem <<= 8;
	retval |= W5100_Read(Sn_TX_FSR0(sock) + 1,  (uint8_t*)&fmem);

	if(retval == W5100_OK)
		return fmem;
	return -1;
}
