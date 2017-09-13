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

#ifndef R88EE_PRIV_H
#define R88EE_PRIV_H

#include <dev/rtwn/rtl8188e/r88e_priv.h>


/*
 * MAC initialization values.
 */
static const struct rtwn_mac_prog rtl8188ee_mac[] = {
	{ 0x026, 0x41 }, { 0x027, 0x35 }, { 0x428, 0x0A }, { 0x429, 0x10 },
	{ 0x430, 0x00 }, { 0x431, 0x01 }, { 0x432, 0x02 }, { 0x433, 0x04 },
	{ 0x434, 0x05 }, { 0x435, 0x06 }, { 0x436, 0x07 }, { 0x437, 0x08 },
	{ 0x438, 0x00 }, { 0x439, 0x00 }, { 0x43A, 0x01 }, { 0x43B, 0x02 },
	{ 0x43C, 0x04 }, { 0x43D, 0x05 }, { 0x43E, 0x06 }, { 0x43F, 0x07 },
	{ 0x440, 0x5D }, { 0x441, 0x01 }, { 0x442, 0x00 }, { 0x444, 0x15 },
	{ 0x445, 0xF0 }, { 0x446, 0x0F }, { 0x447, 0x00 }, { 0x458, 0x41 },
	{ 0x459, 0xA8 }, { 0x45A, 0x72 }, { 0x45B, 0xB9 }, { 0x460, 0x66 },
	{ 0x461, 0x66 }, { 0x480, 0x08 }, { 0x4C8, 0xFF }, { 0x4C9, 0x08 },
	{ 0x4CC, 0xFF }, { 0x4CD, 0xFF }, { 0x4CE, 0x01 }, { 0x4D3, 0x01 },
	{ 0x500, 0x26 }, { 0x501, 0xA2 }, { 0x502, 0x2F }, { 0x503, 0x00 },
	{ 0x504, 0x28 }, { 0x505, 0xA3 }, { 0x506, 0x5E }, { 0x507, 0x00 },
	{ 0x508, 0x2B }, { 0x509, 0xA4 }, { 0x50A, 0x5E }, { 0x50B, 0x00 },
	{ 0x50C, 0x4F }, { 0x50D, 0xA4 }, { 0x50E, 0x00 }, { 0x50F, 0x00 },
	{ 0x512, 0x1C }, { 0x514, 0x0A }, { 0x516, 0x0A }, { 0x525, 0x4F },
	{ 0x550, 0x10 }, { 0x551, 0x10 }, { 0x559, 0x02 }, { 0x55D, 0xFF },
	{ 0x605, 0x30 }, { 0x608, 0x0E }, { 0x609, 0x2A }, { 0x620, 0xFF },
	{ 0x621, 0xFF }, { 0x622, 0xFF }, { 0x623, 0xFF }, { 0x624, 0xFF },
	{ 0x625, 0xFF }, { 0x626, 0xFF }, { 0x627, 0xFF }, { 0x652, 0x20 },
	{ 0x63C, 0x0A }, { 0x63D, 0x0A }, { 0x63E, 0x0E }, { 0x63F, 0x0E },
	{ 0x640, 0x40 }, { 0x66E, 0x05 }, { 0x700, 0x21 }, { 0x701, 0x43 },
	{ 0x702, 0x65 }, { 0x703, 0x87 }, { 0x708, 0x21 }, { 0x709, 0x43 },
	{ 0x70A, 0x65 }, { 0x70B, 0x87 }
};


/*
 * Baseband initialization values.
 */
static const uint16_t rtl8188ee_bb_regs0[] = {
#if 0
	0x0t4, 0x028, 0x800, 0x804, 0x808, 0x80c, 0x810, 0x814, 0x818,
	0x81c, 0x820, 0x824, 0x828, 0x82c, 0x830, 0x834, 0x838, 0x83c,
	0x840, 0x844, 0x848, 0x84c, 0x850, 0x854, 0x858, 0x85c, 0x860,
	0x864, 0x868, 0x86c, 0x870, 0x874, 0x878, 0x87c, 0x880, 0x884,
	0x888, 0x88c, 0x890, 0x894, 0x898, 0x89c, 0x900, 0x904, 0x908,
	0x90c, 0xa00, 0xa04, 0xa08, 0xa0c, 0xa10, 0xa14, 0xa18, 0xa1c,
	0xa20, 0xa24, 0xa28, 0xa2c, 0xa70, 0xa74, 0xc00, 0xc04
}, rtl8192ce_bb_regs1[] = {
	0xc08, 0xc0c, 0xc10, 0xc14, 0xc18, 0xc1c, 0xc20, 0xc24, 0xc28,
	0xc2c, 0xc30, 0xc34, 0xc38, 0xc3c, 0xc40, 0xc44, 0xc48, 0xc4c,
	0xc50, 0xc54, 0xc58, 0xc5c, 0xc60, 0xc64, 0xc68, 0xc6c, 0xc70,
	0xc74, 0xc78, 0xc7c, 0xc80, 0xc84, 0xc88, 0xc8c, 0xc90, 0xc94,
	0xc98, 0xc9c, 0xca0, 0xca4, 0xca8, 0xcac, 0xcb0, 0xcb4, 0xcb8,
	0xcbc, 0xcc0, 0xcc4, 0xcc8, 0xccc, 0xcd0, 0xcd4, 0xcd8, 0xcdc,
	0xce0, 0xce4, 0xce8, 0xcec, 0xd00
#endif
};

