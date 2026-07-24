## 2026-07-13 - Task: Create Longying iOS smoke packaging skeleton
### What was done
- Created an independent Longying iOS packaging skeleton repository for GitHub Actions.
- Added smoke build scripts, GitHub Actions workflow, packaging documentation, and repository ignore/line-ending rules.
- Prepared the skeleton to include only a minimal encrypted `ios_ipa_smoke/outres_smoke` payload, not the full game resources.
- Copied only the native/Cocos build skeleton and Longying encryption tools needed for smoke packaging.
### Testing
- Generated the minimal encrypted `ios_ipa_smoke/outres_smoke` payload successfully.
- Verified the smoke payload contains encrypted `src/main.lua`, encrypted `smoke.json`, and a minimal `asserts`-based `version.json`.
- Ran Python syntax checks for smoke generation, CMake patching, and Longying encryption helper scripts.
- Ran shell syntax validation for the iOS CI build script.
- Verified the smoke CMake patch against a temporary copy of `CMakeLists.txt`.
- Checked the skeleton for forbidden full resource/build directories, AppleDouble files, and files over 95MB.
- Removed the copied Android `proj.android/release` output so packaged `outres` assets from Android do not enter the iOS skeleton.
- Removed the copied Android `proj.android/app/assets` output so Android bundled assets do not enter the iOS skeleton.
- Checked the staged repository contents for forbidden full resource/build artifacts and confirmed the minimal `outres_smoke` files are staged.
### Notes
- `.github/workflows/ios-ipa-smoke.yml`: added the iOS smoke IPA GitHub Actions entry.
- `.gitattributes`: added line-ending and binary-file rules for future commits.
- `.gitignore`: excludes full resources, build outputs, Android assets/release output, local machine files, and generated package artifacts.
- `CMakeLists.txt`, `ios.toolchain.cmake`, `frameworks/`, `tools_new/`: copied the native and tool skeleton needed by the smoke package.
- `ios_ipa_smoke/`: added smoke source, encrypted minimal outres, build scripts, and README; CI excludes Android assets/release output while packaging.
- `docs/ios_ipa_smoke_build.md`: documented the iOS smoke packaging flow.
- Rollback: revert the initial commit in `longying_ios`, or delete the `longying_ios` clone and reclone from the remote before this commit.

## 2026-07-22 - Task: Build an iOS OpenGL ES renderer A/B for the outfit issue
### What was done
- Switched only the iOS Cocos renderer selection from Metal to OpenGL ES 2 while leaving Android, Lua, encrypted `outres`, and packaging inputs unchanged.
- Restored the iOS `CAEAGLLayer`, `EAGLContext`, color framebuffer, and depth/stencil framebuffer lifecycle required by the OpenGL backend.
- Documented that this package is a renderer A/B and requires real-device outfit verification before it is treated as the long-term setting.
### Testing
- Verified the iOS platform macro selects `CC_USE_GLES` and CMake selects the OpenGL backend sources.
- Verified the OpenGL backend includes the iOS GLES2 API mappings and the iOS view binds its default framebuffer before Cocos creates `CommandBufferGL`.
- Ran `git diff --check` after the source edits; the final GitHub Actions and real-device validations remain required.
### Notes
- `frameworks/cocos2d-x/cocos/platform/CCPlatformConfig.h`: selects GLES for iOS while retaining Metal for macOS.
- `frameworks/cocos2d-x/cocos/platform/CCGL.h`: exposes the GLES2 headers and extension aliases required by the shared OpenGL backend on iOS.
- `frameworks/cocos2d-x/cocos/renderer/CMakeLists.txt`: compiles the OpenGL backend sources for iOS.
- `frameworks/cocos2d-x/cocos/platform/ios/CCEAGLView-ios.h`: stores the iOS GLES context and framebuffer state.
- `frameworks/cocos2d-x/cocos/platform/ios/CCEAGLView-ios.mm`: creates, resizes, presents, and destroys the iOS GLES drawable and framebuffer attachments.
- `docs/ios_ipa_smoke_build.md`: records the renderer A/B purpose and acceptance condition.
- `progress.md`: records this task, its static verification, and rollback point.
- Rollback: run `git revert <this-task-commit>` after this task is committed, then push `main` to rebuild the previous Metal IPA.

