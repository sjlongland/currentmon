/*!
 * DC power monitor
 * (C) Stuart Longland <me@vk4msl.id.au>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include "LPC8xx.h"

/* ----- Core application ----- */

int main(void) {
	/* ----- Clock setup ----- */

	LPC_SYSCON->SYSAHBCLKCTRL |= ( /* UM10601 4.6.13 */
		(1 <<  5) |	/* I2C */
		(1 <<  6) |	/* GPIO */
		(1 <<  7) |	/* Switch matrix */
		(1 << 14)	/* UART0 */
	);
	LPC_SYSCON->PRESETCTRL = (
		(1 <<  0) |	/* assert SPI0 reset */
		(1 <<  1) |	/* assert SPI1 reset */
		(0 <<  2) |	/* disassert UARTFRG reset */
		(0 <<  3) |	/* disassert UART0 reset */
		(1 <<  4) |	/* assert UART1 reset */
		(1 <<  5) |	/* assert UART2 reset */
		(0 <<  6) |	/* disassert I2C reset */
		(1 <<  7) |	/* assert MRT reset */
		(1 <<  8) |	/* assert SCT reset */
		(1 <<  9) |	/* assert WKT reset */
		(0 << 10) |	/* disassert GPIO reset */
		(1 << 11) |	/* assert FLASH reset */
		(1 << 12)	/* assert ACMP reset */
	);

	/* ----- Switch matrix setup ----- */

	/* Assign pins */
	LPC_SWM->PINASSIGN0 = (	/* UM10601 9.5.1 */
		/* U0_CTS_I = No Connect */
		(0xfful << 24) |
		/* U0_RTS_O = No Connect */
		(0xfful << 16) |
		/* U0_RXD_I = GPIO 0.0, Pin 8 */
		(0x00ul << 8) |
		/* U0_TXD_O = GPIO 0.4, Pin 2 */
		(0x04ul << 0)
	);
	LPC_SWM->PINASSIGN1 = ( /* UM10601 9.5.2 */
		/* All: no connect */
		0xfffffffful
	);
	LPC_SWM->PINASSIGN2 = ( /* UM10601 9.5.3 */
		/* All: no connect */
		0xfffffffful
	);
	LPC_SWM->PINASSIGN3 = (	/* UM10601 9.5.4 */
		/* All: no connect */
		0xfffffffful
	);
	LPC_SWM->PINASSIGN4 = ( /* UM10601 9.5.5 */
		/* All: no connect */
		0xfffffffful
	);
	LPC_SWM->PINASSIGN5 = ( /* UM10601 9.5.6 */
		/* All: no connect */
		0xfffffffful
	);
	LPC_SWM->PINASSIGN6 = ( /* UM10601 9.5.7 */
		/* All: no connect */
		0xfffffffful
	);
	LPC_SWM->PINASSIGN7 = ( /* UM10601 9.5.8 */
		/* I2C_SDA_IO: GPIO 0.4, Pin 4 */
		(0x04ul << 24) |
		/* CTOUT_3_O: No connect */
		(0xfful << 16) |
		/* CTOUT_2_0: No connect */
		(0xfful << 8) |
		/* CTOUT_1_0: No connect */
		(0xfful << 0)
	);
	LPC_SWM->PINASSIGN8 = ( /* UM10601 9.5.9 */
		/* GPIO_INT_BMAT_O: no connect */
		(0xfful << 24) |
		/* CLKOUT_O: no connect */
		(0xfful << 16) |
		/* ACMP_O_O: no connect */
		(0xfful << 8) |
		/* I2C_SCL_IO: GPIO 0.3, Pin 3 */
		(0x03ul << 0)
	);
	LPC_SWM->PINENABLE0 = ( /* UM10601 9.5.10 */
		(0 << 0) |	/* ACMP_I1_EN */
		(0 << 1) |	/* ACMP_I2_EN */
		(0 << 2) |	/* SWDCLK_EN */
		(0 << 3) |	/* SWDIO_EN */
		(0 << 4) |	/* XTALIN_EN */
		(0 << 5) |	/* XTALOUT_EN */
		(1 << 6) |	/* RESET_EN */
		(0 << 7) |	/* CLKIN */
		(0 << 8)	/* VDDCMP */
	);

	/*
	 * At this point, we should have the following pin assignments:
	 * 1: nRESET	(power on default)
	 * 2: TxD
	 * 3: SCL
	 * 4: SDA
	 * 5: GPIO	(not used)
	 * 6: 3v3
	 * 7: 0v
	 * 8: RxD
	 */

	/* ----- GPIO setup ----- */

	/*
	 * GPIO 0_1 (aka pin 5) will be an output, driven low to ensure we
	 * don't drain unnecessary power or leave an input flapping by
	 * mistake.
	 */
	LPC_GPIO_PORT->DIR0 = 1 << 1;
	LPC_GPIO_PORT->CLR0 = 1 << 1;

	/* ----- I2C setup ----- */

	LPC_I2C->CFG = (1 << 0);	/* Enable master */

	/* ----- NVIC setup ----- */

	NVIC->ISER[0] = ( /* UM10601 3.4.1 */
			(1 << 3) |	/* Enable UART0 interrupts */
			(1 << 8)	/* Enable I2C interrupts */
	);

	/* ----- Main loop ----- */

	while(1) {
		/* TODO */
	}
}

void I2C_IRQHandler(void) {
	/* TODO */
}
