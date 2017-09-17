/*	$OpenBSD: if_urtwn.c,v 1.16 2011/02/10 17:26:40 jakemsr Exp $	*/

/*-
 * Copyright (c) 2010 Damien Bergamini <damien.bergamini@free.fr>
 * Copyright (c) 2014 Kevin Lo <kevlo@FreeBSD.org>
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

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_media.h>

#include <net80211/ieee80211_var.h>
#include <net80211/ieee80211_radiotap.h>

#include <dev/rtwn/if_rtwnreg.h>
#include <dev/rtwn/if_rtwnvar.h>

#include <dev/rtwn/if_rtwn_debug.h>

#include <dev/rtwn/rtl8192c/r92c.h>
#include <dev/rtwn/rtl8192c/r92c_var.h>

#include <dev/rtwn/rtl8188e/r88e.h>
#include <dev/rtwn/rtl8188e/r88e_reg.h>
#include <dev/rtwn/rtl8192c/pci/r92ce_reg.h> // Added due to R92C_INT_MIG

static void
r88e_crystalcap_write(struct rtwn_softc *sc)
{
	struct r92c_softc *rs = sc->sc_priv;
	uint32_t reg;
	uint8_t val;

	val = rs->crystalcap & 0x3f;
	reg = rtwn_bb_read(sc, R92C_AFE_XTAL_CTRL);
	rtwn_bb_write(sc, R92C_AFE_XTAL_CTRL,
	    RW(reg, R92C_AFE_XTAL_CTRL_ADDR, val | val << 6));
}

void
r88e_init_bb(struct rtwn_softc *sc)
{

	/* Enable BB and RF. */
	rtwn_setbits_2(sc, R92C_SYS_FUNC_EN, 0,
	    R92C_SYS_FUNC_EN_BBRSTB | R92C_SYS_FUNC_EN_BB_GLB_RST |
	    R92C_SYS_FUNC_EN_DIO_RF);

	rtwn_write_1(sc, R92C_RF_CTRL,
	    R92C_RF_CTRL_EN | R92C_RF_CTRL_RSTB | R92C_RF_CTRL_SDMRSTB);
	rtwn_write_1(sc, R92C_SYS_FUNC_EN,
	    R92C_SYS_FUNC_EN_USBA | R92C_SYS_FUNC_EN_USBD |
	    R92C_SYS_FUNC_EN_BB_GLB_RST | R92C_SYS_FUNC_EN_BBRSTB);

	r92c_init_bb_common(sc);

	rtwn_bb_write(sc, R92C_OFDM0_AGCCORE1(0), 0x69553422);
	rtwn_delay(sc, 1);
	rtwn_bb_write(sc, R92C_OFDM0_AGCCORE1(0), 0x69553420);
	rtwn_delay(sc, 1);

	r88e_crystalcap_write(sc);
}

void
r88ee_init_bb(struct rtwn_softc *sc)
{
// This code should mirror Linux's rtl8188ee/phy.c rtl88e_phy_bb_config()
	bool rtstatus;
	uint16_t regval;
	uint32_t tmp;
	int i, j;

	// I have no idea what this does
	//_rtl88e_phy_init_bb_rf_register_definition(hw);
#define REG_SYS_FUNC_EN		0x002
#define REG_RF_CTRL		0x001f
#define RF_EN			0x01
#define RF_RSTB			0x02
#define RF_SDMRSTB		0x04
#define FEN_PCIEA		0x40
#define FEN_PPLL		0x80
#define FEN_BB_GLB_RSTN		0x02
#define FEN_BBRSTB		0x01
#define BIT(nr)			(1 << (nr))
#define FEN_DIO_PCIE		BIT(5)

	regval = rtwn_read_2(sc, REG_SYS_FUNC_EN);
	rtwn_write_2(sc, REG_SYS_FUNC_EN, regval | BIT(13) | BIT(0) | BIT(1));

	rtwn_write_1(sc, REG_RF_CTRL, RF_EN | RF_RSTB | RF_SDMRSTB);
	rtwn_write_1(sc, REG_SYS_FUNC_EN, FEN_PPLL | FEN_PCIEA | FEN_DIO_PCIE |
					  FEN_BB_GLB_RSTN | FEN_BBRSTB);
	tmp = rtwn_read_4(sc, 0x4c);
	rtwn_write_4(sc, 0x4c, tmp | BIT(23));
//	rtstatus = _rtl88e_phy_bb8188e_config_parafile(sc);
	// comes from rtl8188ee/phy.c : 367

///////////////////////////////////////////////////
//	handle_branch1(sc, RTL8188EEPHY_REG_1TARRAYLEN, RTL8188EEPHY_REG_1TARRAY); 

	// I do not know what autoload_failflag does.

//	handle_branch2(sc, RTL8188EEAGCTAB_1TARRAYLEN, RTL8188EEAGCTAB_1TARRAY);
///////////////////////////////////////////////////

	// Aftert his is rtl8188ee/phy.c 367
	// Below this should mirror rtl8192c/r92c_init r92c_init_bb_common
	// But should send new RTL8188EE BB and AGC tables
	for (i = 0; i < sc->bb_size; i++) {
		const struct rtwn_bb_prog *bb_prog = &sc->bb_prog[i];

		while(!rtwn_check_condition(sc, bb_prog->cond)) {
			KASSERT(bb_prog->next != NULL,
			    ("%s: wrong condition value (i %d)\n",
			    __func__, i));
			bb_prog = bb_prog->next;
		}

		for (j = 0; j < bb_prog->count; j++) {
			RTWN_DPRINTF(sc, RTWN_DEBUG_RESET,
			    "BB: reg 0x%03x, val 0x%08x\n",
			    bb_prog->reg[j], bb_prog->val[j]);

			rtwn_bb_write(sc, bb_prog->reg[j], bb_prog->val[j]);
			rtwn_delay(sc, 1);
		}
	}
	// Some Chipset stuff, not specific to R92C_CHIP_02C_LT2R)

	for (i = 0; i < sc->agc_size; i++) {
		const struct rtwn_agc_prog *agc_prog = &sc->agc_prog[i];

		while(!rtwn_check_condition(sc, agc_prog->cond)) {
			KASSERT(agc_prog->next != NULL,
			    ("%s: wrong condition value (2) (i %d)\n",
			    __func__, i));
			agc_prog = agc_prog->next;
		}

		for(j = 0; j < agc_prog->count; j++) {
			RTWN_DPRINTF(sc, RTWN_DEBUG_RESET,
			    "AGC: val 0x%08x\n", agc_prog->val[j]);

			rtwn_bb_write(sc, R92C_OFDM0_AGCRSSITABLE,
			    agc_prog->val[j]);
			rtwn_delay(sc, 1);
		}
	}

	if (rtwn_bb_read(sc, R92C_HSSI_PARAM2(0)) & R92C_HSSI_PARAM2_CCK_HIPWR)
		sc->sc_flags |= RTWN_FLAG_CCK_HIPWR;

	printf("Figure out what rtl_get_bbreg does, it happens here\n");

	// This is the same as rtl_get_bbreg()

//	rtl_get_bbreg(sc, R92C_HSSI_PARAM(0), 0x200);
	uint32_t returnvalue, originalvalue, bitshift;
	uint32_t q;

	uint32_t bitmask;

		// The second argument is R92C_HSSI_PARAM(0) aka RFPGA0_XA_HSSIPARAMETER2
	originalvalue = rtwn_read_4(sc, 0x824);

	// _rtl88e_phy_calculate_bit_shift(mask) which is 0x200
	bitmask = 0x200;
	for(q=0;q<=32;q++) {
		if (((bitmask >> q) & 0x1) == 1)
			break;
	}
	bitshift = q;
	returnvalue = (originalvalue & bitmask) >> bitshift;

	printf("The return value is %x\n", returnvalue);
///////////

	rtstatus = true;


	printf("AAAA RTL8188EE:%s:%s not fully implemented\n", __FILE__, __func__);
}

