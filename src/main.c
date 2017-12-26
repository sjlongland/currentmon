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

#include <string.h>
#include <assert.h>
#include "LPC8xx.h"
#include "romapi_8xx.h"

/*! Assert handling */
void __assert_func(const char* expr, int line,
		const char* file, const char* func) {
	/* Ignore these */
	(void)expr;
	(void)line;
	(void)file;
	(void)func;

	/* Hang here */
	while(1);
}

/*! I2C control interface */
static I2C_HANDLE_T* i2c;

/*! UART control interface */
static UART_HANDLE_T* uart;

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

	/* ----- I2C setup: master mode at 100kHz bitrate ----- */

	uint32_t _i2c_mem[LPC_I2CD_API->i2c_get_mem_size() + 3/4];
	i2c = LPC_I2CD_API->i2c_setup(LPC_I2C_BASE, _i2c_mem);
	assert(i2c != NULL);
	assert(LPC_I2CD_API->i2c_set_bitrate(i2c,
				SystemCoreClock,
				100000) == LPC_OK);

	/* ----- UART setup ----- */

	uint32_t _uart_mem[LPC_UARTD_API->uart_get_mem_size() + 3/4];
	{
		uint32_t frg_mult;
		UART_CONFIG_T _uart_cfg = {
			.sys_clk_in_hz	= SystemCoreClock,
			.baudrate_in_hz	= 115200,
			.config 	= (0 << 4)	/* 1 stop bit */
					| (0 << 2)	/* No parity */
					| (1 << 0)	/* 8 bits */
					,
			.sync_mod	= 0,		/* Async mode */
			.error_en	= 0,		/* No errors */
		};

		uart = LPC_UARTD_API->uart_setup(LPC_USART0_BASE,
				(uint8_t*)_uart_mem);
		assert(uart != NULL);

		frg_mult = LPC_UARTD_API->uart_init(uart, &_uart_cfg);
		assert(frg_mult != 0);
		LPC_SYSCON->UARTFRGMULT = frg_mult;
		LPC_SYSCON->UARTFRGDIV = 0xff;
	}

	/* ----- NVIC setup ----- */

	NVIC->ISER[0] = ( /* UM10601 3.4.1 */
			(1 << 3) 	/* Enable UART0 interrupts */
	);

	/* ----- Main loop ----- */

	while(1) {
		/* TODO */
	}
}

/*! Interrupt handler for UART0; pass to ROM code */
void UART0_IRQHandler(void) {
	LPC_UARTD_API->uart_isr(uart);
}
