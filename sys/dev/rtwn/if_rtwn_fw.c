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
#include <sys/firmware.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_media.h>

#include <net80211/ieee80211_var.h>
#include <net80211/ieee80211_radiotap.h>

#include <dev/rtwn/if_rtwnreg.h>
#include <dev/rtwn/if_rtwnvar.h>

#include <dev/rtwn/if_rtwn_debug.h>
#include <dev/rtwn/if_rtwn_fw.h>

#include <dev/rtwn/rtl8192c/r92c_reg.h>


#ifndef RTWN_WITHOUT_UCODE
static int
rtwn_fw_loadpage(struct rtwn_softc *sc, int page, const uint8_t *buf,
    int len)
{
	uint32_t reg;
	uint16_t off;
	int mlen, error;

	printf("1\n");
	reg = rtwn_read_4(sc, R92C_MCUFWDL);
	printf("2\n");
	reg = RW(reg, R92C_MCUFWDL_PAGE, page);
	printf("3\n");
	rtwn_write_4(sc, R92C_MCUFWDL, reg);
	printf("4\n");

	error = 0;
	off = R92C_FW_START_ADDR;
	printf("5\n");
	while (len > 0) {
		if (len > R92C_FW_MAX_BLOCK_SIZE)
			mlen = R92C_FW_MAX_BLOCK_SIZE;
		else if (len > 4)
			mlen = 4;
		else
			mlen = 1;
		error = rtwn_fw_write_block(sc, buf, off, mlen);
		if (error != 0)
			break;
		off += mlen;
		buf += mlen;
		len -= mlen;
		printf("6\n");
	}

	if (error != 0) {
		RTWN_DPRINTF(sc, RTWN_DEBUG_FIRMWARE,
		    "%s: could not load firmware page %d (offset %u)\n",
		    __func__, page, off);
	}

	return (error);
}

static int
rtwn_fw_checksum_report(struct rtwn_softc *sc)
{
	int ntries;

	for (ntries = 0; ntries < 25; ntries++) {
		if (rtwn_read_4(sc, R92C_MCUFWDL) & R92C_MCUFWDL_CHKSUM_RPT)
			break;
		rtwn_delay(sc, 10000);
	}
	if (ntries == 25) {
		RTWN_DPRINTF(sc, RTWN_DEBUG_FIRMWARE,
		    "timeout waiting for checksum report\n");
		return (ETIMEDOUT);
	}

	return (0);
}

