
#ifndef RTL88EE_PWRSEQ_H
#define RTL88EE_PWRSEQ_H

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

#include <net/if.h>
#include <net/if_var.h>
#include <net/ethernet.h>
#include <net/if_media.h>

#include <net80211/ieee80211_var.h>
#include <net80211/ieee80211_radiotap.h>
#include <net80211/ieee80211_ratectl.h>
#ifdef  IEEE80211_SUPPORT_SUPERG
#include <net80211/ieee80211_superg.h>
#endif

#include <dev/rtwn/if_rtwnreg.h>
#include <dev/rtwn/if_rtwnvar.h>

// This should be removed, because it is Linux-specific
#define BIT(nr)     (1 << (nr))


// Taken from drivers/net/wireless/realtek/rtlwifi/rtl8188ee/pwrseq.h

#define REG_TX_RPT_CTRL		0x04EC
#define REG_RXDMA_CONTROL	0x0286
#define REG_PCIE_CTRL_REG	0x0300
#define REG_RF_CTRL		0x001f
#define REG_MCUFWDL		0x0080
#define REG_SYS_FUNC_EN		0x0002
#define REG_32K_CTRL		0x0194
#define REG_RSV_CTRL		0x001C
#define REG_GPIO_PIN_CTRL	0x0044
#define	GPIO_IO_SEL		(REG_GPIO_PIN_CTRL+2)
#define REG_GPIO_IO_SEL		0x0042
#define GPIO_IN			REG_GPIO_PIN_CTRL
#define GPIO_OUT		(REG_GPIO_PIN_CTRL+1)
#define REG_GPIO_IO_SEL_2	0x0062


// pwrseqcmd.h from root RTL directory
/*---------------------------------------------
 * 3 The value of cmd: 4 bits
 *---------------------------------------------
 */
#define    PWR_CMD_READ         0x00
#define    PWR_CMD_WRITE        0x01
#define    PWR_CMD_POLLING      0x02
#define    PWR_CMD_DELAY        0x03
#define    PWR_CMD_END          0x04

/* define the base address of each block */
#define   PWR_BASEADDR_MAC      0x00
#define   PWR_BASEADDR_USB      0x01
#define   PWR_BASEADDR_PCIE     0x02
#define   PWR_BASEADDR_SDIO     0x03

#define PWR_INTF_SDIO_MSK       BIT(0)
#define PWR_INTF_USB_MSK        BIT(1)
#define PWR_INTF_PCI_MSK        BIT(2)
#define PWR_INTF_ALL_MSK        (BIT(0)|BIT(1)|BIT(2)|BIT(3))

#define PWR_FAB_TSMC_MSK        BIT(0)
#define PWR_FAB_UMC_MSK         BIT(1)
#define PWR_FAB_ALL_MSK         (BIT(0)|BIT(1)|BIT(2)|BIT(3))

#define PWR_CUT_TESTCHIP_MSK    BIT(0)
#define PWR_CUT_A_MSK           BIT(1)
#define PWR_CUT_B_MSK           BIT(2)
#define PWR_CUT_C_MSK           BIT(3)
#define PWR_CUT_D_MSK           BIT(4)
#define PWR_CUT_E_MSK           BIT(5)
#define PWR_CUT_F_MSK           BIT(6)
#define PWR_CUT_G_MSK           BIT(7)
#define PWR_CUT_ALL_MSK         0xFF

enum pwrseq_delay_unit {
        PWRSEQ_DELAY_US,
        PWRSEQ_DELAY_MS,
};

struct wlan_pwr_cfg {
        uint16_t	offset;
        uint8_t		cut_msk;
        uint8_t		fab_msk:4;
        uint8_t		interface_msk:4;
        uint8_t		base:4;
        uint8_t		cmd:4;
        uint8_t		msk;
        uint8_t		value;
};

#define GET_PWR_CFG_OFFSET(__PWR_CMD)   (__PWR_CMD.offset)
#define GET_PWR_CFG_CUT_MASK(__PWR_CMD) (__PWR_CMD.cut_msk)
#define GET_PWR_CFG_FAB_MASK(__PWR_CMD) (__PWR_CMD.fab_msk)
#define GET_PWR_CFG_INTF_MASK(__PWR_CMD)        (__PWR_CMD.interface_msk)
#define GET_PWR_CFG_BASE(__PWR_CMD)     (__PWR_CMD.base)
#define GET_PWR_CFG_CMD(__PWR_CMD)      (__PWR_CMD.cmd)
#define GET_PWR_CFG_MASK(__PWR_CMD)     (__PWR_CMD.msk)
#define GET_PWR_CFG_VALUE(__PWR_CMD)    (__PWR_CMD.value)

