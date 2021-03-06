/*
 * uartConst.h --
 *
 *     Internal declarations of constants for the SPUR Signetics SCN68681
 *     Dual Universal Asynchronous Receiver/Transmitter (DUART) chip.

 *     For an explanation of the use of the chip see the "SPUR Memory
 *     System Architecture" (MSA), Report. UCB/CSD 87/394.
 *
 * Copyright 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header$ SPRITE (Berkeley)
 */

#ifndef _UARTCONST
#define _UARTCONST

/*
 * Register address offsets in uart physical address space.
 * See MSA Table 3.1.
 *
 * CHANNEL_A_OFFSET     -	offset for all channel A regs
 * CHANNEL_B_OFFSET     -	offset for all channel B regs
 * MODE 		-	mode register (rd/wr)
 * XFER_STATUS 		-	status register (rd)
 * BAUD 		-	baud rate (wr)
 * COMMAND 		-	command register (wr)
 * XFER_REG 		-	access to RX and TX fifo (rd/wr)
 * INPUT_PORT_CHANGE	-	reports changes in input bits (rd)
 * AUX_CTL 		-	auxiliary control register (wr)
 * INTR_STATUS 		-	interrupt status register (rd)
 * INTR_MASK 		-	interrupt mask register (wr)
 * INTR_VEC 		-	interrupt vector register (rd/wr)
 * INPUT_PORT_REG	-	input port register, for flow control (rd)
 * OUTPUT_PORT_CONF	-	output port configuration, for flow control
 *						(wr)
 */

#define CHANNEL_A_OFFSET   0x00
#define CHANNEL_B_OFFSET   0x20

#define MODE		   0x00
#define XFER_STATUS	   0x04
#define BAUD		   0x04
#define COMMAND		   0x08
#define XFER_REG	   0x0c
#define INPUT_PORT_CHANGE  0x10
#define AUX_CTL		   0x10
#define INTR_STATUS	   0x14
#define INTR_MASK	   0x14
#define INTR_VEC	   0x30
#define INPUT_PORT_REG 	   0x34
#define OUTPUT_PORT_CONF   0x34




/*-------------------------------------------------------------------------
 *
 * Read only registers
 *
 */

/*
 * Status register bit definitions (see MSA Table 3.3).
 * RXD_BREAK		     - received a break 
 * RXD_MISSING_STOP_BIT	     - "framing error" on last byte 
 * RXD_PARITY_ERROR	     - parity, if on, was wrong 
 * RXD_FIFO_OVRFLW	     - didn't read out fast enough 
 * TX_EMPTY		     - transmit fifo empty 
 * TX_NOT_FULL		     - still room left in tx fifo 
 * RX_FULL		     - no room left in receive fifo 
 * RX_NOT_EMPTY		     - something in the receive fifo 
 *
 * RXD_ERROR_MASK	     - mask to check error bits only 
 */
#define RXD_BREAK		0x80	/* received a break */
#define RXD_MISSING_STOP_BIT	0x40	/* "framing error" on last byte */
#define RXD_PARITY_ERROR	0x20	/* parity, if on, was wrong */
#define RXD_FIFO_OVRFLW		0x10	/* didn't read out fast enough */
#define TX_EMPTY		0x08	/* transmit fifo empty */
#define TX_NOT_FULL		0x04	/* still room left in tx fifo */
#define RX_FULL			0x02	/* no room left in receive fifo */
#define RX_NOT_EMPTY		0x01	/* something in the receive fifo */

#define RXD_ERROR_MASK		0xF0	/* check error bits only */