## 2026-07-22 - Task: Build a Metal skinning diagnostic IPA
### What was done
- Restored the original iOS Metal renderer after the GLES2 test caused all skinned meshes to disappear.
- Added an iOS-only three-mode visual diagnostic that cycles between original Metal GPU skinning, bind-pose vertices, and independently CPU-skinned vertices without changing Lua, C3B, textures, or `outres`.
- Added visible color states and strict input checks so an unsupported CPU diagnostic cannot be mistaken for a valid rendering result.
### Testing
- Verified the restored iOS platform macro selects `CC_USE_METAL`, the iOS view owns a `CAMetalLayer`, and CMake excludes iOS from the OpenGL backend source list.
- Verified the CPU path uses the same four blend weights, blend indices, and three-row matrix palette convention as the built-in skinning shaders, with palette bounds and vertex-stride checks.
- Ran `git diff --check`; GitHub Actions compilation and the documented 16-second real-device recording remain required.
### Notes
- `frameworks/cocos2d-x/cocos/platform/CCGL.h`: removed the temporary iOS GLES2 aliases.
- `frameworks/cocos2d-x/cocos/platform/CCPlatformConfig.h`: restored Metal selection for iOS.
- `frameworks/cocos2d-x/cocos/platform/ios/CCEAGLView-ios.h`: removed temporary GLES framebuffer state.
- `frameworks/cocos2d-x/cocos/platform/ios/CCEAGLView-ios.mm`: restored the Metal layer and drawable resize path.
- `frameworks/cocos2d-x/cocos/renderer/CMakeLists.txt`: restored Metal backend source selection for iOS.
- `frameworks/cocos2d-x/cocos/3d/CCMeshVertexIndexData.cpp`: retains original iOS Metal vertex bytes for the temporary CPU diagnostic.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.h`: stores the temporary CPU-skinned vertex buffer.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: cycles diagnostic modes and computes the independent CPU skinning result.
- `frameworks/cocos2d-x/cocos/renderer/backend/ProgramCache.cpp`: enables the diagnostic shader branch only for iOS Metal skinning programs.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionTexture.vert`: adds the diagnostic GPU-skinning bypass for unlit meshes.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionNormalTexture.vert`: adds the diagnostic GPU-skinning bypass for lit meshes.
- `docs/ios_ipa_smoke_build.md`: documents recording steps, color states, and the result decision table.
- `progress.md`: records this diagnostic task, validation boundary, and rollback.
- Rollback: run `git revert HEAD` from `D:\haizi\longying_ios`, then push `main` to rebuild the preceding GLES A/B package.

## 2026-07-22 - Task: Fix iOS ARM64 Release matrix corruption
### What was done
- Removed the temporary Metal skinning diagnostic and restored the original Metal mesh, shader, and program-state paths.
- Confirmed the working Android reference APK is an ARM64 Debug build compiled with `-O0`, while GitHub Actions builds the iOS IPA as Release.
- Applied the upstream Cocos ARM64 NEON non-optimization fix so modern Apple clang cannot corrupt matrix operations used by the skinning palette.
- Replaced the temporary diagnostic instructions with the permanent native-build fix description.
### Testing
- Verified the seven restored diagnostic source files match the initial iOS skeleton object hashes.
- Verified all ten ARM64 NEON assembly helpers carry the upstream `optnone` attribute.
- Verified the source change matches the NEON64 half of upstream Cocos commit `95319e9100`.
- GitHub Actions Release compilation and IPA artifact validation are required after this change is pushed.
### Notes
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: restored the original mesh draw path and removed temporary CPU skinning modes.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.h`: removed temporary CPU skinning state.
- `frameworks/cocos2d-x/cocos/3d/CCMeshVertexIndexData.cpp`: removed temporary retained Metal vertex bytes.
- `frameworks/cocos2d-x/cocos/math/MathUtilNeon64.inl`: disabled compiler optimization for the ARM64 NEON assembly helpers using the upstream fix.
- `frameworks/cocos2d-x/cocos/renderer/CMakeLists.txt`: restored the original renderer source list.
- `frameworks/cocos2d-x/cocos/renderer/backend/ProgramCache.cpp`: removed the temporary diagnostic shader define.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionNormalTexture.vert`: restored the original lit skinning shader.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionTexture.vert`: restored the original unlit skinning shader.
- `docs/ios_ipa_smoke_build.md`: documented the permanent ARM64 Release math fix and its IPA rebuild boundary.
- `progress.md`: appended this task record without rewriting prior history.
- Rollback: revert this task commit, then push `main` to rebuild the preceding diagnostic package.

