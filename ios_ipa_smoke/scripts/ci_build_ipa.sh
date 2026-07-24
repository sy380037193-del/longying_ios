#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
WORK_DIR="${RUNNER_TEMP:-/tmp}/longying_ios_smoke_work"
SMOKE_DIR="$ROOT_DIR/ios_ipa_smoke"
APP_NAME="dhxy"
IOS_BUNDLE_ID="${IOS_BUNDLE_ID:-com.longying.dhxy}"
IPA_NAME="${IPA_NAME:-longying-ios-smoke}"

rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR"

rsync -a \
  --exclude ".git" \
  --exclude "build_ios" \
  --exclude "resources" \
  --exclude "outres" \
  --exclude "outres_pngquant" \
  --exclude "outres*.zip" \
  --exclude "ios_ipa_smoke/artifacts" \
  --exclude "frameworks/runtime-src/proj.android/app/assets" \
  --exclude "frameworks/runtime-src/proj.android/app/build" \
  --exclude "frameworks/runtime-src/proj.android/app/.cxx" \
  --exclude "frameworks/runtime-src/proj.android/.gradle" \
  --exclude "frameworks/runtime-src/proj.android/release" \
  "$ROOT_DIR/" "$WORK_DIR/project/"

cd "$WORK_DIR/project"

rm -rf outres src res
cp -R "$SMOKE_DIR/outres_smoke" outres
mkdir -p src res

python3 "$SMOKE_DIR/scripts/patch_project_for_smoke.py" "$PWD"

cmake -S . -B build_ios_ci -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphoneos \
  -DCMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET=12.0

if [[ "${IOS_ALLOW_UNSIGNED:-1}" == "1" ]]; then
  CODE_SIGNING_ARGS=(
    CODE_SIGNING_ALLOWED=NO
    CODE_SIGNING_REQUIRED=NO
    CODE_SIGN_IDENTITY=
  )
else
  CODE_SIGNING_ARGS=()
fi

xcodebuild \
  -project "build_ios_ci/${APP_NAME}.xcodeproj" \
  -target "$APP_NAME" \
  -configuration Release \
  -sdk iphoneos \
  SYMROOT="$WORK_DIR/xcode-build" \
  OBJROOT="$WORK_DIR/xcode-obj" \
  PRODUCT_BUNDLE_IDENTIFIER="$IOS_BUNDLE_ID" \
  DEBUG_INFORMATION_FORMAT=dwarf-with-dsym \
  GCC_GENERATE_DEBUGGING_SYMBOLS=YES \
  COPY_PHASE_STRIP=NO \
  STRIP_INSTALLED_PRODUCT=NO \
  DEPLOYMENT_POSTPROCESSING=NO \
  "${CODE_SIGNING_ARGS[@]}" \
  build

APP_PATH=""
for candidate in \
  "$PWD/build_ios_ci/bin/${APP_NAME}/Release/${APP_NAME}.app" \
  "$PWD/build_ios_ci/bin/${APP_NAME}/Release-iphoneos/${APP_NAME}.app" \
  "$WORK_DIR/xcode-build/Release-iphoneos/${APP_NAME}.app" \
  "$WORK_DIR/xcode-build/Release/${APP_NAME}.app"; do
  if [[ -d "$candidate" ]]; then
    APP_PATH="$candidate"
    break
  fi
done

if [[ -z "$APP_PATH" ]]; then
  APP_PATH="$(find "$PWD/build_ios_ci" "$WORK_DIR/xcode-build" -name "${APP_NAME}.app" -type d | head -n 1)"
fi

if [[ -z "$APP_PATH" ]]; then
  echo "Could not find built .app" >&2
  find "$PWD/build_ios_ci" "$WORK_DIR/xcode-build" -maxdepth 6 -type d -name "*.app" -print || true
  exit 1
fi

PAYLOAD_DIR="$WORK_DIR/ipa/Payload"
rm -rf "$WORK_DIR/ipa"
mkdir -p "$PAYLOAD_DIR"
cp -R "$APP_PATH" "$PAYLOAD_DIR/"

APP_BUNDLE="$PAYLOAD_DIR/${APP_NAME}.app"
HEADLOCK_PAYLOAD_SOURCE="$SMOKE_DIR/headlock_test_payload"

if [[ ! -d "$HEADLOCK_PAYLOAD_SOURCE" ]]; then
  echo "Missing iOS headlock payload: $HEADLOCK_PAYLOAD_SOURCE" >&2
  exit 1
fi

rm -rf "$APP_BUNDLE/headlock_test_payload"
cp -R "$HEADLOCK_PAYLOAD_SOURCE" "$APP_BUNDLE/headlock_test_payload"

python3 - "$APP_BUNDLE" "$IOS_BUNDLE_ID" <<'PY'
import json
import pathlib
import plistlib
import sys

app_bundle = pathlib.Path(sys.argv[1])
expected_bundle_id = sys.argv[2]
info_path = app_bundle / "Info.plist"
errors = []

if not app_bundle.is_dir():
    errors.append(f"missing app bundle: {app_bundle}")

if not info_path.is_file():
    errors.append("missing Info.plist")
    info = {}
else:
    with info_path.open("rb") as handle:
        info = plistlib.load(handle)

required_info_keys = [
    "CFBundleExecutable",
    "CFBundleIdentifier",
    "CFBundleInfoDictionaryVersion",
    "CFBundleName",
    "CFBundlePackageType",
    "CFBundleShortVersionString",
    "CFBundleVersion",
    "LSRequiresIPhoneOS",
]
for key in required_info_keys:
    value = info.get(key)
    if value in (None, ""):
        errors.append(f"Info.plist missing required value: {key}")
    if isinstance(value, str) and ("$(" in value or "${" in value):
        errors.append(f"Info.plist has unresolved build setting in {key}: {value}")

