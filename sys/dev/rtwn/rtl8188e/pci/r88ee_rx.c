/*	$OpenBSD: if_rtwn.c,v 1.6 2015/08/28 00:03:53 deraadt Exp $	*/

/*-
 * Copyright (c) 2017 Farhan Khan <khanzf@gmail.com>
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
__FBSDID("$FreeBSD: head/sys/dev/rtwn/rtl8188eee/pci/r88eee_rx.c 307529 2016-10-17 20:38:24Z avos $");

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
#include <dev/rtwn/if_rtwn_debug.h>

#include <dev/rtwn/pci/rtwn_pci_var.h>

#include <dev/rtwn/rtl8188ee/pci/r88eee.h>
#include <dev/rtwn/rtl8188ee/pci/r88eee_reg.h>


int
r88eee_classify_intr(struct rtwn_softc *sc, void *arg, int len __unused)
{
#if 0
	uint32_t status;
	int *rings = arg;
	int ret;

	*rings = 0;
	status = rtwn_read_4(sc, R88EE_HISR);
	RTWN_DPRINTF(sc, RTWN_DEBUG_INTR, "%s: HISR %08X, HISRE %04X\n",
	    __func__, status, rtwn_read_2(sc, R88EE_HISRE));
	if (status == 0 || status == 0xffffffff)
		return (0);

	/* Disable interrupts. */
	rtwn_write_4(sc, R88EE_HIMR, 0);

	/* Ack interrupts. */
	rtwn_write_4(sc, R88EE_HISR, status);

	if (status & R88EE_IMR_BDOK)
		*rings |= (1 << RTWN_PCI_BEACON_QUEUE);
	if (status & R88EE_IMR_HIGHDOK)
		*rings |= (1 << RTWN_PCI_HIGH_QUEUE);
	if (status & R88EE_IMR_MGNTDOK)
		*rings |= (1 << RTWN_PCI_MGNT_QUEUE);
	if (status & R88EE_IMR_BKDOK)
		*rings |= (1 << RTWN_PCI_BK_QUEUE);
	if (status & R88EE_IMR_BEDOK)
		*rings |= (1 << RTWN_PCI_BE_QUEUE);
	if (status & R88EE_IMR_VIDOK)
		*rings |= (1 << RTWN_PCI_VI_QUEUE);
	if (status & R88EE_IMR_VODOK)
		*rings |= (1 << RTWN_PCI_VO_QUEUE);

	ret = 0;
	if (status & R88EE_IMR_RXFOVW)
		ret |= RTWN_PCI_INTR_RX_OVERFLOW;
	if (status & R88EE_IMR_RDU)
		ret |= RTWN_PCI_INTR_RX_DESC_UNAVAIL;
	if (status & R88EE_IMR_ROK)
		ret |= RTWN_PCI_INTR_RX_DONE;
	if (status & R88EE_IMR_TXFOVW)
		ret |= RTWN_PCI_INTR_TX_OVERFLOW;
	if (status & R88EE_IMR_PSTIMEOUT)
		ret |= RTWN_PCI_INTR_PS_TIMEOUT;

	return (ret);
#else
	printf("RTL8188EE:%s not implemented\n", __func__);
	return 0;
#endif
}

#define R88EE_INT_ENABLE (R88EE_IMR_ROK | R88EE_IMR_VODOK | R88EE_IMR_VIDOK | \
			R88EE_IMR_BEDOK | R88EE_IMR_BKDOK | R88EE_IMR_MGNTDOK | \
			R88EE_IMR_HIGHDOK | R88EE_IMR_BDOK | R88EE_IMR_RDU | \
			R88EE_IMR_RXFOVW)
void
r88eee_enable_intr(struct rtwn_pci_softc *pc)
{
#if 0
	struct rtwn_softc *sc = &pc->pc_sc;

	/* Enable interrupts. */
	rtwn_write_4(sc, R88EE_HIMR, R88EE_INT_ENABLE);
#else
	printf("RTL8188EE:%s not implemented\n", __func__);
#endif
}

void
r88eee_start_xfers(struct rtwn_softc *sc)
{
#if 0
	/* Clear pending interrupts. */
	rtwn_write_4(sc, R88EE_HISR, 0xffffffff);

	/* Enable interrupts. */
	rtwn_write_4(sc, R88EE_HIMR, R88EE_INT_ENABLE);
#else
	printf("RTL8188EE:%s not implemented\n", __func__);
#endif
}
#undef R88EE_INT_ENABLE
