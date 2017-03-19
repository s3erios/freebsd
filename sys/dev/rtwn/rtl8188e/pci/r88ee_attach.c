/*	$OpenBSD: if_rtwn.c,v 1.6 2015/08/28 00:03:53 deraadt Exp $	*/

/*-
 * Copyright (c) 2016 Farhan Khan <khanzf@gmail.com>
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
__FBSDID("$FreeBSD: head/sys/dev/rtwn/rtl8188ee/pci/r88eee_attach.c 308575 2016-11-12 17:58:37Z avos $");

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

#include <dev/rtwn/rtl8188ee/r88ee.h>
#include <dev/rtwn/rtl8188ee/pci/r88eee.h>
//#include <dev/rtwn/if_rtwn_var.h>
#include <dev/rtwn/if_rtwnreg.h>
#include <dev/rtwn/if_rtwnvar.h>
#include <dev/rtwn/if_rtwn_nop.h>

#include <dev/rtwn/pci/rtwn_pci_var.h>

#include <dev/rtwn/rtl8188ee/r88ee_var.h>

#include <dev/rtwn/rtl8188ee/pci/r88eee.h>
#include <dev/rtwn/rtl8188ee/pci/r88eee_priv.h>
#include <dev/rtwn/rtl8188ee/pci/r88eee_reg.h>
#include <dev/rtwn/rtl8188ee/pci/r88eee_tx_desc.h>


static struct rtwn_r88ee_txpwr r88ee_txpwr;

void	r88eee_attach(struct rtwn_pci_softc *);

/* Set the firmware and rs/ic based on the chip type */
static void
r88eee_postattach(struct rtwn_softc *sc)
{
#if 0
	struct r88ee_softc *rs = sc->sc_priv;
	struct ieee80211com *ic = &sc->sc_ic;

	if (!(rs->chip & R88EE_CHIP_92C) &&
	    rs->board_type == R88EE_BOARD_TYPE_HIGHPA)
		rs->rs_txagc = &rtl8188ru_txagc[0];
	else
		rs->rs_txagc = &rtl8188eeu_txagc[0];

	if ((rs->chip & (R88EE_CHIP_UMC_A_CUT | R88EE_CHIP_92C)) ==
	    R88EE_CHIP_UMC_A_CUT)
		sc->fwname = "rtwn-rtl8188eefwE";
	else
		sc->fwname = "rtwn-rtl8188eefwE_B";
	sc->fwsig = 0x88c;

	rs->rs_scan_start = ic->ic_scan_start;
	ic->ic_scan_start = r88ee_scan_start;
	rs->rs_scan_end = ic->ic_scan_end;
	ic->ic_scan_end = r88ee_scan_end;
#else
	struct r88ee_softc *rs = sc->sc_priv;
	struct ieee80211com *ic = &sc->sc_ic;
	sc->fwname = "rtl8188efw.bin";
	sc->fwsig = 0x88c;

	rs->rs_scan_start = ic->ic_scan_start;
	ic->ic_scan_start = r88ee_scan_start;
	rs->rs_scan_end = ic->ic_scan_end;
	ic->ic_scan_end = r88ee_scan_end;

//	printf("Check this too\n");
	printf("RTL8188EE:%s:%s not fully implemented\n", __FILE__, __func__);

#endif
}

static void
r88eee_set_name(struct rtwn_softc *sc)
{
#if 0
	struct r88ee_softc *rs = sc->sc_priv;

	if (rs->chip & R88EE_CHIP_92C)
		sc->name = "RTL8188EEE";
	else
		sc->name = "RTL8188CE";
#else
	sc->name = "RTL8188EE";
#endif
}


/*
 * Allocates memory and methods specific to the rtl8188ee card and attaches it
 * to the base rtwn framework.
 */
