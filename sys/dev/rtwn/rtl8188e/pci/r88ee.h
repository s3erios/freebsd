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

#ifndef RTL8188EE_H
#define RTL8188EE_H

#include <dev/rtwn/rtl8188e/r88e.h>


/*
 * Global definitions.
 */
#define R88EE_PUBQ_NPAGES	176
#define R88EE_HPQ_NPAGES	41
#define R88EE_LPQ_NPAGES	28
#define R88EE_TX_PAGE_COUNT	\
	(R88EE_PUBQ_NPAGES + R88EE_HPQ_NPAGES + R88EE_LPQ_NPAGES)


/*
 * Function declarations.
 */
/* r88ee_calib.c */
void	r88ee_iq_calib(struct rtwn_softc *);

/* r88ee_fw.c */
#ifndef RTWN_WITHOUT_UCODE
void	r88ee_fw_reset(struct rtwn_softc *, int);
#endif

/* r88ee_init.c */
void	r88ee_init_intr(struct rtwn_softc *);
void	r88ee_init_edca(struct rtwn_softc *);
void	r88ee_init_bb(struct rtwn_softc *);
int	r88ee_power_on(struct rtwn_softc *);
void	r88ee_power_off(struct rtwn_softc *);
void	r88ee_init_ampdu(struct rtwn_softc *);
void	r88ee_post_init(struct rtwn_softc *);

/* r88ee_led.c */
void	r88ee_set_led(struct rtwn_softc *, int, int);

/* r88ee_rx.c */
int	r88ee_classify_intr(struct rtwn_softc *, void *, int);
void	r88ee_enable_intr(struct rtwn_pci_softc *);
void	r88ee_start_xfers(struct rtwn_softc *);

/* r88ee_tx.c */
void	r88ee_setup_tx_desc(struct rtwn_pci_softc *, void *, uint32_t);
void	r88ee_tx_postsetup(struct rtwn_pci_softc *, void *,
	    bus_dma_segment_t[]);
void	r88ee_copy_tx_desc(void *, const void *);
void	r88ee_dump_tx_desc(struct rtwn_softc *, const void *);

#endif	/* RTL8188EE_H */
