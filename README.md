# Watch Dogs 2 Bluetooth Controller Fix

An experimental, build-specific compatibility fix for a Watch Dogs 2 controller-initialisation bug that can prevent an Xbox Elite Wireless Controller Series 2 from working over Bluetooth.

The mod restores the game's existing native XInput path. It does **not** emulate a controller, install a driver, create a virtual gamepad, or run a background application.

> [!WARNING]
> This project is intended only for legitimate offline single-player use. Multiplayer and Easy Anti-Cheat-enabled use are unsupported. The project does not disable, bypass, evade, or modify anti-cheat.

## Project status

`v0.9.0-beta` uses the exact DLL that passed physical in-game testing. The beta label reflects the narrow compatibility sample—not uncertainty about which binary was tested.

Tested successfully with:

- Xbox Elite Wireless Controller Series 2 over Bluetooth.
- Menus and gameplay.
- Buttons, sticks, triggers, and native XInput vibration.
- USB-C input.
- Bluetooth-to-USB-C and USB-C-to-Bluetooth switching.
- Windows 10 x64, build 19045.
- An existing Playnite → WeMod launch flow.

Other controllers, operating systems, storefront variants, game builds, and launch configurations remain untested unless recorded in the [compatibility matrix](docs/COMPATIBILITY.md). No universal Steam, Ubisoft Connect, or Epic Games compatibility is claimed.

## Why the controller is ignored

The controller is available to Windows through XInput over Bluetooth. Watch Dogs 2, however, first performs legacy DirectInput device discovery and uses that result to decide whether to create its native XInput controller objects.

In the tested environment, the Elite Series 2 Bluetooth transport was absent from the game's DirectInput discovery results. The game concluded that no XInput controller existed, skipped XInput-backend construction, and never called `XInputGetState`.

The fix makes a minimal, guarded in-memory correction to that activation decision. Inputs and vibration then continue through the game's own XInput implementation and the genuine Windows System32 XInput DLL.

See [Technical details](docs/TECHNICAL.md) for the complete control-flow evidence and safety checks.

## Supported game build

The authoritative identifiers are:

| File | SHA-256 |
|---|---|
| `WatchDogs2.exe` | `DBC465877DD69F9B2C97B6004D713FC2C2166298514AF516AEE8BF2FBD0A97F3` |
| `Disrupt_64.dll` | `1BC77B469A3591BEC36B3AF41D513F45BA2887BC338276B1388B5DD932CCD865` |

Secondary PE metadata:

- AMD64 / PE32+.
- `WatchDogs2.exe` FileVersion and ProductVersion: `0.1.0.1`.
- `WatchDogs2.exe` timestamp: `0x59131F5C` (2017-05-10 14:10:36 UTC).
- `Disrupt_64.dll` timestamp: `0x59774D30` (2017-07-25 13:52:48 UTC).

No conventional Ubisoft patch number has been verified. `0.1.0.1` and the PE timestamps are file metadata, not confirmed retail patch identifiers. Compare both SHA-256 hashes before installation.

```powershell
Get-FileHash .\WatchDogs2.exe -Algorithm SHA256
Get-FileHash .\Disrupt_64.dll -Algorithm SHA256
```

## Installation

1. Download `WatchDogs2-BluetoothControllerFix-v0.9.0-beta.zip` from the GitHub Release.
2. Close Watch Dogs 2 completely and ensure Easy Anti-Cheat is not running.
3. Extract the ZIP.
4. Locate the directory containing both `WatchDogs2.exe` and `Disrupt_64.dll`. This may be a `bin` subdirectory rather than the top-level game directory.
5. If `xinput9_1_0.dll` already exists there, stop. Identify it and preserve a backup; do not overwrite another mod blindly.
6. Copy the release's `xinput9_1_0.dll` beside `WatchDogs2.exe`.
7. Launch the game normally while offline.

Administrator access is normally unnecessary unless the game directory's Windows permissions require elevation.

Release DLL SHA-256:

`F4112AFAB93E3A9200E2F442286A80EF76EA5BB0552CE9F0A68F2E004BED38CC`

## Uninstallation

1. Close Watch Dogs 2.
2. Open the directory containing `WatchDogs2.exe`.
3. Verify that the local `xinput9_1_0.dll` is this mod's DLL if there is any doubt.
4. Delete the mod's `xinput9_1_0.dll`.
5. Restore any previous local DLL that you backed up before installation.

There is no need to replace or restore `WatchDogs2.exe` or `Disrupt_64.dll`; this mod never changes them on disk.

## Build-specific safety

During DLL attachment, the proxy validates the loaded engine's:

- AMD64 machine type;
- PE timestamp;
- image size;
- original activation-branch bytes; and
- 25 original unused code-cave bytes.

If any check differs, DLL initialisation fails before the patch is applied. Users should still compare the complete game-file hashes because the internal guard intentionally avoids file I/O while the Windows loader lock is held.

## Building from source

The source is under [`src/`](src/). See [BUILDING.md](BUILDING.md) for the pinned Zig toolchain and commands.

The release asset contains the frozen, physically tested DLL. A locally rebuilt DLL is a new artifact and must complete the full validation process before being represented as equivalent to the tested release.

## Reporting results

- Use the [bug report form](../../issues/new?template=bug_report.yml) for failures on the supported build.
- Use the [compatibility test form](../../issues/new?template=compatibility_test.yml) to submit results from another controller, Windows version, storefront, or game build.

Do not submit proprietary game binaries, crash dumps containing private data, or anti-cheat bypass requests.

## Limitations

- Only one exact executable/engine build is currently supported.
- Only the Elite Series 2 and reported Windows/launcher configuration have been physically tested.
- Compatibility with other local DLL proxies, injectors, remappers, and controller tools is not guaranteed.
- Unsupported builds may fail to launch until the local proxy is removed.
- Multiplayer and anti-cheat-enabled use are unsupported.

## Licensing

Released under the [MIT License](LICENSE). See [NOTICE.md](NOTICE.md) for trademark and affiliation information.
