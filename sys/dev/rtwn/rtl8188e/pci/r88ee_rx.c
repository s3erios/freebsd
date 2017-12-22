
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

#include <machine/bus.h>
#include <machine/resource.h>
#include <sys/rman.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_media.h>

#include <net80211/ieee80211_var.h>
#include <net80211/ieee80211_radiotap.h>

#include <dev/rtwn/if_rtwnvar.h>
#include <dev/rtwn/if_rtwn_debug.h>

#include <dev/rtwn/rtl8188e/r88e.h>
#include <dev/rtwn/rtl8188e/r88e_reg.h>
#include <dev/rtwn/pci/rtwn_pci_var.h>

#include <dev/rtwn/rtl8188e/pci/r88ee.h>
#include <dev/rtwn/rtl8192c/pci/r92ce_reg.h>

int
r88ee_classify_intr(struct rtwn_softc *sc, void *arg, int len __unused)
{
   uint32_t status;
	uint32_t statusb;
   int *rings = arg;
   int ret;

   *rings = 0;

//  status = rtwn_read_4(sc, 0xb4);

//   RTWN_DPRINTF(sc, RTWN_DEBUG_INTR, "%s: HISR %08X, HISRE %04X\n",
//       __func__, status, rtwn_read_2(sc, R92C_HISRE));
//   if (status == 0 || status == 0xffffffff)
//      return (0);

   /* Disable interrupts. */
//   rtwn_write_4(sc, 0xb0, 0x00); // This is an RTL8188EE thing, not part of the base
#define REG_HIMR 0xb0
#define REG_HIMRE 0xb8
#define IMR_DISABLED 0x0
	rtwn_write_4(sc, REG_HIMR, 	IMR_DISABLED); // Comes from rtl88ee_disable_interrupt
	rtwn_write_4(sc, REG_HIMRE, 	IMR_DISABLED);

#define ISR_MINE 0xb4
	status = rtwn_read_4(sc, ISR_MINE) & 0x200084ff;
	rtwn_write_4(sc, ISR_MINE, status);
//printf("inta: %d\n", status);
#define REG_HISRE_MINE 0xbc
	statusb = rtwn_read_4(sc, REG_HISRE_MINE) & 0x100;
	rtwn_write_4(sc, REG_HISRE_MINE, statusb);
//printf("intb: %d\n", statusb);

	printf("Statusb: %x\n", statusb);
	if (!status || statusb == 0xffff) {
		rtwn_write_4(sc, 0xb0, 0x200084ff);
		rtwn_write_4(sc, 0xb8, 0x100);
		return 0;
	}


   /* Ack interrupts. */
//   rtwn_write_4(sc, 0xb4, status);

   if (statusb & R92C_IMR_BDOK)
      *rings |= (1 << RTWN_PCI_BEACON_QUEUE);
   if (statusb & R92C_IMR_HIGHDOK)
      *rings |= (1 << RTWN_PCI_HIGH_QUEUE);
   if (statusb & R92C_IMR_MGNTDOK)
      *rings |= (1 << RTWN_PCI_MGNT_QUEUE);
   if (statusb & R92C_IMR_BKDOK)
      *rings |= (1 << RTWN_PCI_BK_QUEUE);
   if (statusb & R92C_IMR_BEDOK)
      *rings |= (1 << RTWN_PCI_BE_QUEUE);
   if (statusb & R92C_IMR_VIDOK)
      *rings |= (1 << RTWN_PCI_VI_QUEUE);
   if (statusb & R92C_IMR_VODOK)
      *rings |= (1 << RTWN_PCI_VO_QUEUE);


	printf("rings: %x\n", *rings);

   ret = 0;
   if (status & R92C_IMR_RXFOVW)
      ret |= RTWN_PCI_INTR_RX_OVERFLOW;
   if (status & R92C_IMR_RDU)
      ret |= RTWN_PCI_INTR_RX_DESC_UNAVAIL;
   if (status & R92C_IMR_ROK)
      ret |= RTWN_PCI_INTR_RX_DONE;
   if (status & R92C_IMR_TXFOVW)
      ret |= RTWN_PCI_INTR_TX_OVERFLOW;
   if (status & R92C_IMR_PSTIMEOUT)
      ret |= RTWN_PCI_INTR_PS_TIMEOUT;

   return (ret);
}

void
r88ee_enable_intr(struct rtwn_pci_softc *pc)
{
	struct rtwn_softc *sc = &pc->pc_sc;

	/* Enable interrupts */
#define R88EE_HIMR	0xb0
#define R88EE_HIMRE	0xb8
	rtwn_write_4(sc, R88EE_HIMR, 0x200084ff); //R88E_INT_ENABLE);
	rtwn_write_4(sc, R88EE_HIMRE, 0x100);


        /* there are some C2H CMDs have been sent
         * before system interrupt is enabled, e.g., C2H, CPWM.
         * So we need to clear all C2H events that FW has notified,
         * otherwise FW won't schedule any commands anymore.
         */
//        rtl_write_byte(rtlpriv, REG_C2HEVT_CLEAR, 0);
	rtwn_write_1(sc, 0x01AF, 0);
        /*enable system interrupt*/
//        rtl_write_dword(rtlpriv, REG_HSIMR,
//                        rtlpci->sys_irq_mask & 0xFFFFFFFF);
	rtwn_write_4(sc, 0x0058, 0xFFFFFFFF && 0xc0 );


}

void
r88ee_start_xfers(struct rtwn_softc *sc)
{
	printf("RTL8188EE:%s:%s\n", __FILE__, __func__);
	/* Clear pending interrupts */
#define R88EE_HIMR   0xb0
#define R88EE_HIMRE  0xb8
	rtwn_write_4(sc, R88EE_HIMR, 0xffffffff);	

	/* Enable interrupts */
//	rtwn_write_4(sc, R88EE_HIMRE, 0xffffffff);

   /* Enable interrupts */
   rtwn_write_4(sc, R88EE_HIMR, 0x200084ff); //R88E_INT_ENABLE);
   rtwn_write_4(sc, R88EE_HIMRE, 0x100);


}

#undef R88EE_INT_ENABLE
