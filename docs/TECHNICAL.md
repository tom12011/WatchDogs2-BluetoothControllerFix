# Technical explanation

## Confirmed failure mode

The Xbox Elite Wireless Controller Series 2 is exposed successfully through XInput over both Bluetooth and USB-C:

- XInput slot 0 in both transports.
- Successful state polling through XInput 9.1.0, 1.3, and 1.4.
- Buttons, sticks, and triggers present through both transports.
- Bluetooth VID/PID `045E:0B22`.
- USB VID/PID `045E:02FF`.

Despite this, the tested Watch Dogs 2 build did not call any XInput export over Bluetooth. The diagnostic proxy was loaded, but `XInputGetState` and `XInputSetState` remained uncalled. USB-C produced continuous successful native XInput polling.

## DirectInput activation dependency

The engine constructs its input manager and performs legacy ANSI DirectInput 8 enumeration before constructing its XInput gamepad objects.

Relevant RVAs in the supported `Disrupt_64.dll`:

| RVA | Role |
|---:|---|
| `0x68AC810` | Input-manager construction |
| `0x68AC8AE` | `DirectInput8Create` call using `IID_IDirectInput8A` |
| `0x68D0720` | Controller discovery and backend construction |
| `0x68D4540` | Reset discovered-device counters |
| `0x68B60D0` | DirectInput enumeration callback |
| `0x68D0775` | Test discovered XInput count at manager `+0x2588` |
| `0x68D077B` | Zero-count conditional branch |
| `0x68D07BD` | Construct four native XInput gamepad objects |
| `0x68D9880` | Native per-gamepad update method |
| `0x68D98DF` | `XInputGetState` call |

The enumeration callback obtains a DirectInput device path, lowercases it, and searches for a legacy contiguous pattern equivalent to `vid_%04x&pid_%04x&ig_0`. Only classified devices increment the XInput count.

The reproduced ANSI DirectInput enumeration returned no Bluetooth Xbox controller, even while a simultaneous XInput check reported slot 0 connected. The count remained zero, and the conditional branch skipped construction of every XInput controller object. This explains why the game made no XInput calls over Bluetooth despite Windows XInput working normally.

USB succeeds because its legacy identity follows the form expected by the classifier, including `VID_045E&PID_02FF&IG_00`.

## Compatibility correction

`Disrupt_64.dll` normally imports `XInputGetState` and `XInputSetState` from `XINPUT9_1_0.dll`; this is not a delay import. The local proxy is therefore initialised as an engine dependency before the controller manager performs initial enumeration.

During `DLL_PROCESS_ATTACH`, the proxy validates:

- the presence of the mapped `Disrupt_64.dll` module;
- DOS and PE signatures;
- AMD64 machine type;
- PE timestamp `0x59774D30`;
- image size `0x0C0B3000`;
- original branch bytes at RVA `0x68D077B`; and
- 25 original `INT3` bytes at RVA `0x68DAD60`.

The validated in-memory correction is:

- Branch RVA `0x68D077B`: `0F 84 A3 00 00 00` → `0F 84 DF A5 00 00`.
- Cave RVA `0x68DAD60`: set manager `+0x2588 = 1`, set `+0x258C = 1`, then jump back to RVA `0x68D0781`.

The cave executes only when legacy discovery found zero XInput devices. If USB discovery succeeds, the original fall-through path is unchanged and the cave is not executed.

This correction causes the game to construct the same four native XInput objects it already uses on a successful USB path. It does not fabricate controller state, change slot numbers, spoof a HID path, alter packet numbers, or modify buttons, sticks, triggers, or vibration.

## Forwarding

The proxy exports the same five names and ordinals expected from XInput 9.1.0. On the first forwarded call, outside `DllMain`, it resolves the genuine DLL using an absolute System32 path. Arguments and return values are passed through unchanged.

The release build contains no routine logging, status mapping, file creation, virtual controller, worker thread, or C runtime dependency.

## Unsupported builds

If engine identity or original bytes differ, `DLL_PROCESS_ATTACH` returns failure and the proxy does not remain loaded. The release has been tested against a deliberately mismatched synthetic AMD64 engine; Windows reported `ERROR_DLL_INIT_FAILED` (`1114`).

The internal guard is intentionally based on mapped-image identity and exact bytes rather than computing full files on disk while the loader lock is held. Users should still compare the documented full-file SHA-256 hashes before installation.

## Cleanup

On an explicit DLL unload, the proxy first restores the original branch, making the cave unreachable, and then restores the original cave bytes. During normal process termination, Windows discards the private modified pages with the process address space. Neither `WatchDogs2.exe` nor `Disrupt_64.dll` is changed on disk.

## Validation scope

The exact release DLL passed automated architecture, export-table, forwarding, unsupported-build, attach/detach, and patch-byte validation, followed by physical in-game Bluetooth, USB-C, switching, and vibration tests.

The genuine engine was inspected statically but was not loaded as a standalone test DLL because it has an active game-specific entry point and dependency graph. A compact synthetic AMD64 fixture was used for lifecycle tests.