// End of the pwrseqcmd.h from root RTL directory

#define RTL8188EE_TRANS_CARDEMU_TO_ACT_STEPS    10
#define RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS    10
#define RTL8188EE_TRANS_CARDEMU_TO_SUS_STEPS    10
#define RTL8188EE_TRANS_SUS_TO_CARDEMU_STEPS    10
#define RTL8188EE_TRANS_CARDEMU_TO_PDN_STEPS    10
#define RTL8188EE_TRANS_PDN_TO_CARDEMU_STEPS    10
#define RTL8188EE_TRANS_ACT_TO_LPS_STEPS                15
#define RTL8188EE_TRANS_LPS_TO_ACT_STEPS                15
#define RTL8188EE_TRANS_END_STEPS               1

/* The following macros have the following format:
 * { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value
 *   comments },
 */
#define RTL8188EE_TRANS_CARDEMU_TO_ACT                                  \
        {0x0006, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_POLLING, BIT(1), BIT(1)               \
        /* wait till 0x04[17] = 1    power ready*/},                    \
        {0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(0)|BIT(1), 0               \
        /* 0x02[1:0] = 0        reset BB*/},                            \
        {0x0026, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), BIT(7)                 \
        /*0x24[23] = 2b'01 schmit trigger */},                          \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), 0                      \
        /* 0x04[15] = 0 disable HWPDN (control by DRV)*/},              \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4)|BIT(3), 0               \
        /*0x04[12:11] = 2b'00 disable WL suspend*/},                    \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(0), BIT(0)                 \
        /*0x04[8] = 1 polling until return 0*/},                        \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_POLLING, BIT(0), 0                    \
        /*wait till 0x04[8] = 0*/},                                     \
        {0x0023, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), 0                      \
        /*LDO normal mode*/},                                           \
        {0x0074, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), BIT(4)                 \
        /*SDIO Driving*/},

#define RTL8188EE_TRANS_ACT_TO_CARDEMU                                  \
        {0x001F, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0                        \
        /*0x1F[7:0] = 0 turn off RF*/},                                 \
        {0x0023, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), BIT(4)                 \
        /*LDO Sleep mode*/},                                            \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(1), BIT(1)                 \
        /*0x04[9] = 1 turn off MAC by HW state machine*/},              \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_POLLING, BIT(1), 0                    \
        /*wait till 0x04[9] = 0 polling until return 0 to disable*/},

#define RTL8188EE_TRANS_CARDEMU_TO_SUS                                  \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,                      \
        PWR_INTF_USB_MSK|PWR_INTF_SDIO_MSK,                             \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(3)|BIT(4), BIT(3)          \
        /*0x04[12:11] = 2b'01enable WL suspend*/},                      \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(3)|BIT(4), BIT(3)|BIT(4)   \
        /*0x04[12:11] = 2b'11enable WL suspend for PCIe*/},             \
        {0x0007, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,                      \
        PWR_INTF_USB_MSK|PWR_INTF_SDIO_MSK,                             \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, BIT(7)                   \
        /*  0x04[31:30] = 2b'10 enable enable bandgap mbias in suspend */},\
        {0x0041, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,                      \
        PWR_INTF_USB_MSK|PWR_INTF_SDIO_MSK,                             \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), 0                      \
        /*Clear SIC_EN register 0x40[12] = 1'b0 */},                    \
        {0xfe10, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,                      \
        PWR_INTF_USB_MSK|PWR_INTF_SDIO_MSK,                             \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), BIT(4)                 \
        /*Set USB suspend enable local register  0xfe10[4]=1 */},       \
        {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
        PWR_BASEADDR_SDIO, PWR_CMD_WRITE, BIT(0), BIT(0)                \
        /*Set SDIO suspend local register*/},                           \
        {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
        PWR_BASEADDR_SDIO, PWR_CMD_POLLING, BIT(1), 0                   \
        /*wait power state to suspend*/},

