/*	$OpenBSD: if_urtwn.c,v 1.16 2011/02/10 17:26:40 jakemsr Exp $	*/

/*-
 * Copyright (c) 2010 Damien Bergamini <damien.bergamini@free.fr>
 * Copyright (c) 2014 Kevin Lo <kevlo@FreeBSD.org>
 * Copyright (c) 2015-2016 Andriy Voskoboinyk <avos@FreeBSD.org>
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
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "opt_wlan.h"

#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/mbuf.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/queue.h>
#include <sys/taskqueue.h>
#include <sys/bus.h>
#include <sys/endian.h>
#include <sys/linker.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_media.h>

#include <net80211/ieee80211_var.h>
#include <net80211/ieee80211_radiotap.h>

#include <dev/rtwn/if_rtwnreg.h>
#include <dev/rtwn/if_rtwnvar.h>

#include <dev/rtwn/if_rtwn_debug.h>

#include <dev/rtwn/rtl8192c/r92c.h>
#include <dev/rtwn/rtl8192c/r92c_var.h>

#include <dev/rtwn/rtl8188e/r88e.h>
#include <dev/rtwn/rtl8188e/r88e_reg.h>
#include <dev/rtwn/rtl8188e/r88e_pwrseq.h>
#include <dev/rtwn/rtl8192c/pci/r92ce_reg.h> // Added due to R92C_INT_MIG

static void
r88e_crystalcap_write(struct rtwn_softc *sc)
{
	struct r92c_softc *rs = sc->sc_priv;
	uint32_t reg;
	uint8_t val;

	val = rs->crystalcap & 0x3f;
	reg = rtwn_bb_read(sc, R92C_AFE_XTAL_CTRL);
	rtwn_bb_write(sc, R92C_AFE_XTAL_CTRL,
	    RW(reg, R92C_AFE_XTAL_CTRL_ADDR, val | val << 6));
}

void
r88e_init_bb(struct rtwn_softc *sc)
{

	/* Enable BB and RF. */
	rtwn_setbits_2(sc, R92C_SYS_FUNC_EN, 0,
	    R92C_SYS_FUNC_EN_BBRSTB | R92C_SYS_FUNC_EN_BB_GLB_RST |
	    R92C_SYS_FUNC_EN_DIO_RF);

	rtwn_write_1(sc, R92C_RF_CTRL,
	    R92C_RF_CTRL_EN | R92C_RF_CTRL_RSTB | R92C_RF_CTRL_SDMRSTB);
	rtwn_write_1(sc, R92C_SYS_FUNC_EN,
	    R92C_SYS_FUNC_EN_USBA | R92C_SYS_FUNC_EN_USBD |
	    R92C_SYS_FUNC_EN_BB_GLB_RST | R92C_SYS_FUNC_EN_BBRSTB);

	r92c_init_bb_common(sc);

	rtwn_bb_write(sc, R92C_OFDM0_AGCCORE1(0), 0x69553422);
	rtwn_delay(sc, 1);
	rtwn_bb_write(sc, R92C_OFDM0_AGCCORE1(0), 0x69553420);
	rtwn_delay(sc, 1);

	r88e_crystalcap_write(sc);
}

void
r88ee_init_bb(struct rtwn_softc *sc)
{
	printf("AAAA RTL8188EE:%s:%s not fully implemented\n", __FILE__, __func__);
	/* Enable BB and RF. */
	rtwn_setbits_2(sc, R92C_SYS_FUNC_EN, 0,
	    R92C_SYS_FUNC_EN_BBRSTB | R92C_SYS_FUNC_EN_BB_GLB_RST |
	    R92C_SYS_FUNC_EN_DIO_RF);
	// Good, lines 263-265

			 // 0x01f, this seems good, 267
	rtwn_write_1(sc, R92C_RF_CTRL, 0x1 | 0x2 | 0x4);
			// line 268
	rtwn_write_1(sc, R92C_SYS_FUNC_EN, 0x80 | 0x40 | 0x20 | 0x2 | 0x1);
			// line 271-272
	rtwn_setbits_4(sc, R92C_LEDCFG0, 0, 0x00800000);

	r92c_init_bb_common(sc);
}

