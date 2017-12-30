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

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "LPC8xx.h"
#include "romapi_8xx.h"

/*! UART control interface */
UART_HANDLE_T* uart;

void putchar(const char c);

void uart_init() {
	uint32_t _uart_mem[LPC_UARTD_API->uart_get_mem_size() + 3/4];
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

/*! Interrupt handler for UART0; pass to ROM code */
void UART0_IRQHandler(void) {
	LPC_UARTD_API->uart_isr(uart);
}

/* ----- stdio glue ----- */

void putchar(const char c) {
	LPC_UARTD_API->uart_put_char(uart, (uint8_t)c);
}

int puts(const char* str) {
	UART_PARAM_T p = {
		.buffer = (uint8_t*)str,
		.size = 0,
		.transfer_mode = 0x01,
		.driver_mode = 0x00,
		.callback_func_pt = NULL,
		.dma_req_func_pt = NULL
	};
	LPC_UARTD_API->uart_put_line(uart, &p);
	return 0;
}