if info.get("CFBundleIdentifier") != expected_bundle_id:
    errors.append(
        f"CFBundleIdentifier is {info.get('CFBundleIdentifier')!r}, "
        f"expected {expected_bundle_id!r}"
    )

if info.get("CFBundlePackageType") != "APPL":
    errors.append(f"CFBundlePackageType is {info.get('CFBundlePackageType')!r}, expected 'APPL'")

executable = info.get("CFBundleExecutable")
executable_path = app_bundle / executable if executable else None
if executable_path and not executable_path.is_file():
    errors.append(f"missing CFBundleExecutable file: {executable}")
elif executable_path:
    executable_data = executable_path.read_bytes()
    for marker in [
        b"LONGYING_IOS_HEADLOCK_V6",
        b"event=ios_headlock_animation",
        b"headlock_test_payload",
    ]:
        if marker not in executable_data:
            errors.append(f"missing iOS headlock executable marker: {marker!r}")

outres = app_bundle / "outres"
version_path = outres / "version.json"
if not outres.is_dir():
    errors.append("missing outres directory")
elif not version_path.is_file():
    errors.append("missing outres/version.json")
else:
    version = json.loads(version_path.read_text(encoding="utf-8"))
    assets = version.get("asserts") or {}
    if len(assets) < 2:
        errors.append("minimal outres should contain at least two asserts")
    for rel in assets:
        asset = outres / rel
        if not asset.is_file():
            errors.append(f"missing outres asset: {rel}")
            continue
        data = asset.read_bytes()
        if rel.endswith(".lua") and b"longying-ios-smoke" in data:
            errors.append(f"lua smoke asset appears to be plain text: {rel}")
        if rel.endswith(".json") and data.lstrip().startswith(b"{"):
            errors.append(f"json smoke asset appears to be plain text: {rel}")

if (app_bundle / "src").exists():
    errors.append("plain src directory should not be bundled in smoke IPA")
if (app_bundle / "res").exists():
    errors.append("plain res directory should not be bundled in smoke IPA")

headlock_shape_ids = [
    1001, 1002, 1011, 1012, 1031, 1032,
    1101, 1102, 1111, 1112, 1131, 1132,
    2003, 2004, 2013, 2014, 2033, 2034,
    3005, 3006, 3015, 3016, 3035, 3036,
    4007, 4008, 4017, 4018, 4037, 4038,
]
headlock_dir = app_bundle / "headlock_test_payload"
expected_headlock_files = {
    f"codex_headlock_{shape_id}.c3b" for shape_id in headlock_shape_ids
}
actual_headlock_files = {
    path.name for path in headlock_dir.iterdir() if path.is_file()
} if headlock_dir.is_dir() else set()
if actual_headlock_files != expected_headlock_files:
    errors.append(
        "iOS headlock payload mismatch: "
        f"missing={sorted(expected_headlock_files - actual_headlock_files)}, "
        f"extra={sorted(actual_headlock_files - expected_headlock_files)}"
    )
for filename in sorted(expected_headlock_files & actual_headlock_files):
    data = (headlock_dir / filename).read_bytes()
    if not data.startswith(b"C3B\0"):
        errors.append(f"invalid iOS headlock C3B: {filename}")

if not (app_bundle / "_CodeSignature").exists():
    print("warning: app bundle is unsigned; this is expected for the smoke build")
if not (app_bundle / "embedded.mobileprovision").exists():
    print("warning: embedded.mobileprovision is absent; this is expected for the smoke build")

if errors:
    for error in errors:
        print(f"ERROR: {error}", file=sys.stderr)
    raise SystemExit(1)

print(f"Validated app bundle: {app_bundle}")
print(f"Bundle ID: {info.get('CFBundleIdentifier')}")
print(f"Executable: {info.get('CFBundleExecutable')}")
PY

DSYM_DIR="$WORK_DIR/dsyms"
rm -rf "$DSYM_DIR"
mkdir -p "$DSYM_DIR"

while IFS= read -r dsym_path; do
  cp -R "$dsym_path" "$DSYM_DIR/"
done < <(find "$PWD/build_ios_ci" "$WORK_DIR/xcode-build" -name "*.dSYM" -type d 2>/dev/null || true)

if ! find "$DSYM_DIR" -name "*.dSYM" -type d | grep -q .; then
  EXECUTABLE_PATH="$APP_BUNDLE/${APP_NAME}"
  if [[ -f "$EXECUTABLE_PATH" ]]; then
    dsymutil "$EXECUTABLE_PATH" -o "$DSYM_DIR/${APP_NAME}.app.dSYM"
  fi
fi

if ! find "$DSYM_DIR" -name "*.dSYM" -type d | grep -q .; then
  echo "Could not generate dSYM for ${APP_NAME}" >&2
  exit 1
fi

mkdir -p "$ROOT_DIR/ios_ipa_smoke/artifacts"
DSYM_ZIP_PATH="$ROOT_DIR/ios_ipa_smoke/artifacts/${IPA_NAME}-dSYM.zip"
rm -f "$DSYM_ZIP_PATH"
(cd "$DSYM_DIR" && zip -qry "$DSYM_ZIP_PATH" .)

IPA_PATH="$ROOT_DIR/ios_ipa_smoke/artifacts/${IPA_NAME}.ipa"
rm -f "$IPA_PATH"
(cd "$WORK_DIR/ipa" && zip -qry "$IPA_PATH" Payload)

echo "IPA: $IPA_PATH"
echo "dSYM: $DSYM_ZIP_PATH"