int
r88ee_power_on(struct rtwn_softc *sc)
{
// I believe this is the code from _rtl88ee_init_mac()

#define RTWN_CHECK(res) do {    \
	if (res != 0)           \
		return (EIO);   \
} while(0)
//	int ntries;

/////// FreeBSD code
//	/* Wait for power ready bit. */
//	for (ntries = 0; ntries < 5000; ntries++) {
//		if (rtwn_read_4(sc, R92C_APS_FSMCO) & R92C_APS_FSMCO_SUS_HOST)
//			break;
//		rtwn_delay(sc, 10);
//	}
//	if (ntries == 5000) {
//		device_printf(sc->sc_dev,
//		    "timeout_waiting for chip power up\n");
//		return (ETIMEDOUT);
//	}
//
#define REG_XCK_OUT_CTRL	0x07c
#define REG_APS_FSMCO		0x0004
#define REG_RSV_CTRL		0x001C

	uint8_t bytetmp;

	bytetmp = rtwn_read_1(sc, REG_XCK_OUT_CTRL) & (~0x10);
	rtwn_write_1(sc, REG_XCK_OUT_CTRL, bytetmp);

	bytetmp = rtwn_read_1(sc, REG_APS_FSMCO + 1) & (~0x80);
	rtwn_write_1(sc, REG_APS_FSMCO + 1, bytetmp);

	rtwn_write_1(sc, REG_RSV_CTRL, 0x00);

	printf("Complex power on here\n");
	if (!pwrseqcmdparsing(sc, PWR_CUT_ALL_MSK,
		PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,
		RTL8188EE_NIC_ENABLE_FLOW)) {
		uprintf("pwrseqcmdparsing failed\n");
	}


#define REG_APS_FSMCO 0x0004

//    bytetmp = rtl_read_byte(rtlpriv, REG_APS_FSMCO) | BIT(4);
//    rtl_write_byte(rtlpriv, REG_APS_FSMCO, bytetmp);
	RTWN_CHECK(rtwn_setbits_1(sc, REG_APS_FSMCO, 0,
		0x10)); //BIT(4)));

#define REG_PCIE_CTRL_REG 0x0300

//    bytetmp = rtl_read_byte(rtlpriv, REG_PCIE_CTRL_REG+2);
//    rtl_write_byte(rtlpriv, REG_PCIE_CTRL_REG+2, bytetmp|BIT(2));
	RTWN_CHECK(rtwn_setbits_1(sc, REG_PCIE_CTRL_REG+2, 0,
		0x04)); //BIT(2)));

#define REG_WATCH_DOG 0x0368

//    bytetmp = rtl_read_byte(rtlpriv, REG_WATCH_DOG+1);
//    rtl_write_byte(rtlpriv, REG_WATCH_DOG+1, bytetmp|BIT(7));
	RTWN_CHECK(rtwn_setbits_1(sc, REG_WATCH_DOG+1, 0,
		0x80)); //BIT(7)));

#define REG_AFE_XTAL_CTRL_EXT 0x0078

//    bytetmp = rtl_read_byte(rtlpriv, REG_AFE_XTAL_CTRL_EXT+1);
//    rtl_write_byte(rtlpriv, REG_AFE_XTAL_CTRL_EXT+1, bytetmp|BIT(1));
	RTWN_CHECK(rtwn_setbits_1(sc, REG_AFE_XTAL_CTRL_EXT+1, 0,
		0x02)); //BIT(1)));

#define REG_TX_RPT_CTRL	0x04EC

//    bytetmp = rtl_read_byte(rtlpriv, REG_TX_RPT_CTRL);
//    rtl_write_byte(rtlpriv, REG_TX_RPT_CTRL, bytetmp|BIT(1)|BIT(0));
//    rtl_write_byte(rtlpriv, REG_TX_RPT_CTRL+1, 2);
//    rtl_write_word(rtlpriv, REG_TX_RPT_TIME, 0xcdf0);