#define RTL8188EE_TRANS_SUS_TO_CARDEMU                                  \
        {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
        PWR_BASEADDR_SDIO, PWR_CMD_WRITE, BIT(0), 0                     \
        /*Set SDIO suspend local register*/},                           \
        {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
        PWR_BASEADDR_SDIO, PWR_CMD_POLLING, BIT(1), BIT(1)              \
        /*wait power state to suspend*/},                               \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(3) | BIT(4), 0             \
        /*0x04[12:11] = 2b'01enable WL suspend*/},

#define RTL8188EE_TRANS_CARDEMU_TO_CARDDIS                              \
        {0x0026, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), BIT(7)                 \
        /*0x24[23] = 2b'01 schmit trigger */},                          \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,                      \
        PWR_INTF_USB_MSK|PWR_INTF_SDIO_MSK,                             \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(3) | BIT(4), BIT(3)        \
        /*0x04[12:11] = 2b'01 enable WL suspend*/},                     \
        {0x0007, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,                      \
        PWR_INTF_USB_MSK|PWR_INTF_SDIO_MSK,                             \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0                        \
        /*  0x04[31:30] = 2b'10 enable enable bandgap mbias in suspend */},\
        {0x0041, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,                      \
        PWR_INTF_USB_MSK|PWR_INTF_SDIO_MSK,                             \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), 0                      \
        /*Clear SIC_EN register 0x40[12] = 1'b0 */},                    \
        {0xfe10, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), BIT(4)                 \
        /*Set USB suspend enable local register  0xfe10[4]=1 */},       \
        {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
        PWR_BASEADDR_SDIO, PWR_CMD_WRITE, BIT(0), BIT(0)                \
        /*Set SDIO suspend local register*/},                           \
        {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
        PWR_BASEADDR_SDIO, PWR_CMD_POLLING, BIT(1), 0                   \
        /*wait power state to suspend*/},

#define RTL8188EE_TRANS_CARDDIS_TO_CARDEMU                              \
        {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
        PWR_BASEADDR_SDIO, PWR_CMD_WRITE, BIT(0), 0                     \
        /*Set SDIO suspend local register*/},                           \
        {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
        PWR_BASEADDR_SDIO, PWR_CMD_POLLING, BIT(1), BIT(1)              \
        /*wait power state to suspend*/},                               \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(3)|BIT(4), 0               \
        /*0x04[12:11] = 2b'01enable WL suspend*/},
#define RTL8188EE_TRANS_CARDEMU_TO_PDN                                  \
        {0x0006, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(0), 0/* 0x04[16] = 0*/},   \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), BIT(7)                 \
        /* 0x04[15] = 1*/},

#define RTL8188EE_TRANS_PDN_TO_CARDEMU                                  \
        {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), 0/* 0x04[15] = 0*/},

#define RTL8188EE_TRANS_ACT_TO_LPS                                      \
        {0x0522, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0x7F                     \
        /*Tx Pause*/},                                                  \
        {0x05F8, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_POLLING, 0xFF, 0                      \
        /*Should be zero if no packet is transmitting*/},               \
        {0x05F9, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_POLLING, 0xFF, 0                      \
        /*Should be zero if no packet is transmitting*/},               \
        {0x05FA, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_POLLING, 0xFF, 0                      \
        /*Should be zero if no packet is transmitting*/},               \
        {0x05FB, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_POLLING, 0xFF, 0                      \
        /*Should be zero if no packet is transmitting*/},               \
        {0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(0), 0                      \
        /*CCK and OFDM are disabled,and clock are gated*/},             \
        {0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_DELAY, 0, PWRSEQ_DELAY_US             \
        /*Delay 1us*/},                                                 \
        {0x0100, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0x3F                     \
        /*Reset MAC TRX*/},                                             \
        {0x0101, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(1), 0                      \
        /*check if removed later*/},                                    \
        {0x0553, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(5), BIT(5)                 \
        /*Respond TxOK to scheduler*/},

