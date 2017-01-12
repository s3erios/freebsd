/*
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


#include <dev/rtwn/rtl8192c/pci/r88ee_reg.h>
	
void
r88ee_start_xfers(struct rtwn_softc *sc)
{
	/* rtl88ee_enable_interrupt
	 * rtl88ee_disable_interrupt
	 */
	rtwn_write_4(sc, R88EE_HIMR, 0xffffffff);
	rtwn_write_4(sc, R88EE_HIMRE, 0xffffffff);
	// Set the IRQ_enable to true
	
	// From https://github.com/lwfinger/rtlwifi_new/blob/b6a14442fea8e059662a52534692a50c63634c4e/rtl8188ee/reg.h#L122
	// 0x01AF
	rtwn_write_BYTEEEEE(sc, REG_C2HEVT_CLEAR, 0);
	// 0x0058
	rtwn_write_4(sc, REG_C2HEVT_CLEAR, 0xFFFFFFFF);
}
