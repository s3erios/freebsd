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
__FBSDID("$FreeBSD$");

// stuff to remove
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

// my legit stuff
#include <dev/rtwn/if_rtwnvar.h>
#include <dev/rtwn/rtl8188e/pci/r88ee_pwr.h>

struct wlan_pwr_cfg rtl8188ee_power_on_flow[RTL8188EE_TRANS_CARDEMU_TO_ACT_STEPS
					+ RTL8188EE_TRANS_END_STEPS] = {
	RTL8188EE_TRANS_CARDEMU_TO_ACT
	RTL8188EE_TRANS_END
};

/*3Radio off GPIO Array */
struct wlan_pwr_cfg rtl8188ee_radio_off_flow[RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS
					+ RTL8188EE_TRANS_END_STEPS] = {
	RTL8188EE_TRANS_ACT_TO_CARDEMU
	RTL8188EE_TRANS_END
};

/*3Card Disable Array*/
struct wlan_pwr_cfg rtl8188ee_card_disable_flow
		[RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS +
		 RTL8188EE_TRANS_CARDEMU_TO_PDN_STEPS +
		 RTL8188EE_TRANS_END_STEPS] = {
	RTL8188EE_TRANS_ACT_TO_CARDEMU
	RTL8188EE_TRANS_CARDEMU_TO_CARDDIS
	RTL8188EE_TRANS_END
};

/*3 Card Enable Array*/
struct wlan_pwr_cfg rtl8188ee_card_enable_flow
		[RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS +
		 RTL8188EE_TRANS_CARDEMU_TO_PDN_STEPS +
		 RTL8188EE_TRANS_END_STEPS] = {
	RTL8188EE_TRANS_CARDDIS_TO_CARDEMU
	RTL8188EE_TRANS_CARDEMU_TO_ACT
	RTL8188EE_TRANS_END
};

/*3Suspend Array*/
struct wlan_pwr_cfg rtl8188ee_suspend_flow[RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS
					+ RTL8188EE_TRANS_CARDEMU_TO_SUS_STEPS
					+ RTL8188EE_TRANS_END_STEPS] = {
	RTL8188EE_TRANS_ACT_TO_CARDEMU
	RTL8188EE_TRANS_CARDEMU_TO_SUS
	RTL8188EE_TRANS_END
};

/*3 Resume Array*/
struct wlan_pwr_cfg rtl8188ee_resume_flow[RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS
					+ RTL8188EE_TRANS_CARDEMU_TO_SUS_STEPS
					+ RTL8188EE_TRANS_END_STEPS] = {
	RTL8188EE_TRANS_SUS_TO_CARDEMU
	RTL8188EE_TRANS_CARDEMU_TO_ACT
	RTL8188EE_TRANS_END
};

/*3HWPDN Array*/
struct wlan_pwr_cfg rtl8188ee_hwpdn_flow[RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS
				+ RTL8188EE_TRANS_CARDEMU_TO_PDN_STEPS
				+ RTL8188EE_TRANS_END_STEPS] = {
	RTL8188EE_TRANS_ACT_TO_CARDEMU
	RTL8188EE_TRANS_CARDEMU_TO_PDN
	RTL8188EE_TRANS_END
};

/*3 Enter LPS */
struct wlan_pwr_cfg rtl8188ee_enter_lps_flow[RTL8188EE_TRANS_ACT_TO_LPS_STEPS
					+ RTL8188EE_TRANS_END_STEPS] = {
	/*FW behavior*/
	RTL8188EE_TRANS_ACT_TO_LPS
	RTL8188EE_TRANS_END
};

/*3 Leave LPS */
struct wlan_pwr_cfg rtl8188ee_leave_lps_flow[RTL8188EE_TRANS_LPS_TO_ACT_STEPS
					+ RTL8188EE_TRANS_END_STEPS] = {
	/*FW behavior*/
	RTL8188EE_TRANS_LPS_TO_ACT
	RTL8188EE_TRANS_END
};

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
                            printf("udelay(10)\n");
                            polling_bit = true; // This is not correct
                        }

                        if (polling_count++ > max_polling_cnt)
                            return false;
                    } while (!polling_bit);
                    break;
                case PWR_CMD_DELAY:
                    printf("Delay\n");
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
