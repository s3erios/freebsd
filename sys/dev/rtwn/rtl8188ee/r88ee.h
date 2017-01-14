/*-
 * Copyright (c) 2010 Damien Bergamini <damien.bergamini@free.fr>
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
 *
 * $OpenBSD: if_urtwnreg.h,v 1.3 2010/11/16 18:02:59 damien Exp $
 * $FreeBSD$
 */

#ifndef RTL8192C_H
#define RTL8192C_H

/*
 * Global definitions.
 */
#define R92C_TXPKTBUF_COUNT	256

#define R92C_TX_PAGE_SIZE	128
#define R92C_RX_DMA_BUFFER_SIZE	0x2800

#define R92C_MAX_FW_SIZE	0x4000
#define R92C_MACID_MAX		31
#define R92C_CAM_ENTRY_COUNT	32

#define R92C_CALIB_THRESHOLD	2


/*
 * Function declarations.
 */
/* r88ee_attach.c */
void	r88ee_detach_private(struct rtwn_softc *);
void	r88ee_read_chipid_vendor(struct rtwn_softc *, uint32_t);

/* r88ee_beacon.c */
void	r88ee_beacon_init(struct rtwn_softc *, void *, int);
void	r88ee_beacon_enable(struct rtwn_softc *, int, int);

/* r88ee_calib.c */
void	r88ee_iq_calib(struct rtwn_softc *);
void	r88ee_lc_calib(struct rtwn_softc *);
void	r88ee_temp_measure(struct rtwn_softc *);
uint8_t	r88ee_temp_read(struct rtwn_softc *);

/* r88ee_chan.c */
void	r88ee_get_txpower(struct rtwn_softc *, int,
	    struct ieee80211_channel *, uint16_t[]);
void	r88ee_set_bw20(struct rtwn_softc *, uint8_t);
void	r88ee_set_chan(struct rtwn_softc *, struct ieee80211_channel *);
void	r88ee_set_gain(struct rtwn_softc *, uint8_t);
void	r88ee_scan_start(struct ieee80211com *);
void	r88ee_scan_end(struct ieee80211com *);

/* r88ee_fw.c */
#ifndef RTWN_WITHOUT_UCODE
void	r88ee_fw_reset(struct rtwn_softc *, int);
void	r88ee_fw_download_enable(struct rtwn_softc *, int);
#endif
void	r88ee_joinbss_rpt(struct rtwn_softc *, int);
#ifndef RTWN_WITHOUT_UCODE
int	r88ee_set_rsvd_page(struct rtwn_softc *, int, int, int);
int	r88ee_set_pwrmode(struct rtwn_softc *, struct ieee80211vap *, int);
void	r88ee_set_rssi(struct rtwn_softc *);
void	r88ee_handle_c2h_report(void *);
#endif

/* r88ee_init.c */
int	r88ee_check_condition(struct rtwn_softc *, const uint8_t[]);
int	r88ee_set_page_size(struct rtwn_softc *);
void	r88ee_init_bb_common(struct rtwn_softc *);
int	r88ee_init_rf_chain(struct rtwn_softc *,
	    const struct rtwn_rf_prog *, int);
void	r88ee_init_rf(struct rtwn_softc *);
void	r88ee_init_edca(struct rtwn_softc *);
void	r88ee_init_ampdu(struct rtwn_softc *);
void	r88ee_init_antsel(struct rtwn_softc *);
void	r88ee_pa_bias_init(struct rtwn_softc *);

/* r88ee_rf.c */
uint32_t	r88ee_rf_read(struct rtwn_softc *, int, uint8_t);
void		r88ee_rf_write(struct rtwn_softc *, int, uint8_t, uint32_t);

/* r88ee_rom.c */
void	r88ee_efuse_postread(struct rtwn_softc *);
void	r88ee_parse_rom(struct rtwn_softc *, uint8_t *);

/* r88ee_rx.c */
int8_t	r88ee_get_rssi_cck(struct rtwn_softc *, void *);
int8_t	r88ee_get_rssi_ofdm(struct rtwn_softc *, void *);
uint8_t	r88ee_rx_radiotap_flags(const void *);

/* r88ee_tx.c */
void	r88ee_tx_enable_ampdu(void *, int);
void	r88ee_tx_setup_hwseq(void *);
void	r88ee_tx_setup_macid(void *, int);
void	r88ee_fill_tx_desc(struct rtwn_softc *, struct ieee80211_node *,
	    struct mbuf *, void *, uint8_t, int);
void	r88ee_fill_tx_desc_raw(struct rtwn_softc *, struct ieee80211_node *,
	    struct mbuf *, void *, const struct ieee80211_bpf_params *);
void	r88ee_fill_tx_desc_null(struct rtwn_softc *, void *, int, int, int);
uint8_t	r88ee_tx_radiotap_flags(const void *);

#endif	/* RTL8192C_H */
