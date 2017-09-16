/*	$OpenBSD: if_rtwn.c,v 1.6 2015/08/28 00:03:53 deraadt Exp $	*/

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

#include <machine/bus.h>
#include <machine/resource.h>
#include <sys/rman.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_media.h>

#include <net80211/ieee80211_var.h>
#include <net80211/ieee80211_radiotap.h>

#include <dev/rtwn/if_rtwnvar.h>

#include <dev/rtwn/pci/rtwn_pci_var.h>
#include <dev/rtwn/rtl8188e/r88e_reg.h>
#include <dev/rtwn/rtl8192c/r92c.h>
#include <dev/rtwn/rtl8192c/pci/r92ce_reg.h> // Temporary until the R88EE values are added

#include <dev/rtwn/rtl8188e/pci/r88ee.h>
#include <dev/rtwn/rtl8188e/pci/r88ee_reg.h>
#include <dev/rtwn/rtl8188e/pci/r88ee_pwrseq.h>

bool pwrseqcmdparsing(struct rtwn_softc *sc, uint8_t cut_version,
              uint8_t faversion, uint8_t interface_type,
              struct wlan_pwr_cfg pwrcfgcmd[]);

void
r88ee_init_intr(struct rtwn_softc *sc)
{
#if 0
	/* Disable interrupts. */
	rtwn_write_4(sc, R88E_HISR, 0x00000000);
	rtwn_write_4(sc, R88E_HIMR, 0x00000000);
#else
	rtwn_write_4(sc, R92C_HISR, 0xffffffff);
	rtwn_write_4(sc, R92C_HISRE, 0xffffffff);
/////////////////////
//	rtwn_write_4(sc, R88E_HIMR, R88E_HIMR_CPWM | R88E_HIMR_CPWM2 |
//		R88E_HIMR_TBDER | R88E_HIMR_PSTIMEOUT);
//	rtwn_write_4(sc, R88E_HIMRE, R88E_HIMRE_RXFOVW |
//		R88E_HIMRE_TXFOVW | R88E_HIMRE_RXERR | R88E_HIMRE_TXERR);
////////////////////// New stuff that seemed to work

///// Below this is the old stuff that doesn't work
/////////////////////////////////////////////////////////////
//	rtwn_setbits_1(sc, R92C_USB_SPECIAL_OPTION, 0,     //
//		R92C_USB_SPECIAL_OPTION_INT_BULK_SEL);     //
/////////////////////////////////////////////////////////////
	printf("RTL8188EE:%s:%s Copied from EU\n", __FILE__, __func__);
#endif
}

void
r88ee_init_edca(struct rtwn_softc *sc)
{

#define REG_SIFS_CTX		0x0514
#define REG_SIFS_TRX		0x0516
#define REG_SPEC_SIFS		0x0428
#define REG_MAC_SPEC_SIFS	0x063A
#define REG_RESP_SIFS_OFDM	0x063E

	rtwn_write_1(sc, REG_SIFS_CTX + 1, 0x0a);
	rtwn_write_1(sc, REG_SIFS_TRX + 1, 0x0a);
	rtwn_write_1(sc, REG_SPEC_SIFS + 1, 0x0a);
	rtwn_write_1(sc, REG_MAC_SPEC_SIFS + 1, 0x0a);

	// Linux has an if-condition here for ht_enable rtl8188ee/hw.c

	rtwn_write_2(sc, REG_RESP_SIFS_OFDM, 0x0e0e);

#if 0
	/* SIFS */
	rtwn_write_2(sc, R88E_SPEC_SIFS, 0x1010);
	rtwn_write_2(sc, R88E_MAC_SPEC_SIFS, 0x1010);
	rtwn_write_2(sc, R88E_SIFS_CCK, 0x1010);
	rtwn_write_2(sc, R88E_SIFS_OFDM, 0x0e0e);
	/* TXOP */
	rtwn_write_4(sc, R88E_EDCA_BE_PARAM, 0x005ea42b);
	rtwn_write_4(sc, R88E_EDCA_BK_PARAM, 0x0000a44f);
	rtwn_write_4(sc, R88E_EDCA_VI_PARAM, 0x005e4322);
	rtwn_write_4(sc, R88E_EDCA_VO_PARAM, 0x002f3222);
#else
	printf("RTL8188EE:%s:%s not fully implemented\n", __FILE__, __func__);
#endif
}

int
r88ee_power_on(struct rtwn_softc *sc)
{
// I believe this is the code from _rtl88ee_init_mac()

#define RTWN_CHECK(res) do {    \
        if (res != 0)           \
                return (EIO);   \
} while(0)
//      int ntries;

