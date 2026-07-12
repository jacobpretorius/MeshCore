#pragma once

// Force-included via -include in [env:t1000e_companion_radio_ble_silent].
// Runs after command-line -D processing, so these undefs strip the buzzer
// and status-LED features from every translation unit.
#undef PIN_BUZZER
#undef PIN_BUZZER_EN
#undef PIN_STATUS_LED
