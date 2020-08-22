/*
 * uart_cable.h
 *
 *  Created on: Aug 21, 2020
 *      Author: Kangabire
 */

#ifndef PRINTF_H_
#define PRINTF_H_

int io_putchar(int c);
int io_puts_no_newline(const char *str);
int printf(char *format, ...);
void init_clocks();
void init_uart();

#endif /* PRINTF_H_ */
