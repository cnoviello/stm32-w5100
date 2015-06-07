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

#include "w5100.h"

#ifdef DEBUG
#include "stm32f4xx_hal_uart.h"
#include <string.h>

UART_HandleTypeDef *ghuart;

void W5100_UART_Debug_Init(UART_HandleTypeDef *huart) {
	ghuart = huart;
}

void W5100_UART_Debug_Print(char *msg) {
	 HAL_UART_Transmit(ghuart, (uint8_t*)msg, strlen(msg), 100);
}

#endif //#ifdef DEBUG


W5100_StatusTypeDef W5100_GetGWIP(W5100_Handle_TypeDef *hw5100, uint8_t *ip) {
	W5100_StatusTypeDef retval;

	for(uint16_t addr = GWIP_ADDR_REG; addr <= GWIP_ADDR_REG + GWIP_ADDR_LEN; addr++)
		retval |= W5100_Read(hw5100, addr, ip++);

	return retval;

}

W5100_StatusTypeDef W5100_GetIP(W5100_Handle_TypeDef *hw5100, uint8_t *ip) {
	W5100_StatusTypeDef retval;

	for(uint8_t addr = IP_ADDR_REG; addr <= IP_ADDR_REG + IP_ADDR_LEN; addr++)
		retval |= W5100_Read(hw5100, addr, ip++);

	return retval;

}

W5100_StatusTypeDef W5100_GetMAC(W5100_Handle_TypeDef *hw5100, uint8_t *mac) {
	W5100_StatusTypeDef retval;

	for(uint8_t addr = MAC_ADDR_REG; addr <= MAC_ADDR_REG + MAC_ADDR_LEN; addr++)
		retval |= W5100_Read(hw5100, addr, mac++);
}

W5100_StatusTypeDef W5100_GetNetMask(W5100_Handle_TypeDef *hw5100, uint8_t *nm) {
	W5100_StatusTypeDef retval;

	for(uint8_t addr = NET_MASK_REG; addr <= NET_MASK_REG + NET_MASK_LEN; addr++)
		retval |= W5100_Read(hw5100, addr, nm++);
}

W5100_StatusTypeDef W5100_Init(W5100_Handle_TypeDef *hw5100) {
	W5100_StatusTypeDef retval;

    retval = W5100_SetIP(hw5100) &&
    		 W5100_SetNetMask(hw5100) &&
			 W5100_SetGWIP(hw5100) &&
			 W5100_SetMAC(hw5100);

    retval |= W5100_Write(hw5100, 0x001A, 0x55); //FIXME: MEMORIA SOCKET, CAMBIARE
    retval |= W5100_Write(hw5100, 0x001B, 0x55); //FIXME: MEMORIA SOCKET, CAMBIARE

    return retval;
}

/**
  * @brief  Write a single byte inside a given W5100 memory register
  * @param  hw5100: pointer to a W5100_Handle_TypeDef structure that contains
  *                the configuration information for W5100 ic.
  * @param  regaddr: first byte of the address location of memory register
  * @param  regl: second byte of the memory register
  * @param  Size: amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
W5100_StatusTypeDef W5100_Write(W5100_Handle_TypeDef *hw5100, uint16_t regaddr, uint8_t data) {
	  HAL_StatusTypeDef status = HAL_OK;

	  /* Every W5100 write command starts with 0xF0 byte, followed by the register address (2 bytes) and data (1 byte) */
	  uint8_t buf[] = {0xF0, regaddr >> 8, regaddr, data};

	  HAL_GPIO_WritePin(hw5100->ssGPIOx, hw5100->ssGPIOpin, GPIO_PIN_RESET); //CS LOW
	  status = HAL_SPI_Transmit(hw5100->hspi, buf, 4, 0xFFFFFFFF);
	  HAL_GPIO_WritePin(hw5100->ssGPIOx, hw5100->ssGPIOpin, GPIO_PIN_SET); //CS HIGH

	  return status;
}

W5100_StatusTypeDef W5100_Read(W5100_Handle_TypeDef *hw5100, uint16_t regaddr, uint8_t *data) {
	  HAL_StatusTypeDef status = HAL_OK;

	  /* Every W5100 read command starts with 0x0F byte, followed by the register address (2 bytes) and data (1 byte) */
	  uint8_t wbuf[] = {0x0F, regaddr >> 8, regaddr, 0x0};
	  uint8_t rbuf[4];

	  HAL_GPIO_WritePin(hw5100->ssGPIOx, hw5100->ssGPIOpin, GPIO_PIN_RESET); //CS LOW
	  status = HAL_SPI_TransmitReceive(hw5100->hspi, wbuf, rbuf, 4, 0xFFFFFFFF);
	  HAL_GPIO_WritePin(hw5100->ssGPIOx, hw5100->ssGPIOpin, GPIO_PIN_SET); //CS HIGH

	  *data = rbuf[3];
	  return status;
}


W5100_StatusTypeDef W5100_SetGWIP(W5100_Handle_TypeDef *hw5100) {
	W5100_StatusTypeDef retval;

	for(uint16_t addr = GWIP_ADDR_REG; addr <= GWIP_ADDR_REG + GWIP_ADDR_LEN; addr++)
		retval |= W5100_Write(hw5100, addr, hw5100->gw[addr - GWIP_ADDR_REG]);

	return retval;
}

W5100_StatusTypeDef W5100_SetIP(W5100_Handle_TypeDef *hw5100) {
	W5100_StatusTypeDef retval;

	for(uint16_t addr = IP_ADDR_REG; addr <= IP_ADDR_REG + IP_ADDR_LEN; addr++)
		retval |= W5100_Write(hw5100, addr, hw5100->ip[addr - IP_ADDR_REG]);

	return retval;
}

W5100_StatusTypeDef W5100_SetMAC(W5100_Handle_TypeDef *hw5100) {
	W5100_StatusTypeDef retval;

	for(uint16_t addr = MAC_ADDR_REG; addr <= MAC_ADDR_REG + MAC_ADDR_LEN; addr++)
		retval |= W5100_Write(hw5100, addr, hw5100->mac[addr - MAC_ADDR_REG]);

	return retval;
}

W5100_StatusTypeDef W5100_SetNetMask(W5100_Handle_TypeDef *hw5100) {
	W5100_StatusTypeDef retval;

	for(uint16_t addr = NET_MASK_REG; addr <= NET_MASK_REG + NET_MASK_LEN; addr++)
		retval |= W5100_Write(hw5100, addr, hw5100->nm[addr - NET_MASK_REG]);

	return retval;
}