## 2026-07-23 - Task: Hide iOS closed-eye meshes by default
### What was done
- Confirmed the persistent face artifact is the head model's closed-eye overlay rather than a dress texture, normal face, or Metal skinning deformation.
- Made Sprite3D hide meshes bound to `bone_biyan` when model data is created; Lua can still show the mesh briefly for the blink animation.
- Applied the same default to normal and attached Sprite3D creation so first loads and cache-backed loads behave consistently.
- Removed the unrelated ARM64 NEON optimization experiment after the device result disproved it.
- Documented the native default and its IPA rebuild boundary.
### Testing
- Parsed `2034.c3b` and rendered `shape4_part1` offline: its two raised eye strips match the persistent artifact in the device screenshots; it contains 36 vertices and 50 triangles.
- Parsed all 270 shape C3B files: 28 meshes use `bone_biyan`, and every match is a dedicated mesh bound only to that bone.
- Verified `2034/2034.c3b` maps `shape4_part1` exclusively to `bone_biyan`; the normal face maps to `Bip01 Head` and `Bip01 Neck`.
- Verified `MathUtilNeon64.inl` has no difference from the initial iOS skeleton revision.
- Ran `git diff --check` successfully. GitHub Actions Release compilation and one final device installation remain required.
### Notes
- `frameworks/cocos2d-x/cocos/3d/CCSprite3D.cpp`: defaults meshes controlled by `bone_biyan` to hidden in both Sprite3D model creation paths.
- `frameworks/cocos2d-x/cocos/math/MathUtilNeon64.inl`: removes the disproved `optnone` experiment and restores the initial skeleton implementation.
- `docs/ios_ipa_smoke_build.md`: replaces the obsolete NEON explanation with the closed-eye mesh behavior and rebuild requirement.
- `progress.md`: appends the evidence, verification boundary, changed-file list, and rollback point for this task.
- Rollback: revert this task commit, then push `main` to rebuild the prior IPA skeleton.

## 2026-07-23 - Task: Lock iOS closed-eye meshes hidden
### What was done
- Confirmed the previous create-time hide was later reversed by `Role3DAni.lua`, which calls `Mesh:setVisible(true)` during head setup and every blink cycle.
- Added a native hidden-visibility lock and applied it only to meshes bound to `bone_biyan` in both Sprite3D model creation paths.
- Kept normal face, dress, body, and texture handling unchanged; the iOS closed-eye overlay animation is intentionally disabled.
### Testing
- Verified the lock is disabled by default for every mesh and enabled only after `hasCloseEyeBone` identifies a `bone_biyan` model mesh.
- Verified `Mesh::setVisible(true)` returns without changing visibility for locked meshes, covering the Lua blink callback that defeated the previous fix.
- Ran `git diff --check` successfully. GitHub Actions Release compilation and final device installation remain required.
### Notes
- `frameworks/cocos2d-x/cocos/3d/CCMesh.h`: adds per-mesh hidden-lock state and the native lock operation.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: initializes the lock off and rejects attempts to show a locked mesh.
- `frameworks/cocos2d-x/cocos/3d/CCSprite3D.cpp`: locks `bone_biyan` meshes hidden in normal and attached model creation paths.
- `docs/ios_ipa_smoke_build.md`: records the permanent-hide behavior, disabled blink overlay, and IPA rebuild boundary.
- `progress.md`: appends the diagnosis, implementation, verification, changed-file list, and rollback point for this task.
- Rollback: revert this task commit, then push `main` to rebuild the previous create-time-hide IPA.