static const uint32_t rtl8188ee_bb_vals0_2t[] = {
#if 0
	0x0011800f, 0x00ffdb83, 0x80040002, 0x00000003, 0x0000fc00,
	0x0000000a, 0x10005388, 0x020c3d10, 0x02200385, 0x00000000,
	0x01000100, 0x00390004, 0x01000100, 0x00390004, 0x27272727,
	0x27272727, 0x27272727, 0x27272727, 0x00010000, 0x00010000,
	0x27272727, 0x27272727, 0x00000000, 0x00000000, 0x569a569a,
	0x0c1b25a4, 0x66e60230, 0x061f0130, 0x27272727, 0x2b2b2b27,
	0x07000700, 0x22184000, 0x08080808, 0x00000000, 0xc0083070,
	0x000004d5, 0x00000000, 0xcc0000c0, 0x00000800, 0xfffffffe,
	0x40302010, 0x00706050, 0x00000000, 0x00000023, 0x00000000,
	0x81121313, 0x00d047c8, 0x80ff000c, 0x8c838300, 0x2e68120f,
	0x9500bb78, 0x11144028, 0x00881117, 0x89140f00, 0x1a1b0000,
	0x090e1317, 0x00000204, 0x00d30000, 0x101fbf00, 0x00000007,
	0x48071d40, 0x03a05633
}, rtl8192ce_bb_vals0_1t[] = {
	0x0011800f, 0x00ffdb83, 0x80040000, 0x00000001, 0x0000fc00,
	0x0000000a, 0x10005388, 0x020c3d10, 0x02200385, 0x00000000,
	0x01000100, 0x00390004, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x569a569a,
	0x001b25a4, 0x66e60230, 0x061f0130, 0x00000000, 0x32323200,
	0x07000700, 0x22004000, 0x00000808, 0x00000000, 0xc0083070,
	0x000004d5, 0x00000000, 0xccc000c0, 0x00000800, 0xfffffffe,
	0x40302010, 0x00706050, 0x00000000, 0x00000023, 0x00000000,
	0x81121111, 0x00d047c8, 0x80ff000c, 0x8c838300, 0x2e68120f,
	0x9500bb78, 0x11144028, 0x00881117, 0x89140f00, 0x1a1b0000,
	0x090e1317, 0x00000204, 0x00d30000, 0x101fbf00, 0x00000007,
	0x48071d40, 0x03a05611
}, rtl8192ce_bb_vals1[] = {
	0x000000e4, 0x6c6c6c6c, 0x08800000, 0x40000100, 0x08800000,
	0x40000100, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x69e9ac44, 0x469652cf, 0x49795994, 0x0a97971c, 0x1f7c403f,
	0x000100b7, 0xec020107, 0x007f037f, 0x69543420, 0x43bc0094,
	0x69543420, 0x433c0094, 0x00000000, 0x5116848b, 0x47c00bff,
	0x00000036, 0x2c7f000d, 0x018610db, 0x0000001f, 0x00b91612,
	0x40000100, 0x20f60000, 0x40000100, 0x20200000, 0x00121820,
	0x00000000, 0x00121820, 0x00007f7f, 0x00000000, 0x00000080,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x28000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x64b22427, 0x00766932, 0x00222222,
	0x00000000, 0x37644302, 0x2f97d40c, 0x00080740
}, rtl8192ce_bb_vals4_1t[] = {
	0x00000010, 0x001b25a4, 0x631b25a0, 0x631b25a0, 0x081b25a0,
	0x081b25a0, 0x081b25a0, 0x081b25a0, 0x631b25a0, 0x081b25a0,
	0x631b25a0, 0x631b25a0, 0x631b25a0, 0x631b25a0, 0x001b25a0,
	0x001b25a0, 0x6b1b25a0, 0x00000003, 0x00000000, 0x00000300
#endif
};

static const struct rtwn_bb_prog rtl8188ee_bb[] = {
#if 0
	{
		nitems(rtl8192ce_bb_regs0),
		rtl8192ce_bb_regs0,
		rtl8192ce_bb_vals0_2t,
		{ R92C_COND_RTL8192C },
		&(const struct rtwn_bb_prog){
			nitems(rtl8192ce_bb_regs0),
			rtl8192ce_bb_regs0,
			rtl8192ce_bb_vals0_1t,
			{ 0 },
			NULL
		}
	},
	{
		nitems(rtl8192ce_bb_regs1),
		rtl8192ce_bb_regs1,
		rtl8192ce_bb_vals1,
		{ 0 },
		NULL
	},
	{
		nitems(rtl8192c_bb_regs3),
		rtl8192c_bb_regs3,
		rtl8192c_bb_vals3_92ce_92cu,
		{ R92C_COND_RTL8192C },
		&(const struct rtwn_bb_prog){
			nitems(rtl8192c_bb_regs3),
			rtl8192c_bb_regs3,
			rtl8192c_bb_vals3_88cu_88ru,
			{ 0 },
			NULL
		}
	},
	{
		nitems(rtl8192c_bb_regs4),
		rtl8192c_bb_regs4,
		rtl8192c_bb_vals4,
		{ 0 },
		NULL
	},
	{
		nitems(rtl8192c_bb_regs5),
		rtl8192c_bb_regs5,
		rtl8192c_bb_vals5_92ce_92cu,
		{ R92C_COND_RTL8192C },
		&(const struct rtwn_bb_prog){
			nitems(rtl8192c_bb_regs5),
			rtl8192c_bb_regs5,
			rtl8192ce_bb_vals4_1t,
			{ 0 },
			NULL
		}
	}
#endif
};

#endif	/* R88EE_PRIV_H */
