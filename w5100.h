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

#ifndef _W5100_STM32_H__
#define _W5100_STM32_H__

#include <stdint.h>
#include "stm32f4xx_hal.h" //<--- Change is according your HAL (F1, F0, ecc)

/* W5100 REGISTER DEFINITIONS */

#define GAR0	0x0001 	//GATEWAY
#define GAR1	0x0002
#define GAR2	0x0003
#define GAR3	0x0004

#define SUBR0	0x0005 	//NETMASK
#define SUBR1	0x0006
#define SUBR2	0x0007
#define SUBR3	0x0008

#define SHAR0	0x0009 	//MAC ADDRESS
#define SHAR1	0x000A
#define SHAR2	0x000B
#define SHAR3	0x000C
#define SHAR4	0x000D
#define SHAR5	0x000E

#define SIPR0	0x000F 	//IP ADDRESS
#define SIPR1	0x0010
#define SIPR2	0x0011
#define SIPR3	0x0012


#define GWIP_ADDR_REG 	GAR0
#define GWIP_ADDR_LEN 	GAR3-GAR0
#define IP_ADDR_REG 	SIPR0
#define IP_ADDR_LEN 	SIPR3-SIPR0
#define NET_MASK_REG 	SUBR0
#define NET_MASK_LEN 	SUBR3-SUBR0
#define MAC_ADDR_REG 	SHAR0
#define MAC_ADDR_LEN 	SHAR5-SHAR0

typedef enum _W5100_StatusTypeDef
{
  W5100_OK       			= 0x00,
  W5100_ERROR    			= 0x01,
  W5100_BUSY     			= 0x02,
  W5100_TIMEOUT	 			= 0x03,
  W5100_NOFREESOCK			= 0x04, // No available free socket
  W5100_SOCK_AREADY_OPENED	= 0x05  // Socket already opened
} W5100_StatusTypeDef;

typedef struct _W5100_Handle_TypeDef {
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef* ssGPIOx;
	uint16_t ssGPIOpin;
	uint8_t *dns;
	uint8_t *gw;
	uint8_t *ip;
	uint8_t *mac;
	uint8_t *nm;
} W5100_Handle_TypeDef;

W5100_StatusTypeDef W5100_GetGWIP(W5100_Handle_TypeDef *hw5100, uint8_t *ip);
W5100_StatusTypeDef W5100_GetIP(W5100_Handle_TypeDef *hw5100, uint8_t *ip);
W5100_StatusTypeDef W5100_GetMAC(W5100_Handle_TypeDef *hw5100, uint8_t *mac);
W5100_StatusTypeDef W5100_GetNetMask(W5100_Handle_TypeDef *hw5100, uint8_t *nm);
W5100_StatusTypeDef W5100_Init(W5100_Handle_TypeDef *hw5100);
W5100_StatusTypeDef W5100_Read(W5100_Handle_TypeDef *hw5100, uint16_t regaddr, uint8_t *data);
W5100_StatusTypeDef W5100_Write(W5100_Handle_TypeDef *hw5100, uint16_t regaddr, uint8_t data);
W5100_StatusTypeDef W5100_SetGWIP(W5100_Handle_TypeDef *hw5100);
W5100_StatusTypeDef W5100_SetIP(W5100_Handle_TypeDef *hw5100);
W5100_StatusTypeDef W5100_SetMAC(W5100_Handle_TypeDef *hw5100);
W5100_StatusTypeDef W5100_SetNetMask(W5100_Handle_TypeDef *hw5100);

#endif //#ifndef _W5100_STM32_H__