static void
r88eee_attach_private(struct rtwn_softc *sc)
{
	struct r88ee_softc *rs;

	rs = malloc(sizeof(struct r88ee_softc), M_RTWN_PRIV, M_WAITOK | M_ZERO);

	rs->rs_txpwr			= &r88ee_txpwr;

	rs->rs_set_bw20			= r88ee_set_bw20;
	rs->rs_get_txpower		= r88ee_get_txpower;
	rs->rs_set_gain			= r88ee_set_gain;
	rs->rs_tx_enable_ampdu		= r88ee_tx_enable_ampdu;
	rs->rs_tx_setup_hwseq		= r88ee_tx_setup_hwseq;
	rs->rs_tx_setup_macid		= r88ee_tx_setup_macid;
	rs->rs_set_name			= r88eee_set_name;

	/* XXX TODO: test with net80211 ratectl! */
#ifndef RTWN_WITHOUT_UCODE
	rs->rs_c2h_timeout		= hz;

	callout_init_mtx(&rs->rs_c2h_report, &sc->sc_mtx, 0);
#endif

	rs->rf_read_delay[0]		= 1000;
	rs->rf_read_delay[1]		= 1000;
	rs->rf_read_delay[2]		= 1000;

	sc->sc_priv = rs;
}

static void
r88eee_adj_devcaps(struct rtwn_softc *sc)
{
#if 0
	struct ieee80211com *ic = &sc->sc_ic;

	/*
	 * XXX do NOT enable PMGT until RSVD_PAGE command
	 * will not be tested / fixed + HRPWM register must be set too.
	 */
	ic->ic_caps &= ~IEEE80211_C_PMGT;
#else
	struct ieee80211com *ic = &sc->sc_ic;
	ic->ic_caps &= ~IEEE80211_C_PMGT;
	printf("RTL8188EE:%s:%s not implemented\n", __FILE__, __func__);
#endif
}

/*
 * Assigns PCI methods
 */

