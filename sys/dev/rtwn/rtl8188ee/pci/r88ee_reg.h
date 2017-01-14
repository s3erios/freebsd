/*	$OpenBSD: if_rtwnreg.h,v 1.3 2015/06/14 08:02:47 stsp Exp $	*/

/*-
 * Copyright (c) 2010 Damien Bergamini <damien.bergamini@free.fr>
 * Copyright (c) 2015 Stefan Sperling <stsp@openbsd.org>
 * Copyright (c) 2016 Andriy Voskoboinyk <avos@FreeBSD.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * $FreeBSD$
 */

#ifndef R88EE_REG_H
#define R88EE_REG_H

#include <dev/rtwn/rtl8188ee/r88ee_reg.h>

/*
 * MAC registers.
 */
/* System Configuration. */
#define R88EE_PCIE_MIO_INTF		0x0e4 // DONE
#define R88EE_PCIE_MIO_INTD		0x0e8 // DONE
/* PCIe Configuration. */
#define R88EE_PCIE_CTRL_REG		0x300 // DONE
#define R88EE_INT_MIG			0x304 // DONE
#define R88EE_BCNQ_DESA			0x308 // DONE
#define R88EE_HQ_DESA			0x310 // DONE
#define R88EE_MGQ_DESA			0x318 // DONE
#define R88EE_VOQ_DESA			0x320 // DONE
#define R88EE_VIQ_DESA			0x328 // DONE
#define R88EE_BEQ_DESA			0x330 // DONE
#define R88EE_BKQ_DESA			0x338 // DONE
#define R88EE_RX_DESA			0x340 // DONE
#define R88EE_DBI			0x348 // DONE
#define R88EE_MDIO			0x354 // DONE
#define R88EE_DBG_SEL			0x360 // DONE
#define R88EE_PCIE_HRPWM		0x361 // DONE
#define R88EE_PCIE_HCPWM		0x363 // DONE
#define R88EE_UART_CTRL			0x364 // DONE
#define R88EE_UART_TX_DES		0x370 // DONE
#define R88EE_UART_RX_DES		0x378 // DONE


/* Bits for R88EE_GPIO_MUXCFG. */
#define R88EE_GPIO_MUXCFG_RFKILL		0x0040 // Was 0x0008, REG_GPIO_MUXCFG

/* Bits for R88EE_GPIO_IO_SEL. */
#define R88EE_GPIO_IO_SEL_RFKILL		0x0042 // Was 0x0008, REG_GPIO_IO_SEL

/* Bits for R88EE_LEDCFG2. */
#define R88EE_LEDCFG2_EN			0x60 // NOT VERIFIED
#define R88EE_LEDCFG2_DIS		0x68 // NOT VERIFIED

/* Bits for R88EE_HIMR. */
#define R88EE_IMR_ROK		0x00000001	/* receive DMA OK */ // DONE
#define R88EE_IMR_VODOK		0x00000004	/* AC_VO DMA OK */ // DONE
#define R88EE_IMR_VIDOK		0x00000008	/* AC_VI DMA OK */ // DONE
#define R88EE_IMR_BEDOK		0x00000010	/* AC_BE DMA OK */ // DONE
#define R88EE_IMR_BKDOK		0x00000020	/* AC_BK DMA OK */ // DONE
#define R88EE_IMR_TXBDER	0x04000000	/* beacon transmit error */ // DONE
#define R88EE_IMR_MGNTDOK	0x00000040	/* management queue DMA OK */ // DONE
#define R88EE_IMR_TBDOK		0x02000000	/* beacon transmit OK */ // DONE
#define R88EE_IMR_HIGHDOK	0x00000080	/* high queue DMA OK */ // DONE
#define R88EE_IMR_BDOK		0x00200000	/* beacon queue DMA OK */ // DONE
#define R88EE_IMR_ATIMEND	0x00001000	/* ATIM window end interrupt */ // DONE
#define R88EE_IMR_RDU		0x00000002	/* Rx descriptor unavailable */ // DONE
#define R88EE_IMR_RXFOVW	0x00000100	/* receive FIFO overflow */ // DONE
#define R88EE_IMR_BCNINT	0x00100000	/* beacon DMA interrupt 0 */ // DONE
#define R88EE_IMR_PSTIMEOUT	0x02000000	/* powersave timeout */ // DONE
#define R88EE_IMR_TXFOVW	0x00000200	/* transmit FIFO overflow */ // DONE
#define R88EE_IMR_TIMEOUT1	0x00010000	/* timeout interrupt 1 */ // NOT VERIFIED
#define R88EE_IMR_TIMEOUT2	0x00020000	/* timeout interrupt 2 */ // NOT VERIFIED
#define R88EE_IMR_BCNDOK1	0x00004000	/* beacon queue DMA OK (1) */ // DONE
#define R88EE_IMR_BCNDOK2	0x00008000	/* beacon queue DMA OK (2) */ // DONE
#define R88EE_IMR_BCNDOK3	0x00010000	/* beacon queue DMA OK (3) */ // DONE
#define R88EE_IMR_BCNDOK4	0x00020000	/* beacon queue DMA OK (4) */ // DONE
#define R88EE_IMR_BCNDOK5	0x00040000	/* beacon queue DMA OK (5) */ // DONE
#define R88EE_IMR_BCNDOK6	0x00080000	/* beacon queue DMA OK (6) */ // DONE
#define R88EE_IMR_BCNDOK7	0x00100000	/* beacon queue DMA OK (7) */ // DONE
#define R88EE_IMR_BCNDOK8	0x02000000	/* beacon queue DMA OK (8) */ // NOT VERIFIED
#define R88EE_IMR_BCNDMAINT1	0x04000000	/* beacon DMA interrupt 1 */ // NOT VERIFIED 
#define R88EE_IMR_BCNDMAINT2	0x08000000	/* beacon DMA interrupt 2 */ // NOT VERIFIED
#define R88EE_IMR_BCNDMAINT3	0x10000000	/* beacon DMA interrupt 3 */ // NOT VERIFIED
#define R88EE_IMR_BCNDMAINT4	0x20000000	/* beacon DMA interrupt 4 */ // NOT VERIFIED
#define R88EE_IMR_BCNDMAINT5	0x40000000	/* beacon DMA interrupt 5 */ // NOT VERIFIED
#define R88EE_IMR_BCNDMAINT6	0x80000000	/* beacon DMA interrupt 6 */ // NOT VERIFIED

/* Shortcut. */
#define R88EE_IBSS_INT_MASK	\
	(R88EE_IMR_BCNINT | R88EE_IMR_TBDOK | R88EE_IMR_TBDER)

#endif	/* R88EE_REG_H */
