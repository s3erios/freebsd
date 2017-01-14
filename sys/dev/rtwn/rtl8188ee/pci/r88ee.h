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

#ifndef RTL8192CE_H
#define RTL8192CE_H

#include <dev/rtwn/rtl8188ee/r88ee.h>


/*
 * Global definitions.
 */
#define R92CE_PUBQ_NPAGES	176
#define R92CE_HPQ_NPAGES	41
#define R92CE_LPQ_NPAGES	28
#define R92CE_TX_PAGE_COUNT	\
	(R92CE_PUBQ_NPAGES + R92CE_HPQ_NPAGES + R92CE_LPQ_NPAGES)


/*
 * Function declarations.
 */
/* r88eee_calib.c */
void	r88eee_iq_calib(struct rtwn_softc *);

/* r88eee_fw.c */
#ifndef RTWN_WITHOUT_UCODE
void	r88eee_fw_reset(struct rtwn_softc *, int);
#endif

/* r88eee_init.c */
void	r88eee_init_intr(struct rtwn_softc *);
void	r88eee_init_edca(struct rtwn_softc *);
void	r88eee_init_bb(struct rtwn_softc *);
int	r88eee_power_on(struct rtwn_softc *);
void	r88eee_power_off(struct rtwn_softc *);
void	r88eee_init_ampdu(struct rtwn_softc *);
void	r88eee_post_init(struct rtwn_softc *);

/* r88eee_led.c */
void	r88eee_set_led(struct rtwn_softc *, int, int);

/* r88eee_rx.c */
int	r88eee_classify_intr(struct rtwn_softc *, void *, int);
void	r88eee_enable_intr(struct rtwn_pci_softc *);
void	r88eee_start_xfers(struct rtwn_softc *);

/* r88eee_tx.c */
void	r88eee_setup_tx_desc(struct rtwn_pci_softc *, void *, uint32_t);
void	r88eee_tx_postsetup(struct rtwn_pci_softc *, void *,
	    bus_dma_segment_t[]);
void	r88eee_copy_tx_desc(void *, const void *);
void	r88eee_dump_tx_desc(struct rtwn_softc *, const void *);

#endif	/* RTL8192CE_H */
