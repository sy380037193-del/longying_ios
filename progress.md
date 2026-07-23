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
