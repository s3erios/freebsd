#ifndef R88EE_REG_H
#define R88EE_REG_H

//#include not sure what???

/*
 * MAC Registers
 */

/* System Configuration. */
#define R88EE_PCIE_MIO_INTF		0x00e4
#define R88EE_PCIE_MIO_INTD		0x00e8
/* PCIe Configuration. */
#define R88EE_PCIE_CTRL_REG		0x0300
#define R88EE_INT_MIG			0x0304
#define R88EE_BCNQ_DESA			0x0308
#define R88EE_HQ_DESA			0x0310
#define R88EE_MGQ_DESA			0x0318
#define R88EE_VOQ_DESA			0x0320
#define R88EE_VIQ_DESA			0x0328
#define R88EE_BEQ_DESA			0x0330
#define R88EE_BKQ_DESA			0x0338
#define R88EE_RX_DESA			0x0340
#define R88EE_DBI			0x0348
#define R88EE_MDIO			0x0354
#define R88EE_DBG_SEL			0x0360
#define R88EE_PCIE_HRPWM		0x0361
#define R88EE_PCIE_HCPWM		0x0363
#define R88EE_UART_CTRL			0x0364
#define R88EE_UART_TX_DES		0x0370
#define R88EE_UART_RX_DES		0x0378

// A bunch of undefined stuff should go here

#define R88EE_IMR_ROK		0x00000001	/* receive DMA OK */
#define R88EE_IMR_VODOK		0x00000004	/* AC_VO DMA OK */
#define R88EE_IMR_VIDOK		0x00000008	/* AC_VI DMA OK */
#define R88EE_IMR_BEDOK		0x00000010	/* AC_BE DMA OK */
#define R88EE_IMR_BKDOK		0x00000020	/* AC_BK DMA OK */
//#define R88EE_IMR_TXBDE	0x00000040	/* beacon transmit error */
#define R88EE_IMR_MGNTDOK	0x00000040	/* management queue DMA OK */
#define R88EE_IMR_TBDOK		0x02000000 	/* beacon transmit OK */
#define R88EE_IMR_HIGHDOK	0x00000080	/* high queue DMA OK */
//#define R88EE_IMR_BDOK	0x00000400	/* beacon queue DMA OK */
#define R88EE_IMR_ATIMEND	0x00001000	/* ATIM window end interrupt */
#define R88EE_IMR_RDU		0x00000002	/* Rx descriptor unavailable */
#define R88EE_IMR_RXFOVW	0x00000100	/* receive FIFO overflow */
#define R88EE_IMR_BCNINT	0x00004000	/* beacon DMA interrupt 0 */
#define R88EE_IMR_PSTIMEOUT	0x20000000	/* powersave timeout */
#define R88EE_IMR_TXFOVW	0x00010000	/* transmit FIFO overflow */
//#define R88EE_IMR_TIMEOUT1	0x00020000	/* timeout interrupt 1 */
//#define R88EE_IMR_TIMEOUT2	0x00040000	/* timeout interrupt 2 */
#define R88EE_IMR_BCNDOK1	0x00004000	/* beacon queue DMA OK (1) */
#define R88EE_IMR_BCNDOK2	0x00008000	/* beacon queue DMA OK (2) */
#define R88EE_IMR_BCNDOK3	0x00010000	/* beacon queue DMA OK (3) */
#define R88EE_IMR_BCNDOK4	0x00020000	/* beacon queue DMA OK (4) */
#define R88EE_IMR_BCNDOK5	0x00040000	/* beacon queue DMA OK (5) */
#define R88EE_IMR_BCNDOK6	0x00080000	/* beacon queue DMA OK (6) */
#define R88EE_IMR_BCNDOK7	0x00100000	/* beacon queue DMA OK (7) */
//#define R88EE_IMR_BCNDOK8	0x04000000	/* beacon queue DMA OK (8) */
#define R88EE_IMR_BCNDMAINT1	0x00200000	/* beacon DMA interrupt 1 */
#define R88EE_IMR_BCNDMAINT2	0x00400000	/* beacon DMA interrupt 2 */
#define R88EE_IMR_BCNDMAINT3	0x00800000	/* beacon DMA interrupt 3 */
#define R88EE_IMR_BCNDMAINT4	0x01000000	/* beacon DMA interrupt 4 */
#define R88EE_IMR_BCNDMAINT5	0x02000000	/* beacon DMA interrupt 5 */
#define R88EE_IMR_BCNDMAINT6	0x04000000	/* beacon DMA interrupt 6 */
// Added from:
// https://github.com/lwfinger/rtlwifi_new/blob/master/rtl8188ee/reg.h#L593
#define R88EE_IMR_BCNDMAINT7	0x08000000	/* beacon DMA interrupt 7 */

/* Shortcut. */
#define R88EE_IBSS_INT_MASK	\
	(R88EE_IMR_BCNINT | R88EE_IMR_TBDOK | R88EE_IMR_TBDER)

#define R88EE_C2HEVT_CLEAR	0x01AF

#endif /* R88EE_REG_H */
