# Local Fork Changes

Personal changes on top of upstream `meshcore-dev/MeshCore`.
Remotes: `origin` = your fork, `upstream` = meshcore-dev.

## What changed

Branch `silent-no-led` adds one thing: a **silent, no-LED** build of the
T1000-E companion radio BLE firmware. Buzzer and blinking status LED are
compiled out entirely — not just disabled at runtime.

Files:

- `variants/t1000-e/platformio.ini` — new env
  `[env:t1000e_companion_radio_ble_silent]`. Extends the stock
  `t1000e_companion_radio_ble` env, adds a force-included header, and drops
  `helpers/ui/buzzer.cpp` from the source filter.
- `variants/t1000-e/silent_no_led.h` — force-included via `-include`.
  `#undef`s `PIN_BUZZER`, `PIN_BUZZER_EN`, `PIN_STATUS_LED`. All buzzer/LED
  code sits behind those `#ifdef`s, so the features vanish at compile time.
  Verified: zero buzzer/RTTTL symbols in the ELF.

Why a header instead of `-U` build flags: SCons reorders `-U` ahead of `-D`
on the command line, which breaks the undef. A force-included header runs
*after* command-line `-D` processing, so the undefs stick.

Still active (intentional): the status LED goes **solid** during power-off
while the button is held — power-off feedback, not blinking. Not removed.

## Build with PlatformIO

From the repo root. PlatformIO Core (`pio`) must be installed.

```sh
# 1. Compile firmware (produces .hex + .elf)
pio run -e t1000e_companion_radio_ble_silent

# 2. Convert hex -> uf2 (custom target)
pio run -e t1000e_companion_radio_ble_silent -t create_uf2
```

Output: `.pio/build/t1000e_companion_radio_ble_silent/firmware.uf2`

Verified build (2026-07-12):

- RAM:   60.7% — 142924 / 235520 bytes
- Flash: 52.2% — 369592 / 708608 bytes

Artifacts produced in the build dir: `firmware.elf`, `firmware.hex`,
`firmware.uf2`, `firmware.zip` (the `.zip` is the signed DFU package used
by the `nrfutil` upload path below).

Optional — write the uf2 to a custom path (e.g. straight to Desktop):

```sh
UF2_FILE_PATH=~/Desktop/meshcore-t1000e-companion-ble-silent-no-led.uf2 \
  pio run -e t1000e_companion_radio_ble_silent -t create_uf2
```

## Flash the T1000-E

Two options.

### Option A — UF2 bootloader (drag-and-drop, no tools)

1. Power off the T1000-E.
2. Hold the button while plugging in USB. It mounts as a UF2 drive.
3. Drag the `.uf2` onto the drive. It reboots into the new firmware
   automatically.

### Option B — serial DFU via `nrfutil` (verified 2026-07-12)

The env sets `upload_protocol = nrfutil`, so PlatformIO can flash directly
over the USB serial port. Put the device in DFU mode first (double-tap the
button, or trigger DFU from the app), then:

```sh
# --upload-port is optional; pio auto-detects if only one board is attached
pio run -e t1000e_companion_radio_ble_silent -t upload \
  --upload-port /dev/cu.usbmodem101
```

pio forces a 1200 bps reset to enter the bootloader, sends `firmware.zip`
via the bundled `tool-adafruit-nrfutil`, then activates the new firmware.
Expect `Device programmed.` on success (~50 s).

Find the port with `ls /dev/cu.usbmodem*` (macOS) or `pio device list`.

### PlatformIO in VS Code

Same build + flash from the extension GUI:

1. Click the **PlatformIO** alien icon in the Activity Bar (left rail).
2. Open Meshcore project in PlatformIO a few times, Expand **PROJECT TASKS → env:t1000e_companion_radio_ble_silent**.
3. **Build** — run **General → Build** (or the ✓ checkmark in the bottom
   status bar). Produces the `.elf`/`.hex`; run the **create_uf2** task under
   the env for the `.uf2`.
4. **Flash** — put the device in DFU mode, then run **General → Upload** (or
   the → arrow in the status bar). Uses the `nrfutil` protocol above.
   - The status-bar env selector (bottom, e.g. `Default (MeshCore)`) sets
     which env Build/Upload target — pick
     `env:t1000e_companion_radio_ble_silent`.
   - Watch the integrated terminal for `Device programmed.`.

## Keep the fork updated

```sh
git fetch upstream
git checkout main && git merge upstream/main && git push origin main
git checkout silent-no-led && git rebase main && git push --force-with-lease
```
