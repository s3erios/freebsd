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

#include <dev/rtwn/if_rtwnreg.h>
#include <dev/rtwn/if_rtwnvar.h>
#include <dev/rtwn/if_rtwn_nop.h>

#include <dev/rtwn/rtl8188e/r88e_priv.h>
#include <dev/rtwn/pci/rtwn_pci_var.h>
#include <dev/rtwn/rtl8192c/r92c.h>
#include <dev/rtwn/rtl8192c/r92c_var.h>
#include <dev/rtwn/rtl8192c/pci/r92ce.h>

// Just added
#include <dev/rtwn/rtl8192c/r92c_var.h>
#include <dev/rtwn/rtl8192c/pci/r92ce.h>
#include <dev/rtwn/rtl8192c/pci/r92ce_priv.h>
#include <dev/rtwn/rtl8192c/pci/r92ce_reg.h>
#include <dev/rtwn/rtl8192c/pci/r92ce_tx_desc.h>
#include <dev/rtwn/rtl8192c/r92c.h>
// Remove at the end

#include <dev/rtwn/rtl8188e/pci/r88ee.h>
#include <dev/rtwn/rtl8188e/pci/r88ee_priv.h>
#include <dev/rtwn/rtl8188e/pci/r88ee_reg.h>
#include <dev/rtwn/rtl8188e/pci/r88ee_tx_desc.h>
#include <dev/rtwn/rtl8188e/usb/r88eu.h>

//static struct rtwn_r88ee_txpwr r88e_txpwr;
static struct rtwn_r92c_txpwr r88ee_txpwr;

void	r88ee_attach(struct rtwn_pci_softc *);

static void
r88ee_postattach(struct rtwn_softc *sc)
{
#if 0
	struct r88ee_softc *rs = sc->sc_priv;
	struct ieee80211com *ic = &sc->sc_ic;

	if (!(rs->chip & R88EE_CHIP_92C) &&
	    rs->board_type == R88EE_BOARD_TYPE_HIGHPA)
		rs->rs_txagc = &rtl8188ru_txagc[0];
	else
		rs->rs_txagc = &rtl8188eu_txagc[0];

	if ((rs->chip & (R88EE_CHIP_UMC_A_CUT | R88EE_CHIP_92C)) ==
	    R88EE_CHIP_UMC_A_CUT)
		sc->fwname = "rtwn-rtl8188efwE";
	else
		sc->fwname = "rtwn-rtl8188efwE_B";
	sc->fwsig = 0x88c;

	rs->rs_scan_start = ic->ic_scan_start;
	ic->ic_scan_start = r88ee_scan_start;
	rs->rs_scan_end = ic->ic_scan_end;
	ic->ic_scan_end = r88ee_scan_end;
#else
	struct r92c_softc *rs = sc->sc_priv;
	struct ieee80211com *ic = &sc->sc_ic;
	sc->fwname = "rtwn-rtl8188eefw";
	sc->fwsig = 0x88E;

	rs->rs_scan_start = ic->ic_scan_start;
	ic->ic_scan_start = r92c_scan_start;
	rs->rs_scan_end = ic->ic_scan_end;
	ic->ic_scan_end = r92c_scan_end;

//	printf("Check this too\n");
	printf("RTL8188EE:%s:%s Function Trace\n", __FILE__, __func__);
//	printf("RTL8188EE:%s:%s not fully implemented\n", __FILE__, __func__);
#endif
}

static void
r88ee_set_name(struct rtwn_softc *sc)
{
	sc->name = "RTL8188EE";
	printf("RTL8188EE:%s:%s Function Trace\n", __FILE__, __func__);
}

static void
r88ee_attach_private(struct rtwn_softc *sc)
{
	struct r92c_softc *rs;

	rs = malloc(sizeof(struct r92c_softc), M_RTWN_PRIV, M_WAITOK | M_ZERO);

	rs->rs_txpwr			= &r88ee_txpwr;

	rs->rs_set_bw20			= r88e_set_bw20;
	rs->rs_get_txpower		= r88e_get_txpower;
	rs->rs_set_gain			= r88e_set_gain;
	rs->rs_tx_enable_ampdu		= r88e_tx_enable_ampdu;
	rs->rs_tx_setup_hwseq		= r88e_tx_setup_hwseq;
	rs->rs_tx_setup_macid		= r88e_tx_setup_macid;
	rs->rs_set_name			= r88ee_set_name;

	/* XXX TODO: test with net80211 ratectl! */
#ifndef RTWN_WITHOUT_UCODE
	rs->rs_c2h_timeout		= hz;

	callout_init_mtx(&rs->rs_c2h_report, &sc->sc_mtx, 0);
#endif

	rs->rf_read_delay[0]		= 1000;
	rs->rf_read_delay[1]		= 1000;
	rs->rf_read_delay[2]		= 1000;

	sc->sc_priv = rs;

	printf("RTL8188EE:%s:%s Function Trace\n", __FILE__, __func__);
}