/*
 * Interrupt status register bit definitions (see MSA Table 3.4).
 *
 * CHG_IP_3_0		- uart input port bits have changed 
 *			  	reset by reading IPCR 
 * CHG_B_BREAK		- break state changed on channel B 
 *			  	reset by CLR_BRK_STATUS in command reg 
 * B_FIFO_INTR		- channel B fifo rx intr (config by MR1) 
 *			  	reset by reading from channel B 
 * B_TX_RDY		- channel B fifo tx ready for more 
 *			  	reset by writing to channel B 
 * TIMER_INTR		- counter or timer has rolled over 
 *			  	reset by read at offset 0x3c (stop timer) 
 * CHG_A_BREAK		- break state changed on channel A 
 * A_FIFO_INTR		- channel A fifo rx intr (config by MR1) 
 * A_TX_RDY		- channel A fifo tx ready for more
 */

#define CHG_IP_3_0	0x80	
#define CHG_B_BREAK	0x40	
#define B_FIFO_INTR	0x20	
#define B_TX_RDY	0x10	
#define TIMER_INTR	0x08	
#define CHG_A_BREAK	0x04	
#define A_FIFO_INTR	0x02	
#define A_TX_RDY	0x01	


/*----------------------------------------------------------------------------
 *
 * Read/write registers
 *
 */

/*
 * Mode register setup values:
 * first write after RESET_MODE1 writes MODE1, subsequent
 * writes access MODE2.  So far, only have set of standard values:
 *  	MODE1_VAL	- 8 bits, no parity, no flow control, receive
 *			  interrupts if not empty.
 *	MODE2_VAL	- 1 stop bit, no flow control from other end.
 *	MODE2_LOOPBACK 	- same as MODE2_VAL but in LOOPBACK mode.
 *
 */

#define MODE1_VAL	0x13
#define MODE2_VAL 	0x07
#define MODE2_LOOPBACK 	0x87

/*---------------------------------------------------------------------------
 *
 * Write only registers
 *
 */

/*
 * Baud rate values.
 */
#define BAUD_9600	0xbb
#define BAUD_2400	0x88
#define BAUD_1200	0x66
#define BAUD_110	0x11
#define BAUD_19200	0xcc

/*
 * Command register control values (see MSA Table 3.2).
 * There are three independent fields: receive (rx) control,
 * transmit (tx) control, overall control.
 *     RX_NOOP		- does not affect receive status
 *     RX_ON		- enable receive
 *     RX_OFF		- disable receive
 * 
 *     TX_NOOP		- does not affect transmit status
 *     TX_ON		- enable transmit
 *     TX_OFF		- disable transmit
 * 
 *     MISC_NOOP        - no effect on other modes
 *     RESET_MR1	- next write to MODE register access MR1 
 *     RESET_RX		- reset RX: dump fifo, status, etc 
 *     RESET_TX		- reset TX: dump fifo, status, etc 
 *     CLR_ERR_STATUS	- reset RXD error bits in status reg 
 *     CLR_BRK_STATUS	- reset break status in status regs 
 *     ENTER_TX_BREAK	- start transmitting break forever 
 *     EXIT_TX_BREAK	- stop transmitting break 
 */

#define RX_NOOP		0x0
#define RX_ON		0x1
#define RX_OFF		0x2
#define TX_NOOP		0x0
#define TX_ON		0x4
#define TX_OFF		0x8
#define MISC_NOOP	0x00
#define RESET_MR1	0x10
#define RESET_RX	0x20
#define RESET_TX	0x30
#define CLR_ERR_STATUS	0x40
#define CLR_BRK_STATUS	0x50
#define ENTER_TX_BREAK	0x60
#define EXIT_TX_BREAK	0x70


/*
 * Auxiliary control register values.
 *    AUX_CMD_VAL		- ignore counter/timer and input port bits
 *    AUX_CMD_3.7MHZ_TIMER	- set timer freq to 3.6864 MHz
 */

#define AUX_CMD_VAL		0x80
#define AUX_CMD_3.7MHZ_TIMER	0xE0

/*
 * Lint complains about doing an OR of an Address and an int.  This
 * macro handles the casting.
 */
#define ADDR_OFFSET(addr, offset) ((Address) (((int) (addr)) | (offset)))

#endif _UARTCONST




