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
 * $FreeBSD: head/sys/dev/rtwn/rtl8188ee/r88ee_fw_cmd.h 307529 2016-10-17 20:38:24Z avos $
 */

#ifndef R88EE_FW_CMD_H
#define R88EE_FW_CMD_H

/*
 * Host to firmware commands.
 */
struct r88ee_fw_cmd {
	uint8_t	id;
#define R88EE_CMD_SET_PWRMODE		1
#define R88EE_CMD_JOINBSS_RPT		2
#define R88EE_CMD_RSVD_PAGE		3
#define R88EE_CMD_RSSI_SETTING		5
#define R88EE_CMD_MACID_CONFIG		6

#define R88EE_CMD_FLAG_EXT		0x80

	uint8_t	msg[5];
} __packed __attribute__((aligned(4)));

/* Structure for R88EE_CMD_JOINBSS_RPT. */
struct r88ee_fw_cmd_joinbss_rpt {
	uint8_t	mstatus;
#define R88EE_MSTATUS_DISASSOC		0x00
#define R88EE_MSTATUS_ASSOC		0x01
} __packed;

/* Structure for R88EE_CMD_SET_PWRMODE. */
struct r88ee_fw_cmd_pwrmode {
	uint8_t	mode;
#define R88EE_PWRMODE_CAM	0
#define R88EE_PWRMODE_MIN	1
#define R88EE_PWRMODE_MAX	2
#define R88EE_PWRMODE_DTIM	3
#define R88EE_PWRMODE_UAPSD_WMM	5
#define R88EE_PWRMODE_UAPSD	6
#define R88EE_PWRMODE_IBSS	7

	uint8_t	smart_ps;
/* XXX undocumented */
#define R88EE_PWRMODE_SMARTPS_NULLDATA	2

	uint8_t	bcn_pass;	/* unit: beacon interval */
} __packed;

/* Structure for R88EE_CMD_RSVD_PAGE. */
struct r88ee_fw_cmd_rsvdpage {
	uint8_t probe_resp;
	uint8_t ps_poll;
	uint8_t null_data;
} __packed;

/* Structure for R88EE_CMD_RSSI_SETTING. */
struct r88ee_fw_cmd_rssi {
	uint8_t	macid;
	uint8_t	reserved;
	uint8_t	pwdb;
} __packed;

/* Structure for R88EE_CMD_MACID_CONFIG. */
struct r88ee_fw_cmd_macid_cfg {
	uint32_t	mask;
	uint8_t		macid;
#define R88EE_CMD_MACID_VALID	0x80
} __packed;

/*
 * C2H event structure.
 */
/* Bigger value is used to prevent buffer overrun. */
#define R88EE_C2H_MSG_MAX_LEN		16

struct r88ee_c2h_evt {
	uint8_t		evtb0;
#define R88EE_C2H_EVTB0_ID_M		0x0f
#define R88EE_C2H_EVTB0_ID_S		0
#define R88EE_C2H_EVTB0_LEN_M		0xf0
#define R88EE_C2H_EVTB0_LEN_S		4

	uint8_t		seq;

	/* Followed by payload (see below). */
} __packed;

/*
 * C2H event types.
 */
#define R88EE_C2H_EVT_DEBUG		0
#define R88EE_C2H_EVT_TX_REPORT		3
#define R88EE_C2H_EVT_EXT_RA_RPT		6

/* Structure for R88EE_C2H_EVT_TX_REPORT event. */
struct r88ee_c2h_tx_rpt {
	uint8_t		rptb0;
#define R88EE_RPTB0_RETRY_CNT_M		0x3f
#define R88EE_RPTB0_RETRY_CNT_S		0

	uint8_t		rptb1;		/* XXX junk */
#define R88EE_RPTB1_RTS_RETRY_CNT_M	0x3f
#define R88EE_RPTB1_RTS_RETRY_CNT_S	0

	uint8_t		queue_time_low;
	uint8_t		queue_time_high;
	uint8_t		rptb4;
#define R88EE_RPTB4_MISSED_PKT_NUM_M	0x1f
#define R88EE_RPTB4_MISSED_PKT_NUM_S	0

	uint8_t		rptb5;
#define R88EE_RPTB5_MACID_M		0x1f
#define R88EE_RPTB5_MACID_S		0
#define R88EE_RPTB5_DES1_FRAGSSN_M	0xe0
#define R88EE_RPTB5_DES1_FRAGSSN_S	5

	uint8_t		rptb6;
#define R88EE_RPTB6_RPT_PKT_NUM_M	0x1f
#define R88EE_RPTB6_RPT_PKT_NUM_S	0
#define R88EE_RPTB6_PKT_DROP		0x20
#define R88EE_RPTB6_LIFE_EXPIRE		0x40
#define R88EE_RPTB6_RETRY_OVER		0x80

	uint8_t		rptb7;
#define R88EE_RPTB7_EDCA_M		0x0f
#define R88EE_RPTB7_EDCA_S		0
#define R88EE_RPTB7_BMC			0x20
#define R88EE_RPTB7_PKT_OK		0x40
#define R88EE_RPTB7_INT_CCX		0x80
} __packed;

#endif	/* R88EE_FW_CMD_H */
