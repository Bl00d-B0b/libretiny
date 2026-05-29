#pragma once

// Default BLE application user configuration for LibreTiny LN882H.
// All settings here are defaults; ln_ble_app_default_cfg.h guards every value
// with #if !defined, so applications that need different settings (e.g. the
// ln882h_ble_tracker ESPHome component) can prepend their own include path and
// this file will be shadowed by their version.

// Observer / Central role — no advertising, scanning only.
#define BLE_DEFAULT_ROLE    BLE_ROLE_CENTRAL
#define BLE_CONFIG_AUTO_ADV (0)
