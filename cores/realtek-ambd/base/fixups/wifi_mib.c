/* Copyright (c) Bl00d-B0b 2026-08-10. */

#include <wifi_conf.h>
#include <wifi_util.h>

// Overrides the SDK's _WEAK wifi_set_mib() (wifi_conf.c). Identical to the
// vendor defaults except:
// - 802.11v BTM roaming and 802.11k are DISABLED: controllers with client
//   steering (e.g. CAPsMAN) steer clients that advertise them, deferring
//   quick re-associations with assoc-reject status 30 — every warm reboot
//   waited out the deferral (bench 2026-08-10);
// - power save starts disabled, matching setSleep(false) semantics (the
//   vendor default re-enables PS mode 1 under CONFIG_POWER_SAVING).
void wifi_set_mib(void) {
	wext_set_adaptivity(RTW_ADAPTIVITY_DISABLE);
	wext_auto_set_adaptivity(DISABLE);
	wext_set_trp_tis(RTW_TRP_TIS_DISABLE);
	wext_set_anti_interference(DISABLE);
	wext_set_roam_on_btm(0);
	wext_set_enable_80211k(0);
	wext_set_powersave_mode(0);
#ifdef CONFIG_SAE_SUPPORT
	wext_set_support_wpa3(ENABLE);
#endif
	wext_set_ant_div_gpio(0);
	wext_set_bw40_enable(0);
	wext_set_softap_gkey_rekey(DISABLE);
#ifdef CONFIG_80211N_HT
	wext_set_wifi_ampdu_tx(ENABLE);
#endif
}