void
r88eee_attach(struct rtwn_pci_softc *pc)
{
	struct rtwn_softc *sc		= &pc->pc_sc;

	/* PCIe part. */
	pc->pc_setup_tx_desc		= r88eee_setup_tx_desc;
	pc->pc_tx_postsetup		= r88eee_tx_postsetup;
	pc->pc_copy_tx_desc		= r88eee_copy_tx_desc;
	pc->pc_enable_intr		= r88eee_enable_intr;

	pc->pc_qmap			= 0xf771;
	pc->tcr				=
	    R88EE_TCR_CFENDFORM | (1 << 12) | (1 << 13);

	/* Common part. */
	/* RTL8188EE* cannot use pairwise keys from first 4 slots */
	sc->sc_flags			= RTWN_FLAG_EXT_HDR; // RTWN_FLAG_CAM_FIXED;

	sc->sc_start_xfers		= r88eee_start_xfers;
	sc->sc_set_chan			= r88ee_set_chan;
	sc->sc_fill_tx_desc		= r88ee_fill_tx_desc;
	sc->sc_fill_tx_desc_raw		= r88ee_fill_tx_desc_raw;
	sc->sc_fill_tx_desc_null	= r88ee_fill_tx_desc_null; /* XXX recheck */
	sc->sc_dump_tx_desc		= r88eee_dump_tx_desc;
	sc->sc_tx_radiotap_flags	= r88ee_tx_radiotap_flags;
	sc->sc_rx_radiotap_flags	= r88ee_rx_radiotap_flags;
	sc->sc_get_rssi_cck		= r88ee_get_rssi_cck;
	sc->sc_get_rssi_ofdm		= r88ee_get_rssi_ofdm;
	sc->sc_classify_intr		= r88eee_classify_intr;
	sc->sc_handle_tx_report		= rtwn_nop_softc_uint8_int;
	sc->sc_handle_c2h_report	= rtwn_nop_softc_uint8_int;
	sc->sc_check_frame		= rtwn_nop_int_softc_mbuf;
	sc->sc_rf_read			= r88ee_rf_read;
	sc->sc_rf_write			= r88ee_rf_write;
	sc->sc_check_condition		= r88ee_check_condition;
	sc->sc_efuse_postread		= r88ee_efuse_postread;
	sc->sc_parse_rom		= r88ee_parse_rom;
	sc->sc_set_led			= r88eee_set_led;
	sc->sc_power_on			= r88eee_power_on;
	sc->sc_power_off		= r88eee_power_off;
#ifndef RTWN_WITHOUT_UCODE
	sc->sc_fw_reset			= r88eee_fw_reset;
	sc->sc_fw_download_enable	= r88ee_fw_download_enable;
#endif
	sc->sc_set_page_size		= r88ee_set_page_size;
	sc->sc_lc_calib			= r88ee_lc_calib;
	sc->sc_iq_calib			= r88eee_iq_calib;
	sc->sc_read_chipid_vendor	= r88ee_read_chipid_vendor;
	sc->sc_adj_devcaps		= r88eee_adj_devcaps;
	sc->sc_vap_preattach		= rtwn_nop_softc_vap;
	sc->sc_postattach		= r88eee_postattach;
	sc->sc_detach_private		= r88ee_detach_private;
	sc->sc_set_media_status		= r88ee_joinbss_rpt;
#ifndef RTWN_WITHOUT_UCODE
	sc->sc_set_rsvd_page		= r88ee_set_rsvd_page;
	sc->sc_set_pwrmode		= r88ee_set_pwrmode;
	sc->sc_set_rssi			= r88ee_set_rssi;
#endif
	sc->sc_beacon_init		= r88ee_beacon_init;
	sc->sc_beacon_enable		= r88ee_beacon_enable;
	sc->sc_beacon_set_rate		= rtwn_nop_void_int;
	sc->sc_beacon_select		= rtwn_nop_softc_int;
	sc->sc_temp_measure		= r88ee_temp_measure;
	sc->sc_temp_read		= r88ee_temp_read;
	sc->sc_init_tx_agg		= rtwn_nop_softc;
	sc->sc_init_rx_agg		= rtwn_nop_softc;
	sc->sc_init_ampdu		= r88eee_init_ampdu;
	sc->sc_init_intr		= r88eee_init_intr;
	sc->sc_init_edca		= r88eee_init_edca;
	sc->sc_init_bb			= r88eee_init_bb;
	sc->sc_init_rf			= r88ee_init_rf;
	sc->sc_init_antsel		= rtwn_nop_softc;
	sc->sc_post_init		= r88eee_post_init;
	sc->sc_init_bcnq1_boundary	= rtwn_nop_int_softc;

	sc->mac_prog			= &rtl8188eee_mac[0];
	sc->mac_size			= nitems(rtl8188eee_mac);
	sc->bb_prog			= &rtl8188eee_bb[0];
	sc->bb_size			= nitems(rtl8188eee_bb);
	sc->agc_prog			= &rtl8188ee_agc[0];
	sc->agc_size			= nitems(rtl8188ee_agc);
	sc->rf_prog			= &rtl8188ee_rf[0];

	sc->page_count			= R88EEE_TX_PAGE_COUNT;
	sc->pktbuf_count		= R88EE_TXPKTBUF_COUNT;

	sc->ackto			= 0x40;
	sc->npubqpages			= R88EEE_PUBQ_NPAGES;
	sc->nhqpages			= R88EEE_HPQ_NPAGES;
	sc->nnqpages			= 0;
	sc->nlqpages			= R88EEE_LPQ_NPAGES;
	sc->page_size			= R88EE_TX_PAGE_SIZE;

	sc->txdesc_len			= sizeof(struct r88eee_tx_desc);
	sc->efuse_maxlen		= R88EE_EFUSE_MAX_LEN;
	sc->efuse_maplen		= R88EE_EFUSE_MAP_LEN;
	sc->rx_dma_size			= R88EE_RX_DMA_BUFFER_SIZE;

	sc->macid_limit			= R88EE_MACID_MAX + 1;
	sc->cam_entry_limit		= R88EE_CAM_ENTRY_COUNT;
	sc->fwsize_limit		= R88EE_MAX_FW_SIZE;
	sc->temp_delta			= R88EE_CALIB_THRESHOLD;

	sc->bcn_status_reg[0]		= R88EE_TDECTRL;
	/*
	 * TODO: some additional setup is required
	 * to maintain few beacons at the same time.
	 *
	 * XXX BCNQ1 mechanism is not needed here; move it to the USB module.
	 */
	sc->bcn_status_reg[1]		= R88EE_TDECTRL;
	sc->rcr				= 0;

	r88eee_attach_private(sc);
}
