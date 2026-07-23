# Longying iOS IPA Smoke Build

This repository is a packaging skeleton. It keeps the native iOS/Cocos build
tree, CI scripts, Longying encryption tools, and a minimal encrypted `outres`
package. It intentionally does not track the full `resources`, full `outres`,
APKs, IPAs, or local build outputs.

## GitHub Actions Flow

The workflow at `.github/workflows/ios-ipa-smoke.yml` runs on `push` to `main`
and by manual `workflow_dispatch`.

The macOS runner performs these steps:

1. Checkout this skeleton repository.
2. Copy the repository to a temporary work directory.
3. Copy `ios_ipa_smoke/outres_smoke` into the temporary project as `outres`.
4. Patch only the temporary `CMakeLists.txt` so iOS bundles encrypted `outres`
   and does not bundle plain `src` or `res`.
5. Generate the Xcode project with CMake.
6. Build `dhxy.app` with `xcodebuild`.
7. Validate the app bundle and minimal `outres`.
8. Package `Payload/dhxy.app` into an unsigned IPA.
9. Upload the IPA and dSYM as GitHub Actions artifacts.

## Minimal Outres

The committed smoke package is generated from:

```text
ios_ipa_smoke/smoke_src/main.lua
ios_ipa_smoke/smoke_res/smoke.json
```

The output is:

```text
ios_ipa_smoke/outres_smoke/
```

Regenerate it from the repository root on Windows:

```powershell
powershell -ExecutionPolicy Bypass -File ios_ipa_smoke\scripts\generate_smoke_outres.ps1
```

The generator uses Longying's `tools_new/hash_encrypt.py` FNV path convention
and the AES key/IV used by `tools_new/win32/encoder.exe`.

## Full Resources Later

When a full iOS resource package is ready, replace the CI resource preparation
step or replace `ios_ipa_smoke/outres_smoke` with the full encrypted package.
Do not commit local Android build caches, APKs, IPAs, `build_ios`, full
`resources`, generated `outres`, or generated ZIP packages into this skeleton
repository.

## Temporary iOS Head Render Diagnostics

The iOS package currently contains a temporary diagnostic channel for the
outfit face-rendering investigation. It does not change Lua, C3B files,
textures, `outres`, `version.json`, or the Android rendering path. The failed
iOS CPU-skinning experiment has been removed.

Before launching the diagnostic IPA, start the receiver from the repository
root on the development computer:

```powershell
python tools_new/receive_ios_head_log.py
```

The receiver listens on `0.0.0.0:39091` and appends payloads to
`ios_head_logs/longying_head_render.log`. The iOS sender currently targets the
development computer at `192.168.1.78:39091`. If that LAN address changes,
update `RECEIVER_IP` in
`frameworks/cocos2d-x/cocos/3d/CCIOSHeadRenderDiagnostics.cpp` and rebuild.

On first launch, iOS may ask for local-network access. Tap **Allow**, enter the
game, equip an outfit, and display the affected character. The package logs
only meshes whose texture path contains `3d/c3btex/shape/`. Each matching
texture binding and first mesh draw includes the actual texture/backend
objects, material and program-state objects, palette size, vertex stride,
index count, and raw UV bounds. The same lines are also appended to
`longying_head_render.log` under the app writable path.

Validate the computer-side receiver without a phone with:

```powershell
python tools_new/receive_ios_head_log.py --self-test
```

This diagnostic code and the `NSLocalNetworkUsageDescription` entry are
temporary and must be removed after the device evidence identifies the final
rendering fix.
