/*	$OpenBSD: if_urtwn.c,v 1.16 2011/02/10 17:26:40 jakemsr Exp $	*/

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
__FBSDID("$FreeBSD: head/sys/dev/rtwn/rtl8188ee/r88ee_rom.c 307529 2016-10-17 20:38:24Z avos $");

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

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_media.h>

#include <net80211/ieee80211_var.h>
#include <net80211/ieee80211_radiotap.h>

#include <dev/rtwn/if_rtwnreg.h>
#include <dev/rtwn/if_rtwnvar.h>

#include <dev/rtwn/if_rtwn_debug.h>
#include <dev/rtwn/if_rtwn_efuse.h>

#include <dev/rtwn/rtl8188ee/r88ee.h>
#include <dev/rtwn/rtl8188ee/r88ee_priv.h>
#include <dev/rtwn/rtl8188ee/r88ee_var.h>
#include <dev/rtwn/rtl8188ee/r88ee_rom_image.h>


static void
r88ee_set_chains(struct rtwn_softc *sc)
{
#if 0
	struct r88ee_softc *rs = sc->sc_priv;

	if (rs->chip & R88EE_CHIP_92C) {
		sc->ntxchains = (rs->chip & R88EE_CHIP_92C_1T2R) ? 1 : 2;
		sc->nrxchains = 2;
	} else {
		sc->ntxchains = 1;
		sc->nrxchains = 1;
	}
#else
	printf("RTL8188EE:%s not implemented\n", __func__);
#endif
}

/*
 * This function does not differ from the rtl8192c version.
 *
 * Per Wikipedia, eFuse is an IBM technology that allows you to dynamically reprogram a chip while
 * it is in operation.
 *
 * This immediate function will set the next_rom_addr value, and simply call rtwn_efuse_read_next()
 * which is exactly how the rtl8192ce function does.
 *
 * The rtwn_efuse_read_next() will read the eFUSE address (R92C_EFUSE_CTRL, same as what would be
 * R88EE_EFUSE_CTRL based on the Linux driver), is saved to 'reg'.
 * I do not understand the next two macro modifications of 'reg', but that value is written to the eFUSE
 * value again.
 * The code will then try 100 times to verify that the write occurred. If unsuccessful, the code will
 * exit.
 * The value is MS()'d, again, a Macro I do not understand, and the value is saved to rs->pa_setttings.
 * The next_rom_addr value is incremented, and returned
 */

void
r88ee_efuse_postread(struct rtwn_softc *sc)
{
	struct r88ee_softc *rs = sc->sc_priv;

	/* XXX Weird but this is what the vendor driver does. */
	sc->next_rom_addr = 0x1fa;
	(void) rtwn_efuse_read_next(sc, &rs->pa_setting);
	RTWN_DPRINTF(sc, RTWN_DEBUG_ROM, "%s: PA setting=0x%x\n", __func__,
	    rs->pa_setting);
}

/*
 * Determine the board and regulatory types.
 * Set the names and chains. The chains are the number of actual RX/TX circuits that the card physically
 * has. Linux calls this rfpaths.
 *
 * Based on the Group Type, it will loop through 
 */


// Variables below from Linux, must be put in roms header
#define R88EE_GROUP_24G		6