int
r88e_power_on(struct rtwn_softc *sc)
{
#define RTWN_CHK(res) do {	\
	if (res != 0)		\
		return (EIO);	\
} while(0)
	int ntries;

	/* Wait for power ready bit. */
	for (ntries = 0; ntries < 5000; ntries++) {
		if (rtwn_read_4(sc, R92C_APS_FSMCO) & R92C_APS_FSMCO_SUS_HOST)
			break;
		rtwn_delay(sc, 10);
	}
	if (ntries == 5000) {
		device_printf(sc->sc_dev,
		    "timeout waiting for chip power up\n");
		return (ETIMEDOUT);
	}

	/* Reset BB. */
	RTWN_CHK(rtwn_setbits_1(sc, R92C_SYS_FUNC_EN,
	    R92C_SYS_FUNC_EN_BBRSTB | R92C_SYS_FUNC_EN_BB_GLB_RST, 0));

	RTWN_CHK(rtwn_setbits_1(sc, R92C_AFE_XTAL_CTRL + 2, 0, 0x80));

	/* Disable HWPDN. */
	RTWN_CHK(rtwn_setbits_1_shift(sc, R92C_APS_FSMCO,
	    R92C_APS_FSMCO_APDM_HPDN, 0, 1));

	/* Disable WL suspend. */
	RTWN_CHK(rtwn_setbits_1_shift(sc, R92C_APS_FSMCO,
	    R92C_APS_FSMCO_AFSM_HSUS | R92C_APS_FSMCO_AFSM_PCIE, 0, 1));

	RTWN_CHK(rtwn_setbits_1_shift(sc, R92C_APS_FSMCO,
	    0, R92C_APS_FSMCO_APFM_ONMAC, 1));
	for (ntries = 0; ntries < 5000; ntries++) {
		if (!(rtwn_read_2(sc, R92C_APS_FSMCO) &
		    R92C_APS_FSMCO_APFM_ONMAC))
			break;
		rtwn_delay(sc, 10);
	}
	if (ntries == 5000)
		return (ETIMEDOUT);

	/* Enable LDO normal mode. */
	RTWN_CHK(rtwn_setbits_1(sc, R92C_LPLDO_CTRL,
	    R92C_LPLDO_CTRL_SLEEP, 0));

	/* Enable MAC DMA/WMAC/SCHEDULE/SEC blocks. */
	RTWN_CHK(rtwn_write_2(sc, R92C_CR, 0));
	RTWN_CHK(rtwn_setbits_2(sc, R92C_CR, 0,
	    R92C_CR_HCI_TXDMA_EN | R92C_CR_TXDMA_EN |
	    R92C_CR_HCI_RXDMA_EN | R92C_CR_RXDMA_EN |
	    R92C_CR_PROTOCOL_EN | R92C_CR_SCHEDULE_EN |
	    ((sc->sc_hwcrypto != RTWN_CRYPTO_SW) ? R92C_CR_ENSEC : 0) |
	    R92C_CR_CALTMR_EN));

	return (0);
#undef RTWN_CHK
}
