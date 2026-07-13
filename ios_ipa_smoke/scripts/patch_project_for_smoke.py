#!/usr/bin/env python3
import pathlib
import sys


def replace_once(text, old, new, label):
    if old not in text:
        raise SystemExit(f"patch failed, missing marker: {label}")
    return text.replace(old, new, 1)


def main():
    if len(sys.argv) != 2:
        raise SystemExit("usage: patch_project_for_smoke.py <project-root>")

    root = pathlib.Path(sys.argv[1]).resolve()
    cmake_path = root / "CMakeLists.txt"
    cmake = cmake_path.read_text(encoding="utf-8", errors="surrogateescape")

    cmake = replace_once(
        cmake,
        "if(APPLE OR VS)",
        "if(VS OR (APPLE AND MACOSX))",
        "disable plain src/res resources for iOS",
    )

    cmake = replace_once(
        cmake,
        'set(res_src_folders\n    "${CMAKE_CURRENT_SOURCE_DIR}/src"\n    )',
        'set(res_src_folders\n    "${CMAKE_CURRENT_SOURCE_DIR}/src"\n    )\n'
        'set(ios_smoke_outres_folder\n'
        '    "${CMAKE_CURRENT_SOURCE_DIR}/outres"\n'
        '    )',
        "add smoke outres variable",
    )

    cmake = replace_once(
        cmake,
        '        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "iPhone Developer")\n    endif()',
        '        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "iPhone Developer")\n'
        '        add_custom_command(TARGET ${APP_NAME} POST_BUILD\n'
        '            COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_BUNDLE_DIR:${APP_NAME}>/outres"\n'
        '            COMMAND ${CMAKE_COMMAND} -E copy_directory "${ios_smoke_outres_folder}" "$<TARGET_BUNDLE_DIR:${APP_NAME}>/outres"\n'
        '            COMMENT "Copy smoke encrypted outres into iOS bundle"\n'
        '        )\n'
        '    endif()',
        "copy smoke outres into iOS bundle",
    )

    cmake_path.write_text(cmake, encoding="utf-8", errors="surrogateescape")


if __name__ == "__main__":
    main()