void
r88ee_parse_rom(struct rtwn_softc *sc, uint8_t *buf)
{
#if 0
	struct r88ee_softc *rs = sc->sc_priv;
	struct rtwn_r88ee_txpwr *rt = rs->rs_txpwr;
	struct r88ee_rom *rom = (struct r88ee_rom *)buf;
	int i, j;

	rs->board_type = MS(rom->rf_opt1, R88EE_ROM_RF1_BOARD_TYPE);
	rs->regulatory = MS(rom->rf_opt1, R88EE_ROM_RF1_REGULATORY);
	RTWN_DPRINTF(sc, RTWN_DEBUG_ROM, "%s: regulatory type=%d\n",
	    __func__, rs->regulatory);

	/* Need to be set before postinit() (but after preinit()). */
	rtwn_r88ee_set_name(sc);
	r88ee_set_chains(sc);

	for (j = 0; j < R88EE_GROUP_2G; j++) {
		for (i = 0; i < sc->ntxchains; i++) {
			rt->cck_tx_pwr[i][j] = rom->cck_tx_pwr[i][j];
			rt->ht40_1s_tx_pwr[i][j] = rom->ht40_1s_tx_pwr[i][j];
		}

		rt->ht40_2s_tx_pwr_diff[0][j] =
		    MS(rom->ht40_2s_tx_pwr_diff[j], LOW_PART);
		rt->ht20_tx_pwr_diff[0][j] =
		    RTWN_SIGN4TO8(MS(rom->ht20_tx_pwr_diff[j],
			LOW_PART));
		rt->ofdm_tx_pwr_diff[0][j] =
		    MS(rom->ofdm_tx_pwr_diff[j], LOW_PART);
		rt->ht40_max_pwr[0][j] =
		    MS(rom->ht40_max_pwr[j], LOW_PART);
		rt->ht20_max_pwr[0][j] =
		    MS(rom->ht20_max_pwr[j], LOW_PART);

		if (sc->ntxchains > 1) {
			rt->ht40_2s_tx_pwr_diff[1][j] =
			    MS(rom->ht40_2s_tx_pwr_diff[j], HIGH_PART);
			rt->ht20_tx_pwr_diff[1][j] =
			    RTWN_SIGN4TO8(MS(rom->ht20_tx_pwr_diff[j],
				HIGH_PART));
			rt->ofdm_tx_pwr_diff[1][j] =
			    MS(rom->ofdm_tx_pwr_diff[j], HIGH_PART);
			rt->ht40_max_pwr[1][j] =
			    MS(rom->ht40_max_pwr[j], HIGH_PART);
			rt->ht20_max_pwr[1][j] =
			    MS(rom->ht20_max_pwr[j], HIGH_PART);
		}
	}

	sc->thermal_meter = MS(rom->thermal_meter, R88EE_ROM_THERMAL_METER);
	if (sc->thermal_meter == R88EE_ROM_THERMAL_METER_M)
		sc->thermal_meter = 0xff;
	IEEE80211_ADDR_COPY(sc->sc_ic.ic_macaddr, rom->macaddr);
#else
	struct r88ee_softc *rs = sc->sc_priv;
	struct rtwn_r88ee_txpwr *rt = rs->rs_txpwr;
	struct r88ee_rom *rom = (struct r88ee_rom *)buf;

	int i, j;
	int y; // Delete this

	// Code comes from: https://github.com/lwfinger/rtlwifi_new/blob/dfd58dae0d52f9d2fa6569be51dd739d8d4eafd4/rtl8188ee/hw.c#L1866
	rs->board_type = MS(rom->rf_opt1, R88EE_ROM_RF1_BOARD_TYPE);
	rs->regulatory = MS(rom->rf_opt1, R88EE_ROM_RF1_REGULATORY);


//	uint32_t offset = 0x10;		// Based on http://src.illumos.org/source/xref/linux-master/drivers/net/wireless/realtek/rtlwifi/rtl8188ee/hw.c#1588

	rtwn_r88ee_set_name(sc);	// This is not completed yet
	r88ee_set_chains(sc);		// This is not completed yet
//	uint16_t         *macaddr;

	uint8_t *x = (char *)buf; // delete me
	for (y=0;y<512;y++)
		printf("%d: %x \t %d\n", y, x[y], x[y]);

//	macaddr = buf+25;
	printf("ID value: Should be 0x8129: 0x%x\n", rom->id);
	printf("Mac address: should be 00:E0:4C: %X:%X:%X:%X:%X:%X:%X\n", rom->macaddr[0], rom->macaddr[1], rom->macaddr[2], rom->macaddr[3], rom->macaddr[4], rom->macaddr[5], rom->test);
	printf("Board type: %x\n", rs->board_type);
	printf("Regulatory: %x\n", rs->regulatory);
//	macaddr = (uint16_t *)&buf[22];
//	printf("Mac address: should be 00:E0:4C: %X:%X:%X:%X:%X:%X\n", macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);


	for (j = 0; j < 6 ; j++ ) { // MAX_RF_PATH on Linux, which is 6
		for (i = 0; i < 4 ; i++ ) { // MAX_CHNL_GROUP_24G on Linux, which is 4
			rt->cck_tx_pwr[i][j] = rom->cck_tx_pwr[i][j];
			printf("%d: cck_tx_twr[%d][%d] = %x\n", (i*14)+28+j, i, j, rom->cck_tx_pwr[i][j]);
		}
	}
//	for (j = 0; j < R88EE_GROUP_24G; j++) {
//		for (i = 0; i < sc->ntxchain; i++) {
//			rt->cck_tx_pwr[i][j] = rom->cck_tx_pwr[i][j];
//		}
//	}

	IEEE80211_ADDR_COPY(sc->sc_ic.ic_macaddr, rom->macaddr);

	//printf("RTL8188EE:%s not implemented\n", __func__);
#endif
}
