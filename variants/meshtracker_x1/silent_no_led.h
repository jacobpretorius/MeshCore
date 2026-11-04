#pragma once

// Force-included via -include in [env:MeshTracker_X1_companion_radio_ble_silent].
// Runs after command-line -D processing, so these undefs strip the buzzer,
// status-LED (single + RGB) and DRV2605 haptic features from every
// translation unit.
#undef PIN_BUZZER
#undef PIN_BUZZER_EN
#undef PIN_STATUS_LED
#undef PIN_STATUS_LED_R
#undef PIN_STATUS_LED_G
#undef PIN_STATUS_LED_B
#undef HAS_DRV2605
#undef DRV2605_USE_LRA

// Stop the Bluefruit library driving LED_BLUE (blink while advertising,
// solid while connected).
#define DISABLE_BLE_LED
