#!/usr/bin/env python3
import hashlib
import json
import os
import pathlib
import shutil
import subprocess
import sys
import time


KEY = "b91f4c4c00c92d562181b58d6ecc7491"
IV = "5e33f4430966a3cd1592d6b1bdc7d32b"


def fnv1a_64(filename):
    hash_value = 14695981039346656037
    for char in filename:
        hash_value ^= ord(char)
        hash_value *= 1099511628211
    return hash_value & 0xFFFFFFFFFFFFFFFF


def hash_to_string(hash_value):
    return f"{hash_value:016x}"


def run_checked(command, env):
    subprocess.run(command, env=env, check=True)


def encrypt_file(encoder, in_path, out_path, env):
    run_checked(
        [
            str(encoder),
            "-k",
            KEY,
            "-v",
            IV,
            "-i",
            str(in_path),
            "-o",
            str(out_path),
        ],
        env,
    )
    if not out_path.is_file():
        raise SystemExit(f"encrypt failed: {in_path}")


def file_md5_and_size(path):
    data = path.read_bytes()
    return hashlib.md5(data).hexdigest(), len(data)


def main():
    repo_root = pathlib.Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else pathlib.Path(__file__).resolve().parents[2]
    smoke_root = repo_root / "ios_ipa_smoke"
    tools_root = repo_root / "tools_new"
    luajit = tools_root / "win32" / "64" / "luajit.exe"
    encoder = tools_root / "win32" / "encoder.exe"
    tmp_file = smoke_root / ".tmp" / "000luabytecode.tmp"
    outres_root = smoke_root / "outres_smoke"

    for required in (luajit, encoder):
        if not required.is_file():
            raise SystemExit(f"missing required tool: {required}")

    if outres_root.exists():
        shutil.rmtree(outres_root)
    outres_root.mkdir(parents=True)
    tmp_file.parent.mkdir(parents=True, exist_ok=True)

    env = os.environ.copy()
    env["LUA_PATH"] = str(tools_root / "?.lua")

    assets = [
        ("src/main.lua", smoke_root / "smoke_src" / "main.lua", True),
        ("smoke.json", smoke_root / "smoke_res" / "smoke.json", False),
    ]
    records = {}

    for logical_name, source_path, is_lua in assets:
        if not source_path.is_file():
            raise SystemExit(f"missing smoke source: {source_path}")

        hash_str = hash_to_string(fnv1a_64(logical_name))
        extension = pathlib.Path(logical_name).suffix
        sub_dir = hash_str[int(hash_str[0], 16)]
        target_dir = outres_root / sub_dir
        target_dir.mkdir(parents=True, exist_ok=True)
        target_path = target_dir / f"{hash_str}{extension}"

        if is_lua:
            run_checked([str(luajit), "-b", str(source_path), str(tmp_file)], env)
            encrypt_file(encoder, tmp_file, target_path, env)
            tmp_file.unlink(missing_ok=True)
        else:
            encrypt_file(encoder, source_path, target_path, env)

        rel_path = target_path.relative_to(outres_root).as_posix()
        records[rel_path] = list(file_md5_and_size(target_path))
        print(f"{logical_name} -> {rel_path}")

    version = {
        "EngineVersion": "8.8.8",
        "GameVersion": time.strftime("%y.%m.%d%H%M"),
        "packages": ["game"],
        "asserts": records,
    }
    version_path = outres_root / "version.json"
    version_path.write_text(json.dumps(version, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"Generated {version_path}")


if __name__ == "__main__":
    main()
