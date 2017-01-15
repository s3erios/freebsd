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

#include <dev/rtwn/rtl8188ee/r88ee.h>
#include <dev/rtwn/rtl8188ee/r88ee_reg.h>
#include <dev/rtwn/rtl8188ee/r88ee_var.h>


void
r88ee_detach_private(struct rtwn_softc *sc)
{
	struct r88ee_softc *rs = sc->sc_priv;

	free(rs, M_RTWN_PRIV);
}

/*
 * This function receives the value reg_sys_cfg. This is an unsigned
 * 32-bit integer read from the rtl card which indicates its version
 * information.
 * It reads the 23'd bit to determine whether its a TEST chip or
 * normal chip.
 * In the Linux driver, the read section is done by the driver itself
 * whereas this is abstracted out due to the similarities in rtl
 * drivers.
*/

void
r88ee_read_chipid_vendor(struct rtwn_softc *sc, uint32_t reg_sys_cfg)
{
	struct r88ee_softc *rs = sc->sc_priv;

	if (reg_sys_cfg & 0x800000) { // BIT(23)
		rs->chip = R88EE_CHIP_8723; // VERSION_TEST_CHIP_88E
	}
	else {
		rs->chip = R88EE_CHIP_92D;
	}

}