/////// FreeBSD code
//      /* Wait for power ready bit. */
//      for (ntries = 0; ntries < 5000; ntries++) {
//              if (rtwn_read_4(sc, R92C_APS_FSMCO) & R92C_APS_FSMCO_SUS_HOST)
//                      break;
//              rtwn_delay(sc, 10);
//      }
//      if (ntries == 5000) {
//              device_printf(sc->sc_dev,
//                  "timeout_waiting for chip power up\n");
//              return (ETIMEDOUT);
//      }
//
#define REG_XCK_OUT_CTRL        0x07c
#define REG_APS_FSMCO           0x0004
#define REG_RSV_CTRL            0x001C

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

#define REG_TX_RPT_CTRL 0x04EC

//    bytetmp = rtl_read_byte(rtlpriv, REG_TX_RPT_CTRL);
//    rtl_write_byte(rtlpriv, REG_TX_RPT_CTRL, bytetmp|BIT(1)|BIT(0));
//    rtl_write_byte(rtlpriv, REG_TX_RPT_CTRL+1, 2);
//    rtl_write_word(rtlpriv, REG_TX_RPT_TIME, 0xcdf0);

#define REG_TX_RPT_CTRL 0x04EC
#define REG_TX_RPT_TIME 0x04F0
#define REG_SYS_CLKR    0x0008
#define REG_GPIO_MUXCFG 0x0040

        RTWN_CHECK(rtwn_setbits_1(sc, REG_TX_RPT_CTRL, 0, 0x02 | 0x01 ));
        rtwn_write_1(sc, REG_TX_RPT_CTRL+1, 2);
        rtwn_write_2(sc, REG_TX_RPT_TIME, 0xcdf0);

        RTWN_CHECK(rtwn_setbits_1(sc, REG_SYS_CLKR, 0, 0x08)); //BIT(3))); // hw.c 882

        RTWN_CHECK(rtwn_setbits_1(sc, REG_GPIO_MUXCFG+1, 0,
                ~(0x10))); //BIT(4) ));

        rtwn_write_1(sc, 0x367, 0x80);

#define REG_CR  0x0100
#define MSR     (REG_CR + 2)

        rtwn_write_2(sc, REG_CR, 0x2ff);
        rtwn_write_1(sc, REG_CR+1, 0x06);
        rtwn_write_1(sc, MSR, 0x00);

        // Linux driver Does the REG_HISR 0xfffffff crap here   
        // This seems to take place in init_intr() code

#define R88E_MCUTST_1   0x01c0

        rtwn_write_4(sc, R92C_INT_MIG, 0); // same from _rtl88ee_init_mac
                                           // same value of INT_MIG
        rtwn_write_4(sc, R88E_MCUTST_1, 0x0);
        rtwn_write_1(sc, R92C_PCIE_CTRL_REG+1, 0);

        return(0);

}

void
r88ee_power_off(struct rtwn_softc *sc)
{
#if 0
#ifndef RTWN_WITHOUT_UCODE
	struct r92c_softc *rs = sc->sc_priv;

	/* Deinit C2H event handler. */
	callout_stop(&rs->rs_c2h_report);
	rs->rs_c2h_paused = 0;
	rs->rs_c2h_pending = 0;
	rs->rs_c2h_timeout = hz;
#endif

	/* Stop hardware. */
	/* Disable interrupts. */
	rtwn_write_4(sc, R88E_HISR, 0);
	rtwn_write_4(sc, R88E_HIMR, 0);

	/* Stop hardware. */
	rtwn_write_1(sc, R88E_TXPAUSE, R88E_TX_QUEUE_ALL);

	/* Turn off RF. */
	rtwn_write_1(sc, R88E_RF_CTRL, 0);

	/* Reset BB state machine */
	rtwn_setbits_1(sc, R88E_SYS_FUNC_EN, 0, R88E_SYS_FUNC_EN_BB_GLB_RST);
	rtwn_setbits_1(sc, R88E_SYS_FUNC_EN, R88E_SYS_FUNC_EN_BB_GLB_RST, 0);

	/* Disable MAC DMA/WMAC/SCHEDULE/SEC blocks. */
	rtwn_setbits_2(sc, R88E_CR,
	    R88E_CR_HCI_TXDMA_EN | R88E_CR_HCI_RXDMA_EN |
	    R88E_CR_TXDMA_EN | R88E_CR_RXDMA_EN | R88E_CR_PROTOCOL_EN |
	    R88E_CR_SCHEDULE_EN | R88E_CR_MACTXEN | R88E_CR_MACRXEN |
	    R88E_CR_ENSEC,
	    0);

	/* If firmware in ram code, do reset. */
#ifndef RTWN_WITHOUT_UCODE
	if (rtwn_read_1(sc, R88E_MCUFWDL) & R88E_MCUFWDL_RAM_DL_SEL)
		r88ee_fw_reset(sc, RTWN_FW_RESET_SHUTDOWN);
#endif

	/* TODO: linux does additional btcoex stuff here */
	rtwn_write_2(sc, R88E_AFE_PLL_CTRL, 0x80); /* linux magic number */
	rtwn_write_1(sc, R88E_SPS0_CTRL, 0x23); /* ditto */
	rtwn_write_1(sc, R88E_AFE_XTAL_CTRL, 0x0e); /* different with btcoex */
	rtwn_write_1(sc, R88E_RSV_CTRL, 0x0e);
	rtwn_write_1(sc, R88E_APS_FSMCO, R88E_APS_FSMCO_PDN_EN);
#else
	printf("RTL8188EE:%s:%s not fully implemented\n", __FILE__, __func__);

#endif
}

