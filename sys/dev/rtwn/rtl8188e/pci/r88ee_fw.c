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

#include <dev/rtwn/rtl8188e/pci/r88ee.h>


#ifndef RTWN_WITHOUT_UCODE
void
r88ee_fw_reset(struct rtwn_softc *sc, int reason)
{
#if 0

	if (reason == RTWN_FW_RESET_CHECKSUM)
		return;

	r88e_fw_reset(sc, reason);

	/*
	 * We must sleep for one second to let the firmware settle.
	 * Accessing registers too early will hang the whole system.
	 */
	if (reason == RTWN_FW_RESET_DOWNLOAD)
		rtwn_delay(sc, 1000 * 1000);

	printf("RTL8188EE:%s:%s Function Trace\n", __FILE__, __func__);
#else
	uint8_t u1b_tmp;
#define REG_SYS_FUNC_EN		0x0002
	u1b_tmp = rtwn_read_1(sc, REG_SYS_FUNC_EN+1);
	rtwn_write_1(sc, REG_SYS_FUNC_EN+1, (u1b_tmp & (~0x04)));
						// BIT(2) is 0x04
	rtwn_write_1(sc, REG_SYS_FUNC_EN+1, (u1b_tmp | 0x04));
						// BIT(2) is 0x04

	printf("RTL8188EE:%s:%s not fully implemented\n", __FILE__, __func__);
#endif
}
#endif
