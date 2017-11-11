/*-
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
 * $FreeBSD$
 */

#ifndef R88E_ROM_IMAGE_H
#define R88E_ROM_IMAGE_H

#include <dev/rtwn/rtl8188e/r88e_rom_defs.h>

/*
 * RTL8188EU ROM image.
 */
struct r88e_rom {
	uint8_t		reserved1[16];
	uint8_t		cck_tx_pwr[R88E_GROUP_2G];
	uint8_t		ht40_tx_pwr[R88E_GROUP_2G - 1];
	uint8_t		tx_pwr_diff;
	uint8_t		reserved2[156];
	uint8_t		channel_plan;
	uint8_t		crystalcap;
#define R88E_ROM_CRYSTALCAP_DEF		0x20

	uint8_t		thermal_meter;
	uint8_t		reserved3[6];
	uint8_t		rf_board_opt;
	uint8_t		rf_feature_opt;
	uint8_t		rf_bt_opt;
	uint8_t		version;
	uint8_t		customer_id;
	uint8_t		reserved4[3];
	uint8_t		rf_ant_opt;
	uint8_t		reserved5[6];
	uint16_t	vid;
	uint16_t	pid;
	uint8_t		usb_opt;
	uint8_t		reserved6[2];
	uint8_t		macaddr[IEEE80211_ADDR_LEN];
	uint8_t		reserved7[2];
	uint8_t		string[33];	/* "realtek 802.11n NIC" */
	uint8_t		reserved8[256];
} __packed;

_Static_assert(sizeof(struct r88e_rom) == R88E_EFUSE_MAP_LEN,
    "R88E_EFUSE_MAP_LEN must be equal to sizeof(struct r88e_rom)!");

/*
 * RTL8188EE ROM image.
 * Largely derived from this:
 * http://src.illumos.org/source/xref/linux-master/drivers/net/wireless/realtek/rtlwifi/rtl8188ee/hw.c#1576
 */

struct _bw40_bw20_ofdm_cck {
	uint8_t				bw40_bw20; // Bitshift 07,08 
	uint8_t				ofdm_cck; // Bitshift 09,10 
};

struct _r88ee_rom_24g {
	uint8_t				index_cck_base[6];//R88EE_GROUP_24G]; // 01 - This value is 6
	uint8_t				index_bw40_base[6-1];//R88EE_GROUP_24G-1]; // 02 - Number is 5
						   // 03 Is not captured anywhere.
	uint8_t				bw20_ofdm; // 04,05 - bitshift_1;
						   // 06 is also not captured
	struct _bw40_bw20_ofdm_cck	bw40_bw20_ofdm_cck[4-1];//R88EE_MAX_CHAINS-1]; // 04 This value is 4
};

struct _r88ee_rom_5g {
	uint8_t				index_bw40_base[14];//R88EE_GROUP_5G]; // 11,12,13 This value is 14
	uint8_t				bw20_ofdm;	// 14,15
	uint8_t				bw40_bw20[4-1];//R88EE_MAX_TX_COUNT-1]; // 16,17 This value is 4-1=3
	uint8_t				ofdm_1[2]; // 18,19 and then 20
//	uint8_t				ofdm_2[MAX_TX_COUNT-1]; // Value is 4-1=3
};

struct _r88ee_rf_path {
	struct _r88ee_rom_24g		rfpath_24g;
	struct _r88ee_rom_5g		rfpath_5g;
};

struct r88ee_rom {
	uint16_t			id; /* Always 0x8129 */

	uint8_t				hpon[4];
	uint16_t			clk;
	uint8_t				testr[8];

	struct _r88ee_rf_path		rfpath[4]; //R88EE [MAX_RF_PATH]; // MAX_RF_PATH is 4 [16-168]

	uint8_t				unknown3[16];
	uint16_t				channel_plan;
	uint8_t				unknown4[7];
	uint8_t				rf_board_option;
	uint8_t				rf_feature_option;
	uint8_t				rf_bt_setting;
	uint8_t				version;
	uint8_t				customer_id;
	uint8_t				reserved1[3];
	uint8_t				rf_antenna_option;

	uint8_t				reserved2[6];
	uint8_t				macaddr[IEEE80211_ADDR_LEN];
	uint16_t			vid;
	uint16_t			did;
	uint16_t			svid;
	uint16_t			smid;

	uint8_t				unknown5[290];
}; // Should be 512 byte

_Static_assert(sizeof(struct r88ee_rom) == R88E_EFUSE_MAP_LEN,
    "R88EE_EFUSE_MAP_LEN must be equal to sizeof(struct r88ee_rom)!");

#endif	/* R88E_ROM_IMAGE_H */
