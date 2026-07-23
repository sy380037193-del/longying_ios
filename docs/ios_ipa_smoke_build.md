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

## iOS Closed-Eye Mesh Default

The head models contain a separate skinned mesh for the closed-eye animation.
That mesh is bound to the `bone_biyan` bone and must start hidden; Lua may show
it briefly while playing the blink animation.

The iOS skeleton now hides every mesh bound to `bone_biyan` when Sprite3D data
is created, including the cache-loading path, and locks those meshes hidden.
This prevents Lua's blink callbacks from showing the closed-eye overlay over
the normal face. The iOS trade-off is that this overlay-based blink animation
is disabled; rebuilding the IPA is required because replacing only `outres`
cannot update this native behavior.