#define RTL8188EE_TRANS_LPS_TO_ACT                                      \
        {0x0080, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,   \
        PWR_BASEADDR_SDIO, PWR_CMD_WRITE, 0xFF, 0x84                    \
        /*SDIO RPWM*/},                                                 \
        {0xFE58, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0x84                     \
        /*USB RPWM*/},                                                  \
        {0x0361, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0x84                     \
        /*PCIe RPWM*/},                                                 \
        {0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_DELAY, 0, PWRSEQ_DELAY_MS             \
        /*Delay*/},                                                     \
        {0x0008, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), 0                      \
        /*.     0x08[4] = 0              switch TSF to 40M*/},          \
        {0x0109, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_POLLING, BIT(7), 0                    \
        /*Polling 0x109[7]=0  TSF in 40M*/},                            \
        {0x0029, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(6)|BIT(7), 0               \
        /*.     0x29[7:6] = 2b'00        enable BB clock*/},            \
        {0x0101, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(1), BIT(1)                 \
        /*.     0x101[1] = 1*/},                                        \
        {0x0100, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0xFF                     \
        /*.     0x100[7:0] = 0xFF        enable WMAC TRX*/},            \
        {0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(1)|BIT(0), BIT(1)|BIT(0)   \
        /*.     0x02[1:0] = 2b'11        enable BB macro*/},            \
        {0x0522, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,    \
        PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0                        \
        /*.     0x522 = 0*/},

#define RTL8188EE_TRANS_END             \
        {0xFFFF, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,\
        0, PWR_CMD_END, 0, 0}

extern struct wlan_pwr_cfg rtl8188ee_power_on_flow
                [RTL8188EE_TRANS_CARDEMU_TO_ACT_STEPS +
                 RTL8188EE_TRANS_END_STEPS];
extern struct wlan_pwr_cfg rtl8188ee_radio_off_flow
                [RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS +
                 RTL8188EE_TRANS_END_STEPS];
extern struct wlan_pwr_cfg rtl8188ee_card_disable_flow
                [RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS +
                 RTL8188EE_TRANS_CARDEMU_TO_PDN_STEPS +
                 RTL8188EE_TRANS_END_STEPS];
extern struct wlan_pwr_cfg rtl8188ee_card_enable_flow
                [RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS +
                 RTL8188EE_TRANS_CARDEMU_TO_PDN_STEPS +
                 RTL8188EE_TRANS_END_STEPS];
extern struct wlan_pwr_cfg rtl8188ee_suspend_flow
                [RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS +
                 RTL8188EE_TRANS_CARDEMU_TO_SUS_STEPS +
                 RTL8188EE_TRANS_END_STEPS];
extern struct wlan_pwr_cfg rtl8188ee_resume_flow
                [RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS +
                 RTL8188EE_TRANS_CARDEMU_TO_SUS_STEPS +
                 RTL8188EE_TRANS_END_STEPS];
extern struct wlan_pwr_cfg rtl8188ee_hwpdn_flow
                [RTL8188EE_TRANS_ACT_TO_CARDEMU_STEPS +
                 RTL8188EE_TRANS_CARDEMU_TO_PDN_STEPS +
                 RTL8188EE_TRANS_END_STEPS];
extern struct wlan_pwr_cfg rtl8188ee_enter_lps_flow
                [RTL8188EE_TRANS_ACT_TO_LPS_STEPS +
                 RTL8188EE_TRANS_END_STEPS];
extern struct wlan_pwr_cfg rtl8188ee_leave_lps_flow
                [RTL8188EE_TRANS_LPS_TO_ACT_STEPS +
                 RTL8188EE_TRANS_END_STEPS];

#define RTL8188EE_NIC_PWR_ON_FLOW       rtl8188ee_power_on_flow
#define RTL8188EE_NIC_RF_OFF_FLOW       rtl8188ee_radio_off_flow
#define RTL8188EE_NIC_DISABLE_FLOW      rtl8188ee_card_disable_flow
#define RTL8188EE_NIC_ENABLE_FLOW       rtl8188ee_card_enable_flow
#define RTL8188EE_NIC_SUSPEND_FLOW      rtl8188ee_suspend_flow
#define RTL8188EE_NIC_RESUME_FLOW       rtl8188ee_resume_flow
#define RTL8188EE_NIC_PDN_FLOW          rtl8188ee_hwpdn_flow
#define RTL8188EE_NIC_LPS_ENTER_FLOW    rtl8188ee_enter_lps_flow
#define RTL8188EE_NIC_LPS_LEAVE_FLOW    rtl8188ee_leave_lps_flow

bool pwrseqcmdparsing(struct rtwn_softc *sc, uint8_t cut_version,
                              uint8_t faversion, uint8_t interface_type,
                              struct wlan_pwr_cfg pwrcfgcmd[]);

#endif
