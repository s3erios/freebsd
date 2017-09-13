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

#define BIT(nr)     (1 << (nr))

enum pwrseq_delay_unit {
	PWRSEQ_DELAY_US,
	PWRSEQ_DELAY_MS,
};


struct wlan_pwr_cfg {
    uint16_t offset;
    uint8_t cut_msk;
    uint8_t fab_msk:4;
    uint8_t interface_msk:4;
    uint8_t base:4;
    uint8_t cmd:4;
    uint8_t msk;
    uint8_t value;
};

#define PWR_CUT_ALL_MSK     0xFF
#define PWR_FAB_ALL_MSK     (BIT(0)|BIT(1)|BIT(2)|BIT(3))
#define PWR_INTF_SDIO_MSK   BIT(0)
#define PWR_BASEADDR_SDIO   0x03
#define PWR_CMD_READ        0x00
#define PWR_CMD_WRITE       0x01
#define PWR_CMD_POLLING     0x02
#define PWR_CMD_DELAY       0x03
#define PWR_CMD_END         0x04
#define PWR_INTF_ALL_MSK    (BIT(0)|BIT(1)|BIT(2)|BIT(3))
#define PWR_BASEADDR_MAC    0x00
#define PWR_INTF_PCI_MSK    BIT(2)

#define RTL8188EE_TRANS_CARDDIS_TO_CARDEMU              \
    {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
    PWR_BASEADDR_SDIO, PWR_CMD_WRITE, BIT(0), 0         \
    /*Set SDIO suspend local register*/},               \
    {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
    PWR_BASEADDR_SDIO, PWR_CMD_POLLING, BIT(1), BIT(1)      \
    /*wait power state to suspend*/},               \
    {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
    PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(3)|BIT(4), 0       \
    /*0x04[12:11] = 2b'01enable WL suspend*/},


#define RTL8188EE_TRANS_CARDEMU_TO_ACT                  \
    {0x0006, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
    PWR_BASEADDR_MAC, PWR_CMD_POLLING, BIT(1), BIT(1)       \
    /* wait till 0x04[17] = 1    power ready*/},            \
    {0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
    PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(0)|BIT(1), 0       \
    /* 0x02[1:0] = 0    reset BB*/},                \
    {0x0026, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
    PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), BIT(7)         \
    /*0x24[23] = 2b'01 schmit trigger */},              \
    {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
    PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), 0          \
    /* 0x04[15] = 0 disable HWPDN (control by DRV)*/},      \
    {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
    PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4)|BIT(3), 0       \
    /*0x04[12:11] = 2b'00 disable WL suspend*/},            \
    {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
    PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(0), BIT(0)         \
    /*0x04[8] = 1 polling until return 0*/},            \
    {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
    PWR_BASEADDR_MAC, PWR_CMD_POLLING, BIT(0), 0            \
    /*wait till 0x04[8] = 0*/},                 \
    {0x0023, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
    PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), 0          \
    /*LDO normal mode*/},                       \
    {0x0074, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
    PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), BIT(4)         \
    /*SDIO Driving*/},

#define RTL8188EE_TRANS_END     \
    {0xFFFF, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,\
    0, PWR_CMD_END, 0, 0}

#define RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS    10
#define RTL8188EE_TRANS_CARDEMU_TO_PDN_STEPS    10
#define RTL8188EE_TRANS_END_STEPS               1

struct wlan_pwr_cfg rtl8188ee_card_enable_flow
        [RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS +
         RTL8188EE_TRANS_CARDEMU_TO_PDN_STEPS +
         RTL8188EE_TRANS_END_STEPS] = {
    RTL8188EE_TRANS_CARDDIS_TO_CARDEMU
    RTL8188EE_TRANS_CARDEMU_TO_ACT
    RTL8188EE_TRANS_END
};

#define RTL8188EE_NIC_ENABLE_FLOW rtl8188ee_card_enable_flow