int
rtwn_load_firmware(struct rtwn_softc *sc)
{
	const struct firmware *fw;
	const struct r92c_fw_hdr *hdr;
	const u_char *ptr;
	size_t len;
	int ntries, error;

	/* Read firmware image from the filesystem. */
	RTWN_UNLOCK(sc);
	fw = firmware_get(sc->fwname);
	RTWN_LOCK(sc);
	if (fw == NULL) {
		device_printf(sc->sc_dev,
		    "failed loadfirmware of file %s\n", sc->fwname);
		return (ENOENT);
	}

	len = fw->datasize;
	if (len < sizeof(*hdr) || len > sc->fwsize_limit) {
		device_printf(sc->sc_dev, "wrong firmware size (%zu)\n", len);
		error = EINVAL;
		goto fail;
	}
	ptr = fw->data;
	hdr = (const struct r92c_fw_hdr *)ptr;
	/* Check if there is a valid FW header and skip it. */
	if ((le16toh(hdr->signature) >> 4) == sc->fwsig) {
		printf("There was a valid firmware header\n");
		sc->fwver = le16toh(hdr->version);

		RTWN_DPRINTF(sc, RTWN_DEBUG_FIRMWARE,
		    "FW V%u.%u %02u-%02u %02u:%02u\n",
		    le16toh(hdr->version), le16toh(hdr->subversion),
		    hdr->month, hdr->date, hdr->hour, hdr->minute);
		ptr += sizeof(*hdr);
		len -= sizeof(*hdr);
	}

	// Matches with _rtl88ee_poweroff_adapter
	// if ((rtl_read_byte(rtlpriv, REG_MCUFWDL) & BIT(7)) && rtlhal->fw_ready)
	if (rtwn_read_1(sc, R92C_MCUFWDL) & 0x80) { //R92C_MCUFWDL_RAM_DL_SEL) {
		rtwn_write_1(sc, R92C_MCUFWDL, 0); // hw.c 1418
		rtwn_fw_reset(sc, RTWN_FW_RESET_DOWNLOAD);
		// rtl88e_firmware_selfreset in fw.c
	}

	/* Enable firmware download. */
	rtwn_fw_download_enable(sc, 1);
	// _rtl88e_enable_fw_download(hw, true);

	// Now this: _rtl88e_write_fw(hw, version, pfwdata, fwsize);
	// For now, I am leaving this as-is, but will come back to this.
/////////////////////////////////////////////////////////////////////////////////
#if 0
	error = 0;	/* compiler warning */
	for (ntries = 0; ntries < 3; ntries++) {
		const u_char *curr_ptr = ptr;
		const int maxpages = len / R92C_FW_PAGE_SIZE;
		int page;

		/* Reset the FWDL checksum. */
		rtwn_setbits_1(sc, R92C_MCUFWDL, 0, R92C_MCUFWDL_CHKSUM_RPT);

		for (page = 0; page < maxpages; page++) {
			error = rtwn_fw_loadpage(sc, page, curr_ptr,
			    R92C_FW_PAGE_SIZE);
			if (error != 0)
				break;
			curr_ptr += R92C_FW_PAGE_SIZE;
		}
		if (page != maxpages)
			continue;

		if (len % R92C_FW_PAGE_SIZE != 0) {
			error = rtwn_fw_loadpage(sc, page, curr_ptr,
			    len % R92C_FW_PAGE_SIZE);
			if (error != 0)
				continue;
		}

		/* Wait for checksum report. */
		error = rtwn_fw_checksum_report(sc);
		if (error == 0)
			break;
	}
	if (ntries == 3) {
		device_printf(sc->sc_dev,
		    "%s: failed to upload firmware %s (error %d)\n",
		    __func__, sc->fwname, error);
		goto fail;
	} else {
		device_printf(sc->sc_dev, "Does not fail I guess?");
	}
#else
	uint32_t pagenums, remainsize;
//	const u_char *curr_ptr = ptr;
	int page;

	printf("If %zu is 0, rtl_fill_dummy not necessary\n", len % 4);

	for (ntries = 0; ntries < 3; ntries++) {
		const unsigned char *curr_ptr = fw->data;
		pagenums = fw->datasize / 4096;
		remainsize = fw->datasize % 4096;

		rtwn_setbits_1(sc, R92C_MCUFWDL, 0, R92C_MCUFWDL_CHKSUM_RPT);

		if (pagenums > 8)
			printf("Page numbers should not be greater than 8\n");

		for (page = 0; page < pagenums; page++) {
			printf("Iter1\n");
			error = rtwn_fw_loadpage(sc, page, curr_ptr, 4096);
			if (error != 0)
				break;
			curr_ptr += 4096;
		}
		printf("No more loop\n");

		if (remainsize) {
			printf("Final\n");
			page = pagenums;
			error = rtwn_fw_loadpage(sc, page, curr_ptr, 4096);
			if (error != 0)
				break;
		}

		error = rtwn_fw_checksum_report(sc);
		if (error == 0)
			break;
	}

	if (ntries == 3) {
		printf("Error: EEEEEEEEEEEERRRRRRRRRRRRROOOOOOOOOOORRRRRRRRRRRRRRR\n");
		device_printf(sc->sc_dev, "%s: failed to upload firmware %s (error %d)\n", __func__, sc->fwname, error);
		goto fail;
	}
#endif
	
/////////////////////////////////////////////////////////////////////////////////

	// _rtl88e_enable_fw_download(hw, true);
	/* Disable firmware download. */
	rtwn_fw_download_enable(sc, 0);
	// Linux's r88ee_fw_download_enable

	// Matched by rtl8188ee/fw.c 105-108
	rtwn_setbits_4(sc, R92C_MCUFWDL, R92C_MCUFWDL_WINTINI_RDY,
	    R92C_MCUFWDL_RDY);
	// Matched by rtl8188ee/fw.c 105-108

	rtwn_fw_reset(sc, RTWN_FW_RESET_CHECKSUM);

	/* Wait for firmware readiness. */
	// This appears to be the same as fw.c 113-120
	for (ntries = 0; ntries < 20; ntries++) {
		if (rtwn_read_4(sc, R92C_MCUFWDL) & R92C_MCUFWDL_WINTINI_RDY)
			break;
		rtwn_delay(sc, 50000);
	}
	if (ntries == 20) {
		printf("DELETE ME ERROR !!!!!!!!!!!!!!!!!!\n");
		device_printf(sc->sc_dev,
		    "timeout waiting for firmware readiness\n");
		error = ETIMEDOUT;
		goto fail;
	}
fail:
	firmware_put(fw, FIRMWARE_UNLOAD);
	return (error);
}
#endif
