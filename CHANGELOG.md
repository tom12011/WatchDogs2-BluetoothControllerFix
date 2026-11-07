# Changelog

All notable public changes are documented here.

## [0.9.0-beta] — 2026-11-07

- First prepared public beta.
- Preserves the exact physically tested release DLL.
- Enables native XInput backend construction when legacy DirectInput discovery reports zero XInput devices.
- Tested with Xbox Elite Wireless Controller Series 2 over Bluetooth and USB-C.
- Tested Bluetooth/USB-C switching, menu input, gameplay input, and vibration.
- Adds exact engine identity and original-byte validation.
- Fails DLL initialisation on unsupported engine identity or patch bytes.
- Removes diagnostic telemetry, TSV tracing, status mappings, and routine file I/O.
- Adds source, build instructions, technical documentation, compatibility matrix, and structured issue forms.

Release DLL SHA-256:

`F4112AFAB93E3A9200E2F442286A80EF76EA5BB0552CE9F0A68F2E004BED38CC`

## Engineering history

Earlier diagnostic and experimental builds established that Bluetooth XInput worked at the Windows API level while the game skipped XInput polling. Those builds are intentionally not distributed in this repository or release asset.
