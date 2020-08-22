// Tiny printf implementation by oPossum from here:
// http://forum.43oh.com/topic/1289-tiny-printf-c-version/#entry10652
//
// NOTE: We are not using the libc printf because it's huge:
// https://e2e.ti.com/support/development_tools/compiler/f/343/t/442632

// There are 7 format specifiers:
//
// %c - Character
//
// %s - String
//
// %i - signed Integer (16 bit)
//
// %u - Unsigned integer (16 bit)
//
// %l - signed Long (32 bit)
//
// %n - uNsigned loNg (32 bit)
//
// %x - heXadecimal (16 bit)
//
// Field width, floating point and other standard printf() features are not supported.


#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <msp430.h>
#include "printf.h"
//------------UART cable communication--------------

// The I/O backend library
int io_putchar(int c) {
    while (!(UCA0IFG&UCTXIFG));                // USCI_A0 TX buffer ready?
    UCA0TXBUF = c;
    return 0;
}

int io_puts_no_newline(const char *str) {
  uint8_t len_str = strlen(str);
  volatile uint8_t i;
  for(i=0;i<len_str;i++) {
    while (!(UCA0IFG&UCTXIFG));                // USCI_A0 TX buffer ready?
    UCA0TXBUF = str[i];
  }
  return 0;
}

void init_clocks(){
    // Startup clock system with max DCO setting ~8MHz
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK; //ACLK set to VLOCLK which is 10kHz
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set ACLK to oscilate to 10kHz
    CSCTL0_H = 0;                           // Lock CS registers
}

void init_uart() {
    // Pins, 2.1/UCA0RXD (to MCU), 2.0/UCA0TXD (from MCU)
    P2REN = 0;
    P2SEL1 |= (BIT1 | BIT0);
    P2SEL0 &= ~(BIT1 | BIT0);

    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                    // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL_2;             // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BRW = 52;                           // 8000000/16/9600
    //UCA0BR1 = 0x00;
    UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA0CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
}

#define PUTC(c) io_putchar(c)

static const unsigned long dv[] = {
//  4294967296      // 32 bit unsigned max
    1000000000,     // +0
     100000000,     // +1
      10000000,     // +2
       1000000,     // +3
        100000,     // +4
//       65535      // 16 bit unsigned max
         10000,     // +5
          1000,     // +6
           100,     // +7
            10,     // +8
             1,     // +9
};

static void xtoa(unsigned long x, const unsigned long *dp)
{
    char c;
    unsigned long d;
    if(x) {
        while(x < *dp) ++dp;
        do {
            d = *dp++;
            c = '0';
            while(x >= d) ++c, x -= d;
            PUTC(c);
        } while(!(d & 1));
    } else
        PUTC('0');
}

static void puth(unsigned n)
{
    static const char hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    PUTC(hex[n & 15]);
}

int printf(char *format, ...)
{
    char c;
    int i;
    long n;
    int fill_zeros;
    unsigned d;

    va_list a;
    va_start(a, format);
    while((c = *format++)) {
        if(c == '%') {
            fill_zeros = 0;
parse_fmt_char:
            switch(c = *format++) {
                case 's':                       // String
                    io_puts_no_newline(va_arg(a, char*));
                    break;
                case 'c':                       // Char
                    PUTC(va_arg(a, int)); // TODO: 'char' generated a warning
                    break;
                case 'i':                       // 16 bit Integer
                case 'u':                       // 16 bit Unsigned
                    i = va_arg(a, int);
                    if(c == 'i' && i < 0) i = -i, PUTC('-');
                    xtoa((unsigned)i, dv + 5);
                    break;
                case 'l':                       // 32 bit Long
                case 'n':                       // 32 bit uNsigned loNg
                    n = va_arg(a, long);
                    if(c == 'l' &&  n < 0) n = -n, PUTC('-');
                    xtoa((unsigned long)n, dv);
                    break;
                case 'x':                       // 16 bit heXadecimal
                    i = va_arg(a, int);
                    d = i >> 12;
                    if (d > 0 || fill_zeros >= 4)
                        puth(d);
                    d = i >> 8;
                    if (d > 0 || fill_zeros >= 3)
                        puth(d);
                    d = i >> 4;
                    if (d > 0 || fill_zeros >= 2)
                        puth(d);
                    puth(i);
                    break;
                case '0':
                    c = *format++;
                    fill_zeros = c - '0';
                    goto parse_fmt_char;
                case 0: return 0;
                default: goto bad_fmt;
            }
        } else
bad_fmt:    PUTC(c);
    }
    va_end(a);
    return 0; // TODO: return number of chars printed
}
