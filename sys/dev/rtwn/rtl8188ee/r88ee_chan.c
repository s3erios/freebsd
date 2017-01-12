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

// From here: https://github.com/lwfinger/rtlwifi_new/blob/dfd58dae0d52f9d2fa6569be51dd739d8d4eafd4/rtl8188ee/hw.c#L2222
void
r88ee_set_chan(struct rtwn_softc *sc, struct ieee80211_channel *c)
{
	struct r92c_soft *rs = sc->sc_priv;
	u_int chan;

	chan = rtwn_chan2centieee(c);

	r99ee_set_txpower(sc, c);

/*	//rtlpriv->cfg->ops->set_hw_reg(hw, HW_VAR_SLOT_TIME, &mac->slot_time);
	rtl88ee_set_hw_reg(hw, HW_VAR_SLOT_TIME, val)
	rtl_write_byte(rtlpriv, REG_SLOT, val[0]);
				0x051B, &mac->slot_time[0]
	rtl_write_byte in wifi.h which calls
	pci_write8_async which is a pointer to pci.h which calls
*/
/******* USE BELOW, ITS THE CONCLUSION **********/
//	writeb( &mac->slot_time, pci_mem_start + addr) // This is Linux code 
	rtwn_pci_write_1(sc, &mac->slot_time, pci_mem_addr + addr) // This is FreeBSD

			       AC_MAX = 4; // Defined as 4 in 
	// Defined here: https://github.com/lwfinger/rtlwifi_new/blob/d1a04f5bd9281b9c5f0425b0c6a0d1b59f1cdcca/wifi.h#L107
	for (e_aci = 0; e_aci < AC_MAX; e_aci++) {
		rtlpriv->cfg->ops->set_hw_reg(hw, HW_VAR_AC_PARAM, &e_aci);
		// Below is the expansion of above
		rtlpriv->dm.current_turbo_edca = false;
		rtlpriv->dm.is_any_nonbepkts = false;
		rtlpriv->dm.is_cur_rdlstate = false;

		if (rtlpci->acm_method != EACMWAY2_SW)
			rtlpriv->cfg->ops->set_hw_reg(hw, HW_VAR_ACM_CTRL, &e_aci);
	}

	
}
