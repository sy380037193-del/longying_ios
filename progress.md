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
