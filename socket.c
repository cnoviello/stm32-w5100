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

W5100_StatusTypeDef W5100_SocketGetStatus(W5100_Socket_TypeDef *hsock, W5100_SocketStatus_TypeDef *status) {
	*status = __W5100_SocketStatus(hsock->hw5100, hsock->_socketnum);

	if(*status < 0x0)
		return W5100_ERROR;
	return W5100_OK;
}

W5100_StatusTypeDef W5100_SocketInit(W5100_Socket_TypeDef *hsock) {

	hsock->_socketnum = 0x10;

	for(uint8_t socknum = 0; socknum < SOCK_MAX_NUM; socknum++)
		if(__W5100_SocketStatus(hsock->hw5100, socknum) == SOCK_CLOSED) {
			hsock->_socketnum = socknum;
			break;
		}

	if(hsock->_socketnum <= 0x3) { // Found a free socket
		if (hsock->connmode == SOCK_MODE_CLIENT) {
			if(hsock->srcportnum == 0x0)
				hsock->srcportnum = rand();
		}
	}
	return W5100_NOFREESOCK;
}

W5100_StatusTypeDef W5100_SocketOpen(W5100_Socket_TypeDef *hsock) {
	/* FIXME: gestire i casi di non corretta configurazione */
	uint16_t sockbaseaddr = SOCK0_BASE_ADDR + (hsock->_socketnum * SOCKn_REG_SIZE);
	W5100_StatusTypeDef status;
	W5100_SocketStatus_TypeDef sockstatus;

	W5100_SocketGetStatus(hsock, &sockstatus);

	if(sockstatus == SOCK_CLOSED) {
		status = W5100_Write(hsock->hw5100, sockbaseaddr + Sn_CR, SOCK_CMD_OPEN);
		if(status == W5100_OK) {
			while(1) { /* Waits until socket is not opened */
				W5100_SocketGetStatus(hsock, &sockstatus);
				if(sockstatus != SOCK_CLOSED)
					break;
				HAL_Delay(1);
			}
		}
	} else
		return W5100_SOCK_AREADY_OPENED;
}

/* FIXME: Può essere eliminato */
W5100_SocketStatus_TypeDef __W5100_SocketStatus(W5100_Handle_TypeDef *hw5100, uint8_t socknum) {
	uint16_t sockbaseaddr = SOCK0_BASE_ADDR + (socknum * SOCKn_REG_SIZE);
	uint8_t data;

	if(W5100_Read(hw5100, sockbaseaddr + Sn_SR, &data) == W5100_OK)
		return data;
	return -1;
}

