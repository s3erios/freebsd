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
 * $FreeBSD: head/sys/dev/rtwn/rtl8188ee/r88ee_priv.h 307529 2016-10-17 20:38:24Z avos $
 */

#ifndef R88EE_PRIV_H
#define R88EE_PRIV_H

#include <dev/rtwn/rtl8188ee/r88ee_rom_defs.h>

/*
 * Parsed Tx power (diff) values.
 */
struct rtwn_r88ee_txpwr {
	uint8_t		cck_tx_pwr[R88EE_MAX_CHAINS][R88EE_GROUP_2G];
	uint8_t		ht40_1s_tx_pwr[R88EE_MAX_CHAINS][R88EE_GROUP_2G];
	int8_t		ht40_2s_tx_pwr_diff[R88EE_MAX_CHAINS][R88EE_GROUP_2G];
	int8_t		ht20_tx_pwr_diff[R88EE_MAX_CHAINS][R88EE_GROUP_2G];
	int8_t		ofdm_tx_pwr_diff[R88EE_MAX_CHAINS][R88EE_GROUP_2G];
	int8_t		ht40_max_pwr[R88EE_MAX_CHAINS][R88EE_GROUP_2G];
	int8_t		ht20_max_pwr[R88EE_MAX_CHAINS][R88EE_GROUP_2G];

};


/*
 * Baseband initialization values (shared parts).
 */
#define R88EE_COND_RTL8188CE	0x01
#define R88EE_COND_RTL8188CU	0x02
#define R88EE_COND_RTL8188RU	0x04
#define R88EE_COND_RTL8188EEE	0x08
#define R88EE_COND_RTL8188EEU	0x10

/* Shortcut. */
#define R88EE_COND_RTL8188EE	(R88EE_COND_RTL8188EEE | R88EE_COND_RTL8188EEU)

static const uint16_t rtl8188ee_bb_regs3[] = {
};

static const uint32_t rtl8188ee_bb_vals3_88cu_88ru[] = {
};

/*
 * RTL8188EEU and RTL8188EEE-VAU.
 */

static const uint32_t rtl8188ee_agc_vals[] = {
};

static const struct rtwn_agc_prog rtl8188ee_agc[] = {
};


/*
 * RF initialization values.
 */
static const uint8_t rtl8188ee_rf0_regs0[] = {
};

static const uint32_t rtl8188ee_rf0_vals0_88ce_88cu_92ce[] = {
};

static const struct rtwn_rf_prog rtl8188ee_rf[] = {
};


struct rtwn_r88ee_txagc {
};

/*
 * Per RF chain/group/rate Tx gain values.
 */
static const struct rtwn_r88ee_txagc rtl8188eeu_txagc[] = {
};

static const struct rtwn_r88ee_txagc rtl8188ru_txagc[] = {
};

#endif	/* R88EE_PRIV_H */
