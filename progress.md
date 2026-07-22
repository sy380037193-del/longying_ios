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