## 2026-07-23 - Task: Fix iOS Sprite3D texture minification
### What was done
- Removed the disproved native lock that permanently hid `bone_biyan` meshes and restored normal Lua blink visibility control.
- Enabled mipmap generation and trilinear minification for Sprite3D diffuse textures only on iOS Metal.
- Completed the Metal sampler mapping so mipmap minification modes select the required Metal mip filter.
- Kept C3B files, texture payloads, Lua behavior, Android code paths, and outfit selection unchanged.
### Testing
- Parsed the actual `2013.c3b` head used by the screenshots and verified the normal face is `shape2_part1`, while the separately hidden `shape4_part1` is only the blink overlay.
- Rendered the normal face mesh offline from its real vertices and UVs and verified the C3B and `batch_head_d_1.png` mapping is structurally correct.
- Verified all three relevant 512x512 head atlases are effectively opaque, ruling out the prior transparent-depth hypothesis.
- Verified the Metal sampler now maps nearest and linear mipmap modes to `MTLSamplerMipFilterNearest` and `MTLSamplerMipFilterLinear` respectively.
- GitHub Actions compilation and final device visual verification remain required.
### Notes
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: removes the failed visibility lock and applies iOS mipmapped filtering at the final diffuse-texture binding point.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.h`: removes the failed hidden-visibility lock API and field.
- `frameworks/cocos2d-x/cocos/3d/CCSprite3D.cpp`: restores normal blink mesh creation without changing model or texture selection.
- `frameworks/cocos2d-x/cocos/renderer/backend/metal/TextureMTL.mm`: maps backend mipmap filters to Metal sampler mip filters.
- `docs/ios_ipa_smoke_build.md`: replaces the disproved closed-eye diagnosis with the Sprite3D minification behavior.
- `progress.md`: appends the implementation, evidence, validation boundary, files, and rollback point.
- Rollback: revert this task commit after it is created, then push `main` to rebuild the preceding IPA.

## 2026-07-23 - Task: Verify iOS Sprite3D minification IPA build
### What was done
- Confirmed GitHub Actions built the iOS Sprite3D minification change successfully from commit `d4e7507`.
- Confirmed the workflow produced the `longying-ios-smoke` artifact for device installation.
### Testing
- GitHub Actions run `29981884611` completed successfully in 4m 52s.
- The uploaded artifact is 87.6 MB with digest `sha256:2df5694ebf263953ee20f68eb122db5e0fb6f38b2a6d2bc129daf99dd506994f`.
- Real-device visual verification remains pending because Windows did not enumerate an Apple USB device and `pymobiledevice3 usbmux list` returned no devices.
### Notes
- `progress.md`: records the successful CI build, artifact identity, and remaining device-verification boundary.
- Rollback: no runtime files changed in this verification task; remove only this appended record if the verification note must be reverted.

## 2026-07-23 - Task: Replace failed iOS outfit experiments with submesh CPU skinning
### What was done
- Removed the disproved iOS Sprite3D mipmap and Metal mip-filter changes while keeping normal blink visibility behavior.
- Added an iOS Metal compatibility path that CPU-skins only vertices referenced by each submesh's own index list and local bone palette.
- Added shader bypasses for the built-in skinned Sprite3D programs; unsupported custom materials and invalid source data retain the original GPU path.
- Kept Lua, C3B, textures, encrypted `outres`, `version.json`, and Android rendering unchanged.
### Testing
- Parsed all 502 C3B files under the full GameClient resource tree: 262 files contain meshes, and all 1,201 skinned submeshes passed vertex-index and local-palette bounds validation with zero errors.
- Verified the dynamic iOS Metal vertex buffers use the engine's existing triple-buffered implementation.
- Verified CPU skinning is enabled only after every active material pass exposes `u_iosCpuSkinning`, preventing custom shaders from applying CPU and GPU skinning twice.
- Ran `git diff --check` before documentation updates; GitHub Actions Release compilation and packaged-executable marker verification remain required.
### Notes
- `frameworks/cocos2d-x/cocos/3d/CCMeshVertexIndexData.h`: allows Mesh to read the retained per-submesh source indices.
- `frameworks/cocos2d-x/cocos/3d/CCMeshVertexIndexData.cpp`: retains original vertex and index data only for Android cache recovery or iOS Metal CPU skinning.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.h`: stores the iOS-only dynamic CPU-skinned vertex buffer.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: performs per-submesh CPU skinning and selects it only for compatible iOS Metal shader passes.
- `frameworks/cocos2d-x/cocos/renderer/backend/ProgramCache.cpp`: enables the CPU-skinning shader switch only in iOS Metal built-in skinning programs.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionTexture.vert`: bypasses GPU position skinning when iOS supplies CPU-skinned vertices.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionNormalTexture.vert`: bypasses GPU position and direction skinning when iOS supplies CPU-skinned vertices.
- `frameworks/cocos2d-x/cocos/renderer/backend/metal/TextureMTL.mm`: removes the disproved custom mip-filter mapping and restores the original Metal sampler behavior.
- `docs/ios_ipa_smoke_build.md`: replaces the disproved mipmap explanation with the iOS CPU-skinning behavior and package marker.
- `progress.md`: appends this implementation, validation evidence, file list, and rollback point.
- Rollback: revert the commit created for this task, then push `main` to rebuild the preceding IPA.

