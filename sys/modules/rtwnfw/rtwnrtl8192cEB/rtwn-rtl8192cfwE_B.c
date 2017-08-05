/*
 * Automatically generated by:
 * $FreeBSD$
 */
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/linker.h>
#include <sys/firmware.h>
#include <sys/systm.h>

extern char _binary_rtwn_rtl8192cfwE_B_fw_start[], _binary_rtwn_rtl8192cfwE_B_fw_end[];

static int
rtwn_rtl8192cfwE_B_fw_modevent(module_t mod, int type, void *unused)
{
	const struct firmware *fp, *parent;
	int error;
	switch (type) {
	case MOD_LOAD:

		fp = firmware_register("rtwn-rtl8192cfwE_B", _binary_rtwn_rtl8192cfwE_B_fw_start , (size_t)(_binary_rtwn_rtl8192cfwE_B_fw_end - _binary_rtwn_rtl8192cfwE_B_fw_start), 111, NULL);
		if (fp == NULL)
			goto fail_0;
		parent = fp;
		return (0);
	fail_0:
		return (ENXIO);
	case MOD_UNLOAD:
		error = firmware_unregister("rtwn-rtl8192cfwE_B");
		return (error);
	}
	return (EINVAL);
}

static moduledata_t rtwn_rtl8192cfwE_B_fw_mod = {
        "rtwn_rtl8192cfwE_B_fw",
        rtwn_rtl8192cfwE_B_fw_modevent,
        0
};
DECLARE_MODULE(rtwn_rtl8192cfwE_B_fw, rtwn_rtl8192cfwE_B_fw_mod, SI_SUB_DRIVERS, SI_ORDER_FIRST);
MODULE_VERSION(rtwn_rtl8192cfwE_B_fw, 1);
MODULE_DEPEND(rtwn_rtl8192cfwE_B_fw, firmware, 1, 1, 1);

