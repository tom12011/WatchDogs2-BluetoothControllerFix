# Compatibility matrix

Compatibility is determined first by the exact game-file hashes, not by storefront branding. Other configurations are welcome as community test submissions, but are not automatically considered supported.

## Game build

| Configuration | Status | Evidence |
|---|---|---|
| `WatchDogs2.exe` SHA-256 `DBC465…97F3` plus `Disrupt_64.dll` SHA-256 `1BC77B…D865` | **Tested working** | Physical in-game test and automated binary validation |
| Same engine hash with a different executable hash | Untested | Internal engine checks may pass, but no compatibility claim is made |
| Different engine hash | Unsupported/untested | Do not force installation; separate analysis is required |
| Steam-labelled installation | Untested as a storefront category | Compatibility must be established by hashes |
| Ubisoft Connect-labelled installation | Untested as a storefront category | Compatibility must be established by hashes |
| Epic Games-labelled installation | Untested as a storefront category | Compatibility must be established by hashes |

The tested environment used an existing Playnite → WeMod launch flow and included a modified Ubisoft/Uplay loader. That observation does not establish compatibility with every copy from any storefront.

## Controllers and transport

| Controller / connection | Status | Notes |
|---|---|---|
| Xbox Elite Wireless Controller Series 2 — Bluetooth | **Tested working** | Menus, gameplay, buttons, sticks, triggers, and vibration |
| Xbox Elite Wireless Controller Series 2 — USB-C | **Tested working** | Native wired input retained |
| Elite Series 2 — Bluetooth → USB-C switch | **Tested working** | In tested session |
| Elite Series 2 — USB-C → Bluetooth switch | **Tested working** | In tested session |
| Standard Xbox Series controller — Bluetooth | Untested | Submit a compatibility report |
| Xbox One controller — Bluetooth | Untested | Submit a compatibility report |
| Xbox Wireless Adapter | Untested | No claim |
| Third-party Xbox-compatible controller | Untested | No claim |
| PlayStation controller or wrapper | Out of scope | This project corrects a native XInput activation path |

## Operating systems

| OS | Status |
|---|---|
| Windows 10 x64, build 19045 | **Tested working** |
| Other Windows 10 builds | Untested |
| Windows 11 | Untested |
| Windows on ARM | Unsupported/untested |
| Linux/Proton/Wine | Untested |

## Other input software

| Configuration | Status |
|---|---|
| No virtual-controller or remapping layer | **Tested working** |
| Playnite → WeMod launch flow | **Tested working** |
| Steam Input enabled | Untested |
| DS4Windows, reWASD, x360ce, ViGEm, HidHide, or similar | Untested; possible conflict |
| Another local `xinput9_1_0.dll` proxy | Not directly compatible without deliberate proxy chaining |

## Safety scope

| Mode | Status |
|---|---|
| Offline single-player | Intended use |
| Multiplayer | Unsupported |
| Easy Anti-Cheat active | Unsupported; do not use |

To contribute a result, use the repository's compatibility-test issue form and include hashes rather than uploading proprietary binaries.
