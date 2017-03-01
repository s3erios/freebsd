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
 * $FreeBSD: head/sys/dev/rtwn/rtl8188ee/r88ee_rom_image.h 307529 2016-10-17 20:38:24Z avos $
 */

#ifndef R88EE_ROM_IMAGE_H
#define R88EE_ROM_IMAGE_H

#include <dev/rtwn/rtl8188ee/r88ee_rom_defs.h>

/*
 * RTL8188EE ROM image.
 */

struct r88ee_rom {
	uint16_t	id;				/* Offset: 0, Must be 0x8129 */
//	uint8_t		cck_tx_pwr[2];			/* Offset: 2, This is a guess!!!!!! */
//	uint8_t		ht40_tx_pwr[1];			/* Offset: 4, Guess based on r80e */
//	uint8_t		tx_pwr_diff;			/* Offset: 5, 3rd guess */
	
	uint8_t		unknown_1[23];
	uint8_t		macaddr[IEEE80211_ADDR_LEN];	/* Offset: 22+28 so 6 bytes, Begins with 00:E0:4C in little endian */
	uint8_t		test;

//	uint8_t		unknown_2[28];			/* Offset: 28 */

	uint8_t		cck_tx_pwr[2][14];		/* Offset: 32 */ // The define's need 

	uint8_t		unknown_3[133];			/* Offset: 56 */

	// Ofsets come from here:
	// https://github.com/lwfinger/rtlwifi_new/blob/b6a14442fea8e059662a52534692a50c63634c4e/rtl8188ee/reg.h#L704
	uint8_t		rf_opt1;			/* Offset: 193 */ // Comes from 0xC1 from Linux's 
	uint8_t		rf_opt2;			/* Offset: 194 */
	uint8_t		rf_opt3;			/* Offset: 195 */
	uint8_t		rf_opt4;			/* Offset: 196 */

	uint8_t		unknown_4[315]; // This length is whatever minus 512
};

/*
 * RTL8192C ROM image.
 */
struct r88ee_rom_old {
	uint16_t	id;		/* Offset: 0, Must be 0x8188 */
	uint8_t		reserved1[5];
	uint8_t		dbg_sel;
	uint16_t	reserved2;
	uint16_t	vid;
	uint16_t	pid;
	uint8_t		usb_opt;
	uint8_t		ep_setting;
	uint16_t	reserved3;
	uint8_t		usb_phy;
	uint8_t		reserved4[3];
	uint8_t		macaddr[IEEE80211_ADDR_LEN]; /* Offset: 22, 6 bytes */
	uint8_t		string[61];	/* "Realtek" */
	uint8_t		subcustomer_id;
	uint8_t		cck_tx_pwr[R88EE_MAX_CHAINS][R88EE_GROUP_2G];
	uint8_t		ht40_1s_tx_pwr[R88EE_MAX_CHAINS][R88EE_GROUP_2G];
	uint8_t		ht40_2s_tx_pwr_diff[R88EE_GROUP_2G];
	uint8_t		ht20_tx_pwr_diff[R88EE_GROUP_2G];
	uint8_t		ofdm_tx_pwr_diff[R88EE_GROUP_2G];
	uint8_t		ht40_max_pwr[R88EE_GROUP_2G];
	uint8_t		ht20_max_pwr[R88EE_GROUP_2G];
	uint8_t		xtal_calib;
	uint8_t		tssi[R88EE_MAX_CHAINS];
	uint8_t		thermal_meter;
#define R88EE_ROM_THERMAL_METER_M	0x1f
#define R88EE_ROM_THERMAL_METER_S	0

	uint8_t		rf_opt1;
	uint8_t		rf_opt2;
	uint8_t		rf_opt3;
	uint8_t		rf_opt4;
	uint8_t		channel_plan;
#define R88EE_CHANNEL_PLAN_BY_HW		0x80

	uint8_t		version;
	uint8_t		customer_id;
} __packed;

_Static_assert(sizeof(struct r88ee_rom) == R88EE_EFUSE_MAP_LEN,
    "R88EE_EFUSE_MAP_LEN must be equal to sizeof(struct r88ee_rom)!");

#endif	/* R88EE_ROM_IMAGE_H */