## 2026-07-23 - Task: Add retrievable iOS head-render diagnostics
### What was done
- Removed the disproved iOS CPU-skinning experiment and restored the original GPU skinning shaders and matrix-palette upload.
- Added an iOS-only diagnostic path for head texture binding and first mesh draw state, with both app-writable-file and UDP delivery to the development computer.
- Added raw UV-bound collection without retaining complete source vertex/index buffers, plus a local UDP receiver and the required iOS local-network usage description.
- Kept Lua, C3B files, textures, encrypted `outres`, `version.json`, Android rendering, and the currently reverted mipmap experiment unchanged.
### Testing
- Ran `git diff --check` successfully.
- Verified `CC_IOS_CPU_SKINNING`, `u_iosCpuSkinning`, `updateIosCpuSkinningBuffer`, and `_iosCpuSkinning` no longer occur under the Cocos source tree.
- Ran `python -m py_compile tools_new/receive_ios_head_log.py` successfully.
- Ran `python tools_new/receive_ios_head_log.py --self-test`; it bound `0.0.0.0:39091`, received the loopback payload, and verified the persisted line exactly.
- Parsed the edited iOS `Info.plist` with Python `plistlib` and verified `NSLocalNetworkUsageDescription` is present. GitHub Actions compilation and real-device UDP receipt remain required.
### Notes
- `.gitignore`: excludes locally received iOS diagnostic logs.
- `frameworks/cocos2d-x/cocos/3d/CCIOSHeadRenderDiagnostics.h`: declares the temporary head-path filter, first-draw limiter, and logger.
- `frameworks/cocos2d-x/cocos/3d/CCIOSHeadRenderDiagnostics.cpp`: appends iOS diagnostics locally and sends them to `192.168.1.78:39091` over UDP.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: removes CPU skinning and emits actual head texture/material/draw state.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.h`: removes CPU-skinning buffers and methods.
- `frameworks/cocos2d-x/cocos/3d/CCMeshVertexIndexData.cpp`: restores normal source-data retention and calculates iOS UV bounds during upload.
- `frameworks/cocos2d-x/cocos/3d/CCMeshVertexIndexData.h`: exposes stored UV bounds and removes CPU-skinning access.
- `frameworks/cocos2d-x/cocos/3d/CMakeLists.txt`: registers the diagnostic source and header.
- `frameworks/cocos2d-x/cocos/renderer/backend/ProgramCache.cpp`: restores original built-in skinning shader creation.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionNormalTexture.vert`: removes the iOS CPU-skinning bypass.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionTexture.vert`: removes the iOS CPU-skinning bypass.
- `frameworks/runtime-src/proj.ios_mac/ios/Info.plist`: declares why the diagnostic package requests local-network access.
- `tools_new/receive_ios_head_log.py`: receives, prints, persists, and self-tests the phone UDP diagnostics.
- `docs/ios_ipa_smoke_build.md`: documents the temporary diagnostic workflow and cleanup boundary.
- `progress.md`: appends this implementation, verification evidence, changed-file list, and rollback command.
- Rollback: run `git revert <this-task-commit-sha>` and `git push origin main` to rebuild the prior package.

## 2026-07-23 - Task: Make iOS diagnostics trigger independently of encrypted paths
### What was done
- Confirmed the first diagnostic IPA displayed the abnormal 3D face but emitted no UDP lines and did not request local-network access, proving the clear-text head-path filter never triggered.
- Added an iOS application-launch handshake so the diagnostic sender runs before Lua or encrypted resources load.
- Replaced the clear-text texture-path filter with a first-draw filter for textured, skinned 3D meshes and kept one log per mesh to avoid per-frame flooding.
### Testing
- Confirmed the computer receiver remained bound to `0.0.0.0:39091` while the first diagnostic IPA produced no phone payloads.
- Verified the launch handshake is iOS-only and the broadened draw logger still requires both a skin and a texture.
- GitHub Actions compilation, launch handshake receipt, iOS local-network prompt, and real outfit draw logs remain required.
### Notes
- `frameworks/runtime-src/Classes/AppDelegate.cpp`: sends the iOS diagnostic handshake at application launch.
- `frameworks/cocos2d-x/cocos/3d/CCIOSHeadRenderDiagnostics.h`: removes the obsolete clear-text head-path filter declaration.
- `frameworks/cocos2d-x/cocos/3d/CCIOSHeadRenderDiagnostics.cpp`: removes path filtering support and changes the package marker to diagnostics V2.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: logs first draws for all textured, skinned 3D meshes regardless of encrypted runtime path.
- `docs/ios_ipa_smoke_build.md`: documents the immediate handshake and path-independent mesh trigger.
- `progress.md`: appends the failed-trigger evidence, replacement behavior, verification boundary, and rollback command.
- Rollback: run `git revert <this-task-commit-sha>` and `git push origin main` to restore the V1 path-filtered diagnostic package.

## 2026-07-23 - Task: Fix iOS Metal Sprite3D atlas sampling and final texture binding
### What was done
- Used V2 phone logs to verify the head meshes loaded valid UVs, skins, textures, and independent ProgramStates while reproducing the abnormal face.
- Limited the iOS Metal built-in textured Sprite3D programs to raw C3B V coordinates; Android, OpenGL, 2D UI, particles, and non-textured 3D programs retain their existing behavior.
- Rebound each iOS Mesh diffuse texture to fragment slot 0 immediately before submission and advanced the device marker to diagnostics V3.
### Testing
- Received the V2 launch handshake and 51 first-draw records directly from the phone at `192.168.1.53` through the computer receiver on UDP port `39091`.
- Verified all logged head textures (`batch_head_d_1`, `batch_head_d_2`, and `batch_hat_d_1`) matched their fragment slot-0 backend textures and had valid per-submesh UV bounds.
- Detected two concrete dress mismatches before the fix: `batch_toushi_d_1` drew with `batch_body_d_5`, and `batch_body_t_4` drew with `batch_body_d_7`.
- Compared the current shader sample with the raw-V atlas sample for the affected face; the current flipped V selected a different face region while raw V selected the expected face region.
- GitHub Actions compilation and V3 real-device visual/log verification remain required.
### Notes
- `frameworks/cocos2d-x/cocos/renderer/backend/ProgramCache.cpp`: enables the raw-V shader branch only for iOS Metal textured Sprite3D programs.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionTexture.vert`: preserves raw V for iOS Metal unlit textured 3D meshes.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionNormalTexture.vert`: preserves raw V for iOS Metal lit and normal-mapped textured 3D meshes.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: restores each iOS Mesh diffuse texture at the final pre-submit boundary.
- `frameworks/cocos2d-x/cocos/3d/CCIOSHeadRenderDiagnostics.cpp`: identifies the verification package as diagnostics V3.
- `docs/ios_ipa_smoke_build.md`: documents the iOS-only atlas and final-binding behavior.
- `progress.md`: appends the phone evidence, implementation, verification boundary, changed-file list, and rollback command.
- Rollback: run `git revert <this-task-commit-sha>` and `git push origin main` to restore the V2 evidence-only package.

## 2026-07-23 - Task: Fix iOS V3 package compilation
### What was done
- Corrected the iOS final texture-binding loop to keep each material pass mutable when calling its texture setter.
- Kept the V3 atlas-coordinate and final texture-binding behavior unchanged.
### Testing
- Confirmed the failed Xcode build stopped only because `setUniformTexture` was called through a `const Pass` pointer at `CCMesh.cpp:421`.
- Ran `git diff --check` successfully after the edit.
- GitHub Actions compilation and IPA generation remain required.
### Notes
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: changes the local pass pointer from const to mutable so the existing non-const setter compiles.
- `progress.md`: appends this compile-fix record and verification boundary.
- Rollback: run `git revert <this-task-commit-sha>` and `git push origin main` to restore the compile-failing V3 source.

## 2026-07-23 - Task: Revert disproved iOS raw-V atlas sampling
### What was done
- Used V3 phone logs and screenshots to confirm that raw C3B V coordinates selected incorrect vertical atlas regions across heads and outfits.
- Restored the engine's original `1.0 - y` Sprite3D texture-coordinate conversion on iOS while retaining the verified final per-mesh texture-slot binding.
- Advanced the phone diagnostic marker to V4 so the restored package can be distinguished from V3 without relying on the app version string.
### Testing
- Received the V3 launch marker and 90 first-draw records from the installed phone package.
- Verified all 90 records had matching Mesh backend textures and fragment slot-0 textures, while the accompanying screenshots showed green heads and widespread atlas corruption.
- Verified the iOS-only raw-V macro no longer occurs in ProgramCache or the textured Sprite3D shaders.
- Ran `git diff --check` successfully after the edit.
- GitHub Actions compilation, IPA generation, and V4 real-device visual verification remain required.
### Notes
- `frameworks/cocos2d-x/cocos/renderer/backend/ProgramCache.cpp`: removes the disproved iOS raw-V shader macro injection.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionTexture.vert`: restores unconditional V conversion for unlit textured Sprite3D programs.
- `frameworks/cocos2d-x/cocos/renderer/shaders/3D_positionNormalTexture.vert`: restores unconditional V conversion for lit and normal-mapped textured Sprite3D programs.
- `frameworks/cocos2d-x/cocos/3d/CCIOSHeadRenderDiagnostics.cpp`: identifies the restored verification package as diagnostics V4.
- `docs/ios_ipa_smoke_build.md`: records the V3 device result and V4 restored behavior.
- `progress.md`: appends the device evidence, rollback, and verification boundary.
- Rollback: run `git revert <this-task-commit-sha>` and `git push origin main` to restore the disproved V3 raw-V package.