void
r88ee_init_ampdu(struct rtwn_softc *sc)
{
#if 0
	/* Setup AMPDU aggregation. */
	rtwn_write_4(sc, R88E_AGGLEN_LMT, 0x99997631);	/* MCS7~0 */
	rtwn_write_1(sc, R88E_AGGR_BREAK_TIME, 0x16);
#else
	printf("RTL8188EE:%s:%s not fully implemented\n", __FILE__, __func__);

#endif
}

void
r88ee_post_init(struct rtwn_softc *sc)
{
#if 0
	rtwn_write_2(sc, R88E_FWHW_TXQ_CTRL,
	    0x1f00 | R88E_FWHW_TXQ_CTRL_AMPDU_RTY_NEW);

	rtwn_write_1(sc, R88E_BCN_MAX_ERR, 0xff);

	/* Perform LO and IQ calibrations. */
	r88ee_iq_calib(sc);
	/* Perform LC calibration. */
	r92c_lc_calib(sc);

	r92c_pa_bias_init(sc);

	/* Fix for lower temperature. */
	rtwn_write_1(sc, 0x15, 0xe9);

#ifndef RTWN_WITHOUT_UCODE
	if (sc->sc_flags & RTWN_FW_LOADED) {
		struct r92c_softc *rs = sc->sc_priv;

		if (sc->sc_ratectl_sysctl == RTWN_RATECTL_FW) {
			/* XXX TODO: fix (see comment in r92cu_init.c) */
			sc->sc_ratectl = RTWN_RATECTL_NET80211;
		} else
			sc->sc_ratectl = sc->sc_ratectl_sysctl;

		/* Start C2H event handling. */
		callout_reset(&rs->rs_c2h_report, rs->rs_c2h_timeout,
		    r92c_handle_c2h_report, sc);
	} else
#endif
		sc->sc_ratectl = RTWN_RATECTL_NONE;
#else
	printf("RTL8188EE:%s:%s not fully implemented\n", __FILE__, __func__);
#endif
}

void
r88ee_init_rx_agg(struct rtwn_softc *sc) {
//	struct r92c_softc *rs = sc->sc_priv;
	struct rtwn_pci_softc *pc = sc->sc_priv;
//	sc->sc_priv->tcr

	unsigned short wordtmp;

	wordtmp = rtwn_read_2(sc, R88EE_TRXDMA_CTRL);
	wordtmp &= 0xf;
	wordtmp |= 0xE771;
	rtwn_write_2(sc, R88EE_TRXDMA_CTRL, wordtmp);

	rtwn_write_4(sc, R92C_RCR, sc->rcr); //rtlpci->receive_config); // same value
	rtwn_write_2(sc, R92C_RXFLTMAP2, 0xffff); // Same R92C value
	rtwn_write_4(sc, R92C_TCR, pc->tcr); //rtlpci->receive_config); // same value

	// Seems to come from rtl_init_mac, line 945
//	rtl_write_byte(rtlpriv, REG_PCIE_CTRL_REG+1, 0);

	rtwn_write_1(sc, R92C_PCIE_CTRL_REG+1, 0);

//////////////////////////////////////////////////////
// Seems like we should include the code beforehand //
// _rtl88ee_gen_refresh_led_state from Linux        //
//////////////////////////////////////////////////////
}
