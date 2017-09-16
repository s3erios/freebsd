#include <dev/rtwn/rtl8188e/pci/r88ee_pwrseq.h>

/* drivers should parse below arrays and do the corresponding actions */
/*3 Power on  Array*/
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

extern bool pwrseqcmdparsing(struct rtwn_softc *sc, uint8_t cut_version,
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