## 2026-07-23 - Task: Add evidence-only iOS head orientation diagnostics
### What was done
- Used the V4 phone launch and 37 first-draw records to confirm the restored package is active, all head texture slots match, and the original face-only artifact remains.
- Kept rendering behavior unchanged and extended the existing first-draw record with model-view and first skin-palette matrices, their 3x3 determinants, and material cull, winding, and depth state.
- Advanced the package marker to V5 so the evidence-only build cannot be confused with V4.
### Testing
- Verified the V4 phone segment starts at line 307, contains one application launch and 37 first draws, and contains no Mesh-to-slot-0 texture mismatch.
- Ran `git diff --check` successfully after the edit.
- Ran `python tools_new/receive_ios_head_log.py --self-test --port 39092` successfully while the live phone receiver remained bound to port 39091.
- No local C++ compiler is available on this Windows computer; GitHub Actions compilation, V5 marker receipt, and real-device matrix/state evidence remain required.
### Notes
- `frameworks/cocos2d-x/cocos/renderer/CCRenderState.h`: grants Mesh read-only diagnostic access to the fixed-function StateBlock fields without adding a public API.
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: emits transform, skin-palette, cull, winding, and depth evidence in the existing iOS-only first-draw line.
- `frameworks/cocos2d-x/cocos/3d/CCIOSHeadRenderDiagnostics.cpp`: identifies the evidence-only package as diagnostics V5.
- `docs/ios_ipa_smoke_build.md`: documents the V4 device result and the V5 diagnostic boundary.
- `progress.md`: appends this implementation, verification boundary, changed-file list, and rollback command.
- Rollback: run `git revert --no-edit HEAD` and `git push origin main` after this task commit is created.

