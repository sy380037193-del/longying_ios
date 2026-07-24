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
7. Add and validate the 30-file iOS headlock payload beside `outres`.
8. Validate the app bundle and minimal `outres`.
9. Package `Payload/dhxy.app` into an unsigned IPA.
10. Upload the IPA and dSYM as GitHub Actions artifacts.

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

The iOS package contains a temporary diagnostic channel for the outfit
face-rendering investigation. The channel does not change Lua, textures,
`outres`, `version.json`, or the Android rendering path. V6 additionally
bundles the iOS-only corrected C3B animations documented below. The failed iOS
CPU-skinning experiment has been removed.

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

On first launch, the app immediately sends an `app_launch` handshake so iOS
can request local-network access. Tap **Allow**, enter the game, equip an
outfit, and display the affected character. The package then logs the first
draw of every textured, skinned 3D mesh instead of relying on a clear-text
resource path. Each line includes the actual texture/backend objects, material
and program-state objects, palette size, vertex stride, index count, and raw
per-submesh UV bounds. The same lines are also appended to
`longying_head_render.log` under the app writable path.

Validate the computer-side receiver without a phone with:

```powershell
python tools_new/receive_ios_head_log.py --self-test
```

This diagnostic code and the `NSLocalNetworkUsageDescription` entry are
temporary and must be removed after the device evidence identifies the final
rendering fix.

## iOS Metal Sprite3D Texture Compatibility

The V3 real-device test disproved the iOS raw-V experiment: it selected the
wrong vertical regions across head and dress atlases, producing green heads
and widespread outfit corruption. V4 restores the engine's original
`1.0 - y` behavior for every textured Sprite3D program.

Immediately before an iOS Mesh is submitted, its own diffuse backend texture
is also rebound to slot 0 of every active material pass. This prevents a
submesh from drawing with a different dress texture left in its ProgramState.
The V4 diagnostic package records both the Mesh backend texture and the
fragment slot-0 backend texture after the final rebind.

## V5 iOS Head Orientation Diagnostics

The V4 real-device test restored the original face-only artifact and confirmed
that every logged Mesh diffuse texture matched fragment slot 0. V5 does not
change rendering. Its first-draw record additionally includes the model-view
matrix and determinant, the first skin-palette matrix and determinant, and the
material cull, winding, and depth state. These fields distinguish a mirrored or
reversed head transform from a skin-palette or fixed-function state problem
without cycling visual rendering experiments.

## V6 iOS Head-Bone Animation Fix

The V6 package contains corrected dress-stand animations for all 30 supported
shape IDs under `headlock_test_payload` in the app bundle. On iOS only,
`Animation3D::create` replaces a matching
`3d/c3b/shape/<shape>/<shape>_dress-stand.c3b` request with the bundled
`codex_headlock_<shape>.c3b`. Other actions and Android keep their original
paths.

The corrected animations retain neck and body movement while preventing the
`Bip01 Head` keys from driving the detached head model into the mismatched face
pose. They are bundled outside `outres`, so inserting the full production
`outres` does not remove them. The package does not write or copy files into
`Documents/URes`.

The UDP stream starts with
`marker=LONGYING_IOS_HEADLOCK_V6`. When the fix is selected it also emits
`event=ios_headlock_animation` with the requested path, resolved path,
file/cache source, and success result. A stale user-managed Lua override that
stops all head actions must still be removed or replaced; an animation payload
cannot move a Sprite3D action that Lua has explicitly stopped.
