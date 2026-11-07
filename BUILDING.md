# Building

## Toolchain

- Windows x64
- Zig 0.17.1
- Zig's `x86_64-windows-gnu` headers and import libraries

No third-party source library is copied into or statically linked with the proxy. The release imports selected `KERNEL32.dll` functions and dynamically loads `%SystemRoot%\System32\xinput9_1_0.dll` by absolute path.

## Commands

Run from the repository root in PowerShell:

```powershell
$zig = 'C:\path\to\zig.exe'

& $zig cc -target x86_64-windows-gnu -O2 -g0 -Wall -Wextra -Werror `
  -c -o release-proxy.obj .\src\proxy.c

& $zig cc -target x86_64-windows-gnu -nostdlib -shared -g0 `
  '-Wl,--entry,DllMain' '-Wl,/Brepro' `
  -o .\xinput9_1_0.dll `
  .\release-proxy.obj .\src\xinput9_1_0.def -lkernel32
```

The linker may also create an import library and PDB. They are development artifacts and are not needed by players.

## Required exports

| Ordinal | Export |
|---:|---|
| 1 | `DllMain` |
| 2 | `XInputGetCapabilities` |
| 3 | `XInputGetDSoundAudioDeviceGuids` |
| 4 | `XInputGetState` |
| 5 | `XInputSetState` |

## Reproducibility note

The commands reproducibly build the implementation and export surface with the pinned toolchain. Zig/LLD can include path-sensitive CodeView or build metadata, so a different checkout path is not guaranteed to produce the physically tested SHA-256 even when generated machine code is equivalent.

The frozen tested DLL has SHA-256:

`F4112AFAB93E3A9200E2F442286A80EF76EA5BB0552CE9F0A68F2E004BED38CC`

Do not replace a published tested binary with a local rebuild without repeating architecture, export, forwarding, unsupported-build, patch-byte, lifecycle, and physical in-game validation.