bool pwrseqcmdparsing(struct rtwn_softc *sc, uint8_t cut_version,
	      uint8_t faversion, uint8_t interface_type,
	      struct wlan_pwr_cfg pwrcfgcmd[]);

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
	printf("AAAA RTL8188EE:%s:%s not fully implemented\n", __FILE__, __func__);
	/* Enable BB and RF. */
	rtwn_setbits_2(sc, R92C_SYS_FUNC_EN, 0,
	    R92C_SYS_FUNC_EN_BBRSTB | R92C_SYS_FUNC_EN_BB_GLB_RST |
	    R92C_SYS_FUNC_EN_DIO_RF);
	// Good, lines 263-265

			 // 0x01f, this seems good, 267
	rtwn_write_1(sc, R92C_RF_CTRL, 0x1 | 0x2 | 0x4);
			// line 268
	rtwn_write_1(sc, R92C_SYS_FUNC_EN, 0x80 | 0x40 | 0x20 | 0x2 | 0x1);
			// line 271-272
	rtwn_setbits_4(sc, R92C_LEDCFG0, 0, 0x00800000);

	r92c_init_bb_common(sc);
}

bool pwrseqcmdparsing(struct rtwn_softc *sc, uint8_t cut_version,
                              uint8_t faversion, uint8_t interface_type,
                              struct wlan_pwr_cfg pwrcfgcmd[]) {

    struct wlan_pwr_cfg cfg_cmd = {0};
    bool polling_bit = false;
    uint32_t ary_idx = 0;
    uint8_t value = 0;
    uint32_t offset = 0;
    uint32_t polling_count = 0;
    uint32_t max_polling_cnt = 5000;

    do {
        cfg_cmd = pwrcfgcmd[ary_idx];

        if ((cfg_cmd.fab_msk & faversion) &&
            (cfg_cmd.cut_msk & cut_version) &&
            (cfg_cmd.interface_msk & interface_type)) {
            switch(cfg_cmd.cmd) {
                case PWR_CMD_READ:
                    printf("Read\n");
                    break;

                case PWR_CMD_WRITE:
                    printf("Write\n");

                    offset = cfg_cmd.offset;
                    value = rtwn_read_1(sc, offset);
                    value &= (~(cfg_cmd.msk));
                    value |= cfg_cmd.value & cfg_cmd.msk;

                    rtwn_write_1(sc, offset, value);

                    break;
                case PWR_CMD_POLLING:
                    printf("Polling\n");
                    polling_bit = false;
                    offset = cfg_cmd.offset;
                    do {
                        value = rtwn_read_1(sc, offset);
                        value &= cfg_cmd.msk;
                        if (value == (cfg_cmd.value & cfg_cmd.msk))
                            polling_bit = true;
                        else {
				rtwn_delay(sc, 10);
                        }

                        if (polling_count++ > max_polling_cnt)
                            return false;
                    } while (!polling_bit);
                    break;
                case PWR_CMD_DELAY:
		    if(cfg_cmd.value == PWRSEQ_DELAY_US)
                        rtwn_delay(sc, cfg_cmd.offset * offset);
                    else
                        rtwn_delay(sc, cfg_cmd.offset * 1000);
                    break;
                case PWR_CMD_END:
                    printf("End\n");
                    return true;
                default:
                    printf("This should not happen\n");
                    break;
            }
        }
        ary_idx++;
    } while(1);

    return true;
}


int
r88ee_power_on(struct rtwn_softc *sc)
{
// I believe this is the code from _rtl88ee_init_mac()

#define RTWN_CHECK(res) do {    \
	if (res != 0)           \
		return (EIO);   \
} while(0)
//	int ntries;

/////// FreeBSD code
//	/* Wait for power ready bit. */
//	for (ntries = 0; ntries < 5000; ntries++) {
//		if (rtwn_read_4(sc, R92C_APS_FSMCO) & R92C_APS_FSMCO_SUS_HOST)
//			break;
//		rtwn_delay(sc, 10);
//	}
//	if (ntries == 5000) {
//		device_printf(sc->sc_dev,
//		    "timeout_waiting for chip power up\n");
//		return (ETIMEDOUT);
//	}
//
#define REG_XCK_OUT_CTRL	0x07c
#define REG_APS_FSMCO		0x0004
#define REG_RSV_CTRL		0x001C

	uint8_t bytetmp;

	bytetmp = rtwn_read_1(sc, REG_XCK_OUT_CTRL) & (~0x10);
	rtwn_write_1(sc, REG_XCK_OUT_CTRL, bytetmp);

	bytetmp = rtwn_read_1(sc, REG_APS_FSMCO + 1) & (~0x80);
	rtwn_write_1(sc, REG_APS_FSMCO + 1, bytetmp);

	rtwn_write_1(sc, REG_RSV_CTRL, 0x00);

	printf("Complex power on here\n");
	if (!pwrseqcmdparsing(sc, PWR_CUT_ALL_MSK,
		PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,
		RTL8188EE_NIC_ENABLE_FLOW)) {
		uprintf("pwrseqcmdparsing failed\n");
	}