## 2026-07-23 - Task: Record V5 iOS build verification
### What was done
- Confirmed GitHub Actions built the V5 evidence-only diagnostic IPA successfully from commit `70ee49e`.
- Recorded the successful run and artifact identity without changing code or triggering another package build.
### Testing
- GitHub Actions run `30010821367` completed successfully in 7 minutes 2 seconds.
- The run produced the `longying-ios-smoke` artifact with displayed size 86.9 MB and SHA-256 digest `0e414fb11a8800c01150ab6cab94b401e282a73c8aa95e4a75d4d51f6a071592`.
- V5 marker receipt and real-device matrix/state evidence remain required after the full encrypted `outres` is inserted and the IPA is installed.
### Notes
- `progress.md`: appends the successful V5 GitHub Actions build and remaining real-device verification boundary.
- Rollback: run `git revert --no-edit HEAD` and `git push origin main` after this documentation-only commit is created.


## 2026-07-24 - Task: Package the iOS head-bone animation fix
### What was done
- Added 30 iOS-only corrected dress-stand animation payloads for every supported character shape.
- Changed iOS Animation3D loading so the matching shape dress-stand animation resolves to the bundled corrected payload while Android and every other action keep their existing path.
- Preserved direct loading of user-managed `Documents/URes/codex_headlock_*.c3b` files and added UDP evidence for the requested path, resolved path, cache/file source, and load result.
- Advanced the launch marker to `LONGYING_IOS_HEADLOCK_V6` and made the Action package and validate the exact 30-file C3B payload.
- Did not write, copy, delete, or migrate any file under the phone's `Documents/URes`.
### Testing
- Verified the payload contains exactly 30 unique shape files totaling 5,931,052 bytes; all start with the `C3B\0` header.
- Verified all 30 repository payload SHA-256 hashes match the prepared `phone_headlock_test_20260724_1017` artifacts with zero mismatches.
- Compared every payload with its source `*_dress-stand.c3b`: all 30 retain the exact source byte length and differ by 292 to 398 bytes.
- Ran `git diff --check` successfully.
- Ran `bash -n ios_ipa_smoke/scripts/ci_build_ipa.sh` successfully.
- Verified the native resolver uses `fullPathForDirectory` plus an absolute C3B path, bypassing the client's automatic logical-path hashing.
- Added Action validation for the V6 marker, headlock load event, and payload-directory strings in the packaged executable.
- Verified all 30 logical dress-stand requests map to existing payloads and a non-dress action does not match.
- Compiled the embedded Action Python validation block successfully.
- Ran `python tools_new/receive_ios_head_log.py --self-test --port 39092` successfully.
- GitHub Actions iOS compilation, IPA payload validation, UDP receipt, and real-device visual verification remain required.
### Notes
- `.gitattributes`: treats committed C3B payloads as binary files.
- `frameworks/cocos2d-x/cocos/3d/CCAnimation3D.cpp`: applies the iOS-only dress-stand substitution and reports actual load results.
- `frameworks/cocos2d-x/cocos/3d/CCIOSHeadRenderDiagnostics.cpp`: advances the package marker to V6.
- `ios_ipa_smoke/scripts/ci_build_ipa.sh`: copies and validates the exact headlock payload in the app bundle.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1001.c3b`: adds the corrected shape 1001 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1002.c3b`: adds the corrected shape 1002 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1011.c3b`: adds the corrected shape 1011 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1012.c3b`: adds the corrected shape 1012 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1031.c3b`: adds the corrected shape 1031 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1032.c3b`: adds the corrected shape 1032 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1101.c3b`: adds the corrected shape 1101 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1102.c3b`: adds the corrected shape 1102 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1111.c3b`: adds the corrected shape 1111 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1112.c3b`: adds the corrected shape 1112 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1131.c3b`: adds the corrected shape 1131 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_1132.c3b`: adds the corrected shape 1132 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_2003.c3b`: adds the corrected shape 2003 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_2004.c3b`: adds the corrected shape 2004 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_2013.c3b`: adds the corrected shape 2013 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_2014.c3b`: adds the corrected shape 2014 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_2033.c3b`: adds the corrected shape 2033 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_2034.c3b`: adds the corrected shape 2034 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_3005.c3b`: adds the corrected shape 3005 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_3006.c3b`: adds the corrected shape 3006 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_3015.c3b`: adds the corrected shape 3015 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_3016.c3b`: adds the corrected shape 3016 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_3035.c3b`: adds the corrected shape 3035 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_3036.c3b`: adds the corrected shape 3036 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_4007.c3b`: adds the corrected shape 4007 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_4008.c3b`: adds the corrected shape 4008 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_4017.c3b`: adds the corrected shape 4017 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_4018.c3b`: adds the corrected shape 4018 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_4037.c3b`: adds the corrected shape 4037 dress-stand animation.
- `ios_ipa_smoke/headlock_test_payload/codex_headlock_4038.c3b`: adds the corrected shape 4038 dress-stand animation.
- `ios_ipa_smoke/README.md`: documents the bundled iOS-only payload and its non-URes behavior.
- `docs/ios_ipa_smoke_build.md`: documents the V6 runtime substitution, diagnostics, dependency, and rollback boundary.
- `progress.md`: records implementation, validation evidence, changed files, and rollback.
- Rollback: run `git revert --no-edit <this-task-commit>` and push `main` to rebuild the preceding V5 IPA; separately restore or remove any user-managed `Documents/URes` Lua/C3B overrides because this package does not modify them.


## 2026-07-24 - Task: Fix V6 iOS app-bundle path compilation
### What was done
- Replaced the inaccessible protected FileUtils directory resolver with the iOS executable-directory API.
- Kept the same absolute app-bundle payload lookup, iOS-only dress-stand substitution, and URes boundary.
### Testing
- GitHub Actions run `30064664024` reached `CCAnimation3D.cpp` and failed only because `fullPathForDirectory` is protected.
- Confirmed Xcode reported one compile failure at the protected call and no other V6 compile errors before stopping.
- Ran `git diff --check` successfully after replacing the protected call.
- GitHub Actions recompilation and IPA validation remain required after this fix is pushed.
### Notes
- `frameworks/cocos2d-x/cocos/3d/CCAnimation3D.cpp`: derives the app-bundle directory through `_NSGetExecutablePath` instead of a protected FileUtils method.
- `progress.md`: records the failed Action evidence, correction, validation boundary, and rollback.
- Rollback: revert the compilation-fix commit after it is created; reverting only this fix restores commit `fb48dfa`, which is known not to compile on iOS.

## 2026-07-24 - Task: Route iOS Metal rigid single-bone faces through the model transform
### What was done
- Kept back-face culling enabled after the real-device double-sided test produced long skin-colored triangles through the character.
- Added an iOS Metal-only path for exactly one-bone skinned meshes that reconstructs the 3x4 palette matrix, combines it with the submitted model transform, and uploads an identity skin palette.
- Left multi-bone hair and clothing, Android, Lua, C3B files, textures, and `outres` unchanged.
- Added `rigid_single_bone=1` to the existing first-draw UDP evidence and made IPA validation require that executable marker.
### Testing
- Compared the original shader row-dot calculation with the combined `transform * boneMatrix` calculation for 10,000 points using a real phone palette and transform; maximum absolute error was `2.2737367544323206e-13`.
- Verified the iOS Metal condition is paired, only activates at `iosMatrixPaletteRows == 3`, and the changed Mesh source contains no `setCullFace(false)` call.
- Verified all preprocessor condition blocks in `CCMesh.cpp` are balanced.
- Ran `git diff --check` successfully after all source, build-script, and documentation edits.
- Ran the UDP receiver self-test successfully on `0.0.0.0:39093` without replacing the live receiver on port 39091.
- GitHub Actions Xcode compilation, packaged executable marker validation, UDP receipt, and real-device visual acceptance remain required.
### Notes
- `frameworks/cocos2d-x/cocos/3d/CCMesh.cpp`: submits one-bone iOS Metal meshes through a combined model transform and reports whether that path was used.
- `ios_ipa_smoke/scripts/ci_build_ipa.sh`: rejects an IPA whose executable does not contain the rigid single-bone diagnostic marker.
- `docs/ios_ipa_smoke_build.md`: records the rejected double-sided test, the constrained replacement path, and real-device acceptance criteria.
- `progress.md`: records implementation, validation evidence, changed files, and rollback.
- Rollback: run `git revert --no-edit <this-task-commit>` and push `main` to rebuild commit `1d32018`; do not redeploy the rejected double-sided payload.

## 2026-07-24 - Task: Record the rigid single-bone iOS build verification
### What was done
- Confirmed GitHub Actions built and uploaded the rigid single-bone diagnostic IPA from commit `8db35df83e6e6fc0f6799ab5b558a2e366a01cc2`.
- Kept real-device appearance and UDP evidence as the remaining acceptance boundary.
### Testing
- GitHub Actions run `30078066123` completed with conclusion `success`; its `build` job also completed successfully.
- The successful sequential job confirms Xcode compiled the changed engine source, the IPA validation found the required `rigid_single_bone=` executable marker, and the artifact upload step completed.
- Real-device acceptance still requires a face first-draw line containing `palette_rows=3 rigid_single_bone=1 cull_enabled=1` and visual confirmation that the face is complete and dynamic.
### Notes
- `progress.md`: records the successful Action and the remaining phone-only acceptance check.
- Rollback: revert commit `8db35df` and push `main` to rebuild the preceding `1d32018` source; this documentation-only record can be reverted independently.