#define REG_TX_RPT_CTRL 0x04EC
#define REG_TX_RPT_TIME 0x04F0
#define REG_SYS_CLKR	0x0008
#define REG_GPIO_MUXCFG	0x0040

	RTWN_CHECK(rtwn_setbits_1(sc, REG_TX_RPT_CTRL, 0, 0x02 | 0x01 ));
	rtwn_write_1(sc, REG_TX_RPT_CTRL+1, 2);
	rtwn_write_2(sc, REG_TX_RPT_TIME, 0xcdf0);

	RTWN_CHECK(rtwn_setbits_1(sc, REG_SYS_CLKR, 0, 0x08)); //BIT(3))); // hw.c 882

	RTWN_CHECK(rtwn_setbits_1(sc, REG_GPIO_MUXCFG+1, 0,
		~(0x10))); //BIT(4) ));

	rtwn_write_1(sc, 0x367, 0x80);

#define REG_CR	0x0100
#define MSR	(REG_CR + 2)

	rtwn_write_2(sc, REG_CR, 0x2ff);
	rtwn_write_1(sc, REG_CR+1, 0x06);
	rtwn_write_1(sc, MSR, 0x00);

	// Linux driver Does the REG_HISR 0xfffffff crap here	
	// This seems to take place in init_intr() code

#define R88E_MCUTST_1	0x01c0

	rtwn_write_4(sc, R92C_INT_MIG, 0); // same from _rtl88ee_init_mac
					   // same value of INT_MIG
	rtwn_write_4(sc, R88E_MCUTST_1, 0x0);
	rtwn_write_1(sc, R92C_PCIE_CTRL_REG+1, 0);

	return(0);
}

int
r88e_power_on(struct rtwn_softc *sc)
{
#define RTWN_CHK(res) do {	\
	if (res != 0)		\
		return (EIO);	\
} while(0)
	int ntries;

	/* Wait for power ready bit. */
	for (ntries = 0; ntries < 5000; ntries++) {
		if (rtwn_read_4(sc, R92C_APS_FSMCO) & R92C_APS_FSMCO_SUS_HOST)
			break;
		rtwn_delay(sc, 10);
	}
	if (ntries == 5000) {
		device_printf(sc->sc_dev,
		    "timeout waiting for chip power up\n");
		return (ETIMEDOUT);
	}

	/* Reset BB. */
	RTWN_CHK(rtwn_setbits_1(sc, R92C_SYS_FUNC_EN,
	    R92C_SYS_FUNC_EN_BBRSTB | R92C_SYS_FUNC_EN_BB_GLB_RST, 0));

	RTWN_CHK(rtwn_setbits_1(sc, R92C_AFE_XTAL_CTRL + 2, 0, 0x80));

	/* Disable HWPDN. */
	RTWN_CHK(rtwn_setbits_1_shift(sc, R92C_APS_FSMCO,
	    R92C_APS_FSMCO_APDM_HPDN, 0, 1));

	/* Disable WL suspend. */
	RTWN_CHK(rtwn_setbits_1_shift(sc, R92C_APS_FSMCO,
	    R92C_APS_FSMCO_AFSM_HSUS | R92C_APS_FSMCO_AFSM_PCIE, 0, 1));

	RTWN_CHK(rtwn_setbits_1_shift(sc, R92C_APS_FSMCO,
	    0, R92C_APS_FSMCO_APFM_ONMAC, 1));
	for (ntries = 0; ntries < 5000; ntries++) {
		if (!(rtwn_read_2(sc, R92C_APS_FSMCO) &
		    R92C_APS_FSMCO_APFM_ONMAC))
			break;
		rtwn_delay(sc, 10);
	}
	if (ntries == 5000)
		return (ETIMEDOUT);

	/* Enable LDO normal mode. */
	RTWN_CHK(rtwn_setbits_1(sc, R92C_LPLDO_CTRL,
	    R92C_LPLDO_CTRL_SLEEP, 0));

	/* Enable MAC DMA/WMAC/SCHEDULE/SEC blocks. */
	RTWN_CHK(rtwn_write_2(sc, R92C_CR, 0));
	RTWN_CHK(rtwn_setbits_2(sc, R92C_CR, 0,
	    R92C_CR_HCI_TXDMA_EN | R92C_CR_TXDMA_EN |
	    R92C_CR_HCI_RXDMA_EN | R92C_CR_RXDMA_EN |
	    R92C_CR_PROTOCOL_EN | R92C_CR_SCHEDULE_EN |
	    ((sc->sc_hwcrypto != RTWN_CRYPTO_SW) ? R92C_CR_ENSEC : 0) |
	    R92C_CR_CALTMR_EN));

	return (0);
#undef RTWN_CHK
}
