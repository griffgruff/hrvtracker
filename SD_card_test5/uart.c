/******************************************************************************
 *
 * $RCSfile: $
 * $Revision: $
 *
 * This module provides interface routines to the LPC ARM UARTs.
 * Copyright 2004, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 * reduced to see what has to be done for minimum UART-support by mthomas
 *****************************************************************************/

// #warning "this is a reduced version of the R O Software code"

#include "iolpc2103.h"
#include "uart.h"
#include "main.h"

/* on LPC210x: UART0 TX-Pin=P0.2, RX-Pin=P0.1 
   PINSEL0 has to be set to "UART-Function" = Function "01" 
   for Pin 0.0 and 0.1 */
   
#define PINSEL_BITPIN0  0
#define PINSEL_BITPIN1  2
#define PINSEL_BITPIN8  16
#define PINSEL_BITPIN9  18
#define PINSEL_FIRST_ALT_FUNC    1
#define PINSEL_SECOND_ALT_FUNC   2
#define PINSEL_THIRD_ALT_FUNC    3

// Values of Bits 0-3 in PINSEL to activate UART0
#define UART0_PINSEL    ((PINSEL_FIRST_ALT_FUNC<<PINSEL_BITPIN0)|(PINSEL_FIRST_ALT_FUNC<<PINSEL_BITPIN1))
// Mask of Bits 0-4
#define UART0_PINMASK      (0x0000000F)    /* PINSEL0 Mask for UART0 */

// Values of Bits 16-19 in PINSEL to activate UART1
#define UART1_PINSEL    ((PINSEL_FIRST_ALT_FUNC<<PINSEL_BITPIN8)|(PINSEL_FIRST_ALT_FUNC<<PINSEL_BITPIN9))
// Mask of Bits 16-19
#define UART1_PINMASK      (0x000F0000)    /* PINSEL0 Mask for UART1 */


// U0_LCR devisor latch bit 
#define UART0_LCR_DLAB  7
// U1_LCR devisor latch bit 
#define UART1_LCR_DLAB  7



//baud in bps eg:115200
void uart0Init(int baud)
{
  
  unsigned int divisor = peripheralClockFrequency() / (16 * baud);

  //set Line Control Register (8 bit, 1 stop bit, no parity, enable DLAB)
  U0LCR_bit.WLS   = 0x3;    //8 bit
  U0LCR_bit.SBS   = 0x0;    //1 stop bit
  U0LCR_bit.PE    = 0x0;    //no parity
  U0LCR_bit.DLAB  = 0x1;    //enable DLAB
  //with one row
  // U0LCR = 0x83;

  //devisor
  U0DLL = divisor & 0xFF;
  U0DLM = (divisor >> 8) & 0xFF;
  U0LCR &= ~0x80;

  //set functionalite to pins:  port0.0 -> TX0,  port0.1 -> RXD0
  PINSEL0_bit.P0_0 = 0x1;
  PINSEL0_bit.P0_1 = 0x1;
  //with one row
  //PINSEL0 = PINSEL0 & ~0xF | 0x5;  
  U0FCR = UART_FIFO_8;
}

int uart0Putch(int ch)
{
  while (!(U0LSR & ULSR_THRE))          // wait for TX buffer to empty
    continue;                           // also either WDOG() or swap()

  U0THR = (uint8_t)ch;  // put char to Transmit Holding Register
  return (uint8_t)ch;      // return char ("stdio-compatible"?)
}

const char *uart0Puts(const char *string)
{
	char ch;
	
	while ((ch = *string)) {
		if (uart0Putch(ch)<0) break;
		string++;
	}
	
	return string;
}

int uart0TxEmpty(void)
{
  return (U0LSR & (ULSR_THRE | ULSR_TEMT)) == (ULSR_THRE | ULSR_TEMT);
}

void uart0TxFlush(void)
{
  U0FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo
}


/* Returns: character on success, -1 if no character is available */
int uart0Getch(void)
{
  if (U0LSR & ULSR_RDR)                 // check if character is available
    return U0RBR;                       // return character

  return -1;
}


void uart1Init(int baud)
{
  
  unsigned int divisor = peripheralClockFrequency() / (16 * baud);

  //set Line Control Register (8 bit, 1 stop bit, no parity, enable DLAB)
  U1LCR_bit.WLS   = 0x3;    //8 bit
  U1LCR_bit.SBS   = 0x0;    //1 stop bit
  U1LCR_bit.PE    = 0x0;    //no parity
  U1LCR_bit.DLAB  = 0x1;    //enable DLAB
  //with one row
  // U0LCR = 0x83;

  //devisor
  U1DLL = divisor & 0xFF;
  U1DLM = (divisor >> 8) & 0xFF;
  U1LCR &= ~0x80;

  //set functionalite to pins:  port0.8 -> TX1,  port0.9 -> RXD1
  PINSEL0_bit.P0_8 = 0x1;
  PINSEL0_bit.P0_9 = 0x1;
  //with one row
  //PINSEL0 = PINSEL0 & ~0xF | 0x5;  
  U1FCR = UART_FIFO_8;
}

int uart1Putch(int ch)
{
  while (!(U1LSR & ULSR_THRE))          // wait for TX buffer to empty
    continue;                           // also either WDOG() or swap()

  U1THR = (uint8_t)ch;  // put char to Transmit Holding Register
  return (uint8_t)ch;      // return char ("stdio-compatible"?)
}

const char *uart1Puts(const char *string)
{
	char ch;
	
	while ((ch = *string)) {
		if (uart1Putch(ch)<0) break;
		string++;
	}
	
	return string;
}

int uart1TxEmpty(void)
{
  return (U1LSR & (ULSR_THRE | ULSR_TEMT)) == (ULSR_THRE | ULSR_TEMT);
}

void uart1TxFlush(void)
{
  U1FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo
}


/* Returns: character on success, -1 if no character is available */
int uart1Getch(void)
{
  if (U1LSR & ULSR_RDR)                 // check if character is available
    return U1RBR;                       // return character

  return -1;
}
