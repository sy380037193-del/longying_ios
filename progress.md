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