static void
r88ee_adj_devcaps(struct rtwn_softc *sc)
{
	struct ieee80211com *ic = &sc->sc_ic;

	/*
	 * XXX do NOT enable PMGT until RSVD_PAGE command
	 * will not be tested / fixed + HRPWM register must be set too.
	 */
	ic->ic_caps &= ~IEEE80211_C_PMGT;
}

void
r88ee_attach(struct rtwn_pci_softc *pc)
{
	struct rtwn_softc *sc		= &pc->pc_sc;

	/* PCIe part. */
	pc->pc_setup_tx_desc		= r88ee_setup_tx_desc;
	pc->pc_tx_postsetup		= r88ee_tx_postsetup;
	pc->pc_copy_tx_desc		= r88ee_copy_tx_desc;
						// tap
	pc->pc_enable_intr		= r88ee_enable_intr; //r92ce_enable_intr;

	pc->pc_qmap			= 0xf771;
	pc->tcr				=
	    R92C_TCR_CFENDFORM | (1 << 12) | (1 << 13);

	/* Common part. */
	/* RTL8192C* cannot use pairwise keys from first 4 slots */
	sc->sc_flags			= RTWN_FLAG_EXT_HDR; //RTWN_FLAG_CAM_FIXED;

						// tap
	sc->sc_start_xfers		= r88ee_start_xfers;
						// tap
	sc->sc_set_chan			= r92c_set_chan;
						// tap
	sc->sc_fill_tx_desc		= r92c_fill_tx_desc;
						// tap
	sc->sc_fill_tx_desc_raw		= r92c_fill_tx_desc_raw;
						// tap
	sc->sc_fill_tx_desc_null	= r92c_fill_tx_desc_null; /* XXX recheck */
						// tap
	sc->sc_dump_tx_desc		= r92ce_dump_tx_desc;
						// tap
	sc->sc_tx_radiotap_flags	= r92c_tx_radiotap_flags;
						// tap
	sc->sc_rx_radiotap_flags	= r92c_rx_radiotap_flags;
						// tap
	sc->sc_get_rssi_cck		= r88e_get_rssi_cck;
						// 
	sc->sc_get_rssi_ofdm		= r88e_get_rssi_ofdm;
						// tap
	sc->sc_classify_intr		= r92ce_classify_intr;
	sc->sc_handle_tx_report		= rtwn_nop_softc_uint8_int;
	sc->sc_handle_c2h_report	= rtwn_nop_softc_uint8_int;
	sc->sc_check_frame		= rtwn_nop_int_softc_mbuf;
	sc->sc_rf_read			= r92c_rf_read;
	sc->sc_rf_write			= r88e_rf_write;
						// tap
	sc->sc_check_condition		= r92c_check_condition;
	sc->sc_efuse_postread		= r92c_efuse_postread;
	sc->sc_parse_rom		= r88ee_parse_rom;
	sc->sc_set_led			= r88e_set_led;
	sc->sc_power_on		= r88ee_power_on;
	sc->sc_power_off		= r88ee_power_off;
#ifndef RTWN_WITHOUT_UCODE
	sc->sc_fw_reset			= r88ee_fw_reset;
	sc->sc_fw_download_enable	= r88ee_fw_download_enable;
#endif
						// tap
	sc->sc_set_page_size		= r92c_set_page_size;
						// tap
	sc->sc_lc_calib			= r92c_lc_calib;
	sc->sc_iq_calib			= r88e_iq_calib;
	sc->sc_read_chipid_vendor	= rtwn_nop_softc_uint32;
	sc->sc_adj_devcaps		= r88ee_adj_devcaps;
	sc->sc_vap_preattach		= rtwn_nop_softc_vap;
	sc->sc_postattach		= r88ee_postattach;
	sc->sc_detach_private		= r92c_detach_private;
	sc->sc_set_media_status		= r88e_set_media_status;
#ifndef RTWN_WITHOUT_UCODE
	sc->sc_set_rsvd_page		= r88e_set_rsvd_page;
	sc->sc_set_pwrmode		= r88e_set_pwrmode;
	sc->sc_set_rssi			= r92c_set_rssi;
#endif
						// tap
	sc->sc_beacon_init		= r92c_beacon_init;
	sc->sc_beacon_enable		= r88e_beacon_enable;
	sc->sc_beacon_set_rate		= rtwn_nop_void_int;
	sc->sc_beacon_select		= rtwn_nop_softc_int;
	sc->sc_temp_measure		= r88e_temp_measure;
	sc->sc_temp_read		= r88e_temp_read;
	sc->sc_init_tx_agg		= rtwn_nop_softc;
	sc->sc_init_rx_agg		= rtwn_nop_softc; //r88ee_init_rx_agg;
						// tap
	sc->sc_init_ampdu		= r92ce_init_ampdu;
	sc->sc_init_intr		= r88ee_init_intr; // Was r92c_init_intr, changed
						// tap
	sc->sc_init_edca		= r92ce_init_edca;
	sc->sc_init_bb			= r88ee_init_bb;
						// tap
	sc->sc_init_rf			= r92c_init_rf;
	sc->sc_init_antsel	= rtwn_nop_softc; // Might be r92c_init_antsel
						// tap
	sc->sc_post_init		= r88ee_post_init; //r92ce_post_init;
	sc->sc_init_bcnq1_boundary	= rtwn_nop_int_softc;

	sc->mac_prog			= &rtl8188ee_mac[0];
	sc->mac_size			= nitems(rtl8188ee_mac);
	sc->bb_prog				= &rtl8188ee_bb[0];
	sc->bb_size				= nitems(rtl8188ee_bb);
	sc->agc_prog			= &rtl8188ee_agc[0];
	sc->agc_size			= nitems(rtl8188ee_agc);
	sc->rf_prog				= &rtl8188ee_rf[0];

	sc->page_count			= 0xAB - 1;// This came from the Linux driver, previous R92CE_TX_PAGE_COUNT;
									// This came from rtl8188ee/hw.c 794
	sc->pktbuf_count		= R92C_TXPKTBUF_COUNT;

	sc->ackto			= 0x40;
	sc->npubqpages			= R92CE_PUBQ_NPAGES;
	sc->nhqpages			= R92CE_HPQ_NPAGES;
	sc->nnqpages			= 1; // Changed to one per Linux
	sc->nlqpages			= R92CE_LPQ_NPAGES;
	sc->page_size			= R92C_TX_PAGE_SIZE;

	sc->txdesc_len			= sizeof(struct r88ee_tx_desc);
	sc->efuse_maxlen		= R88E_EFUSE_MAX_LEN;
	sc->efuse_maplen		= R88E_EFUSE_MAP_LEN;
	sc->rx_dma_size			= R88E_RX_DMA_BUFFER_SIZE;

	sc->macid_limit			= R88E_MACID_MAX + 1;
	sc->cam_entry_limit		= R92C_CAM_ENTRY_COUNT;
//	sc->fwsize_limit		= R92C_MAX_FW_SIZE; 
	sc->fwsize_limit		= 77895; // Stolen from Linux
	// https://github.com/lwfinger/rtlwifi_new/blob/master/rtl8188ee/fw.h#L32
	sc->temp_delta			= R88E_CALIB_THRESHOLD;

	sc->bcn_status_reg[0]		= R92C_TDECTRL;
	/*
	 * TODO: some additional setup is required
	 * to maintain few beacons at the same time.
	 *
	 * XXX BCNQ1 mechanism is not needed here; move it to the USB module.
	 */
	sc->bcn_status_reg[1]		= R92C_TDECTRL;
	sc->rcr				= 0;

	sc->sc_llt_init =		r92c_llt_init;

	sc->ntxchains				= 1; // This is a guess
	sc->nrxchains				= 1; // This is a guess also

	printf("RTL8188EE:%s:%s Function Trace\n", __FILE__, __func__);
	printf("Is this called now?\n");
	r88ee_attach_private(sc);
}
