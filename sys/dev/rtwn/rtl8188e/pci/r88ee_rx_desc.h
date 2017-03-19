/*	$OpenBSD: if_rtwnreg.h,v 1.3 2015/06/14 08:02:47 stsp Exp $	*/

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
 * 
 * $FreeBSD: head/sys/dev/rtwn/rtl8188ee/pci/r88eee_rx_desc.h 307529 2016-10-17 20:38:24Z avos $
 */

#ifndef R88EEE_RX_DESC_H
#define R88EEE_RX_DESC_H

#include <dev/rtwn/rtl8188ee/r88ee_rx_desc.h>

/* Rx MAC descriptor (PCIe). */
struct r88eee_rx_stat {
	uint32_t	rxdw0;
	uint32_t	rxdw1;
	uint32_t	rxdw2;
	uint32_t	rxdw3;
	uint32_t	rxdw4;
	uint32_t	tsf_low;

	uint32_t	rxbufaddr;
	uint32_t	rxbufaddr64;
} __packed __attribute__((aligned(4)));

#endif	/* R88EEE_RX_DESC_H */
