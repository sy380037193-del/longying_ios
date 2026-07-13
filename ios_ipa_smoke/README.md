# Longying iOS IPA Smoke Build

This folder is the minimal iOS packaging skeleton. It verifies that the app can
start from a tiny encrypted `outres` package without committing the full
Longying resource tree.

## Contents

- `smoke_src/main.lua`: tiny Lua entry script, encrypted as logical `src/main.lua`.
- `smoke_res/smoke.json`: tiny encrypted resource read by the smoke script.
- `outres_smoke/`: generated minimal encrypted resources committed with the skeleton.
- `scripts/generate_smoke_outres.py`: generates `outres_smoke` with Longying encryption.
- `scripts/generate_smoke_outres.ps1`: Windows wrapper for the generator.
- `scripts/ci_build_ipa.sh`: GitHub Actions macOS build script.
- `scripts/patch_project_for_smoke.py`: patches only the temporary CI build copy.

## Local Regeneration

From the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File ios_ipa_smoke\scripts\generate_smoke_outres.ps1
```

The full production resource package can replace this minimal outres later, but
the skeleton intentionally keeps only the smoke package in Git.