#define REG_APS_FSMCO 0x0004

//    bytetmp = rtl_read_byte(rtlpriv, REG_APS_FSMCO) | BIT(4);
//    rtl_write_byte(rtlpriv, REG_APS_FSMCO, bytetmp);
	RTWN_CHECK(rtwn_setbits_1(sc, REG_APS_FSMCO, 0,
		0x10)); //BIT(4)));

#define REG_PCIE_CTRL_REG 0x0300

//    bytetmp = rtl_read_byte(rtlpriv, REG_PCIE_CTRL_REG+2);
//    rtl_write_byte(rtlpriv, REG_PCIE_CTRL_REG+2, bytetmp|BIT(2));
	RTWN_CHECK(rtwn_setbits_1(sc, REG_PCIE_CTRL_REG+2, 0,
		0x04)); //BIT(2)));

#define REG_WATCH_DOG 0x0368

//    bytetmp = rtl_read_byte(rtlpriv, REG_WATCH_DOG+1);
//    rtl_write_byte(rtlpriv, REG_WATCH_DOG+1, bytetmp|BIT(7));
	RTWN_CHECK(rtwn_setbits_1(sc, REG_WATCH_DOG+1, 0,
		0x80)); //BIT(7)));

#define REG_AFE_XTAL_CTRL_EXT 0x0078

//    bytetmp = rtl_read_byte(rtlpriv, REG_AFE_XTAL_CTRL_EXT+1);
//    rtl_write_byte(rtlpriv, REG_AFE_XTAL_CTRL_EXT+1, bytetmp|BIT(1));
	RTWN_CHECK(rtwn_setbits_1(sc, REG_AFE_XTAL_CTRL_EXT+1, 0,
		0x02)); //BIT(1)));

#define REG_TX_RPT_CTRL	0x04EC

//    bytetmp = rtl_read_byte(rtlpriv, REG_TX_RPT_CTRL);
//    rtl_write_byte(rtlpriv, REG_TX_RPT_CTRL, bytetmp|BIT(1)|BIT(0));
//    rtl_write_byte(rtlpriv, REG_TX_RPT_CTRL+1, 2);
//    rtl_write_word(rtlpriv, REG_TX_RPT_TIME, 0xcdf0);

#define REG_TX_RPT_CTRL 0x04EC
#define REG_TX_RPT_TIME 0x04F0
#define REG_SYS_CLKR	0x0008
#define REG_GPIO_MUXCFG	0x0040

	RTWN_CHECK(rtwn_setbits_1(sc, REG_TX_RPT_CTRL, 0, 0x02 | 0x01 ));
	rtwn_write_1(sc, REG_TX_RPT_CTRL+1, 2);
	rtwn_write_2(sc, REG_TX_RPT_TIME, 0xcdf0);

	RTWN_CHECK(rtwn_setbits_1(sc, REG_SYS_CLKR, 0, 0x08)); //BIT(3))); // hw.c 882

	RTWN_CHECK(rtwn_setbits_1(sc, REG_GPIO_MUXCFG+1, 0,
		~(0x10))); //BIT(4) ));

	rtwn_write_1(sc, 0x367, 0x80);

#define REG_CR	0x0100
#define MSR	(REG_CR + 2)

	rtwn_write_2(sc, REG_CR, 0x2ff);
	rtwn_write_1(sc, REG_CR+1, 0x06);
	rtwn_write_1(sc, MSR, 0x00);

	// Linux driver Does the REG_HISR 0xfffffff crap here	
	// This seems to take place in init_intr() code

#define R88E_MCUTST_1	0x01c0

	rtwn_write_4(sc, R92C_INT_MIG, 0); // same from _rtl88ee_init_mac
					   // same value of INT_MIG
	rtwn_write_4(sc, R88E_MCUTST_1, 0x0);
	rtwn_write_1(sc, R92C_PCIE_CTRL_REG+1, 0);

	return(0);
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
