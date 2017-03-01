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
 * $OpenBSD: if_urtwnreg.h,v 1.3 2010/11/16 18:02:59 damien Exp $
 * $FreeBSD: head/sys/dev/rtwn/rtl8188ee/r88ee_rom_defs.h 307529 2016-10-17 20:38:24Z avos $
 */

#ifndef R88EE_ROM_DEFS_H
#define R88EE_ROM_DEFS_H

#define R88EE_MAX_CHAINS		2 // 4 // Done, taken from Linux's MAX_RF_PATH
#define R88EE_GROUP_2G			3 // 6 // Done, taken from Linux's MAX_CHNL_GROUP_24G

#define R88EE_EFUSE_MAX_LEN		512
#define R88EE_EFUSE_MAP_LEN		512 // Originally 128 

/*
 * Some generic rom parsing macros.
 */
#define RTWN_GET_ROM_VAR(var, def)	(((var) != 0xff) ? (var) : (def))
#define RTWN_SIGN4TO8(val)		(((val) & 0x08) ? (val) | 0xf0 : (val))

#define LOW_PART_M	0x0f
#define LOW_PART_S	0
#define HIGH_PART_M	0xf0
#define HIGH_PART_S	4

/* Bits for rf_board_opt (rf_opt1) field. */
#define R88EE_ROM_RF1_REGULATORY_M	0x07
#define R88EE_ROM_RF1_REGULATORY_S	0
#define R88EE_ROM_RF1_BOARD_TYPE_M	0xe0
#define R88EE_ROM_RF1_BOARD_TYPE_S	5

/* Generic board types. */
#define R88EE_BOARD_TYPE_DONGLE		0
#define R88EE_BOARD_TYPE_HIGHPA		1
#define R88EE_BOARD_TYPE_MINICARD	2
#define R88EE_BOARD_TYPE_SOLO		3
#define R88EE_BOARD_TYPE_COMBO		4

/* Bits for channel_plan field. */
#define R88EE_CHANNEL_PLAN_BY_HW		0x80

#endif	/* R88EE_ROM_DEFS_H */
