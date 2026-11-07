# Contributing

Compatibility evidence and narrowly scoped safety fixes are welcome.

## Reports

Use the structured issue forms. Include hashes, controller model, connection type, Windows build, and other input software. Do not upload Ubisoft binaries, game assets, private crash dumps, or files containing personal information.

Storefront labels alone are not sufficient build identifiers. Always provide the executable and engine SHA-256 hashes.

## Code changes

- Do not change patch RVAs, branch bytes, cave instructions, or build guards without fresh read-only analysis of the exact target binary.
- Preserve transparent XInput argument and response forwarding.
- Keep the project offline single-player only; anti-cheat bypass or evasion changes are out of scope.
- Keep documentation and compatibility reports focused on game builds and controller behaviour; omit user-specific launch-flow and unrelated utility names.
- New target builds require independent patch-site verification and physical testing.
- Compile warning-clean and repeat export, forwarding, lifecycle, unsupported-build, and patch-byte validation.

## Licensing

By submitting a contribution, you agree that it may be distributed under this project's [MIT License](LICENSE).

