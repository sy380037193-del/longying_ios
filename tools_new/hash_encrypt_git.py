import os
import sys
import shutil
import subprocess
import hashlib
import json

# 配置区
scriptRoot = os.path.split(os.path.realpath(__file__))[0]
projRoot = os.path.split(scriptRoot)[0]
excluded_files = ["version.json"]
# 可自由选择要处理的目录（res/src）
target_dirs = ["src"]  # 示例：["res", "src"]
# version.json路径（使用原有全量的version.json，需确保路径正确）
version_json_path = os.path.join(projRoot, "outres", "version.json")

# 全局变量
cmd_env = os.environ.copy()
cmd_env['LUA_PATH'] = os.path.join(scriptRoot, "?.lua")
tmpFile = os.path.join(scriptRoot, "000luabytecode.tmp")
# 记录新生成的outres文件（绝对路径: 相对outres的路径）
new_outres_files = {}

# -------------------------- 原有核心函数 --------------------------
def checkFileExt(path):
    binExt = ['.json', '.plist', '.png', '.ogg', '.atlas', '.jpg', '.csb', '.lua', '.mt', '.mb', '.ttf', '.mp3', '.lua-bak']
    ext = os.path.splitext(path)[1].lower()
    return ext in binExt

def fnv1a_64(filename):
    FNV_prime = 1099511628211
    offset_basis = 14695981039346656037
    hash_val = offset_basis
    for c in filename:
        hash_val ^= ord(c)
        hash_val *= FNV_prime
    return hash_val & 0xffffffffffffffff

def hash_to_string(hash_val):
    return "{:016x}".format(hash_val)

def byte_code(jitPath, path):
    jitcmd = f'{jitPath} -b "{path}" "{tmpFile}"'
    cmd = subprocess.Popen(jitcmd, shell=True, stdout=subprocess.PIPE, env=cmd_env)
    cmd.wait()
    if not os.path.exists(tmpFile):
        print(f"Error: 编译失败:{path}")
        sys.exit(-1)

def encrypt_file(encPath, inpath, outpath):
    enccmd = f'{encPath} -k b91f4c4c00c92d562181b58d6ecc7491 -v 5e33f4430966a3cd1592d6b1bdc7d32b -i "{inpath}" -o "{outpath}"'
    cmd = subprocess.Popen(enccmd, shell=True, stdout=subprocess.PIPE, env=cmd_env)
    cmd.wait()
    if not os.path.exists(outpath):
        print(f"Error: 加密失败:{inpath}")
        sys.exit(-1)

# -------------------------- Git相关函数 --------------------------
def get_git_root_dir(target_dir):
    """获取指定目录的Git根目录"""
    try:
        git_root = subprocess.check_output(
            ["git", "rev-parse", "--show-toplevel"],
            cwd=target_dir,
            stderr=subprocess.STDOUT,
            text=True
        ).strip()
        return git_root
    except subprocess.CalledProcessError as e:
        print(f"Error: 目录 {target_dir} 不是Git仓库：{e.output.strip()}")
        sys.exit(-1)
    except FileNotFoundError:
        print("Error: 未找到Git，请确保Git已安装并添加到环境变量中。")
        sys.exit(-1)

def get_git_untracked_files(target_dir):
    """获取指定Git仓库目录下的未提交文件列表"""
    try:
        result = subprocess.check_output(
            ["git", "status", "--porcelain", "--untracked-files=all"],
            cwd=target_dir,
            stderr=subprocess.STDOUT,
            text=True
        )
    except subprocess.CalledProcessError as e:
        print(f"Error: 执行Git命令失败: {e.output.strip()}")
        sys.exit(-1)

    untracked_files = []
    lines = result.strip().split('\n')
    for line in lines:
        if not line:
            continue
        parts = line.split(maxsplit=2)
        status = parts[0]
        file_path = parts[1]

        # 处理重命名（取新路径）
        if status.startswith('R') and len(parts) == 3:
            file_path = parts[2]

        # 拼接绝对路径并过滤文件
        abs_path = os.path.abspath(os.path.join(target_dir, file_path))
        if os.path.isfile(abs_path):
            untracked_files.append(abs_path)
    return untracked_files

# -------------------------- MD5和版本更新函数 --------------------------
def get_file_md5_and_size(file_path):
    """计算文件的MD5值和大小"""
    md5 = hashlib.md5()
    with open(file_path, 'rb') as f:
        md5.update(f.read())
    return md5.hexdigest(), os.path.getsize(file_path)

def increment_version(version_str):
    """版本号自增（仅最后一位+1，如1.0.2 → 1.0.3）"""
    try:
        version_parts = list(map(int, version_str.split('.')))
        version_parts[-1] += 1
        return '.'.join(map(str, version_parts))
    except Exception as e:
        print(f"警告：版本号格式错误({version_str})，使用默认版本号1.0.1")
        return "1.0.1"

def update_version_json():
    """更新原有version.json：替换新文件的MD5，版本号+1"""
    # 1. 读取原有version.json
    if not os.path.exists(version_json_path):
        print(f"Error: 未找到原有version.json文件：{version_json_path}")
        sys.exit(-1)

    with open(version_json_path, 'r', encoding='utf-8') as f:
        version_data = json.load(f)

    # 2. 版本号自增
    if "GameVersion" in version_data:
        version_data["GameVersion"] = increment_version(version_data["GameVersion"])
    else:
        version_data["GameVersion"] = "1.0.1"
        print("警告：version.json中未找到GameVersion，初始化为1.0.1")

    # 3. 遍历新生成的文件，更新MD5和大小
    for abs_path, rel_path in new_outres_files.items():
        md5, size = get_file_md5_and_size(abs_path)
        # 替换/新增条目（rel_path作为key）
        version_data["asserts"][rel_path] = [md5, size]
        print(f"更新version.json：{rel_path} → MD5: {md5}")

    # 4. 保存更新后的version.json
    with open(version_json_path, 'w', encoding='utf-8') as f:
        json.dump(version_data, f, indent=2, ensure_ascii=False)

    print(f"\n成功更新version.json：{version_json_path}")
    print(f"新版本号：{version_data['GameVersion']}")
    print(f"更新文件数量：{len(new_outres_files)}")

# -------------------------- 主处理逻辑 --------------------------
if __name__ == "__main__":
    # 1. 初始化路径
    jitPath = os.path.join(scriptRoot, "win32", "64", "luajit.exe")
    encPath = os.path.join(scriptRoot, "win32", "encoder.exe")
    dest_root = os.path.join(projRoot, "outres")
    os.makedirs(dest_root, exist_ok=True)

    all_records = {}
    has_err = False
    count = 0

    # 2. 获取Git未提交文件
    res_root = os.path.join(projRoot, "resources")
    git_untracked_files = {}
    total_files = 0

    for dir_name in target_dirs:
        target_dir = os.path.join(res_root, dir_name)
        git_root = get_git_root_dir(target_dir)
        print(f"检测到【{dir_name}】的Git仓库根目录：{git_root}")
        
        files = get_git_untracked_files(git_root)
        git_untracked_files[dir_name] = files
        total_files += len(files)

    if total_files == 0:
        print(f"【{','.join(target_dirs)}】目录下没有Git未提交的文件，任务结束")
        sys.exit(0)
    print(f"找到 {total_files} 个Git未提交的文件\n")

    # 3. 处理文件并生成到outres
    with open(os.path.join(scriptRoot, "collision_log.txt"), "w") as fp:
        for dir_name in target_dirs:
            source_directory = os.path.join(res_root, dir_name)
            files = git_untracked_files.get(dir_name, [])

            for filepath in files:
                filename = os.path.basename(filepath)
                # 排除指定文件
                if filename in excluded_files:
                    print(f"排除文件：{filename}")
                    continue

                # 计算相对路径和哈希
                relative_path = os.path.relpath(filepath, source_directory).replace("\\", "/")
                _, file_ext = os.path.splitext(relative_path)
                prefix = "" if dir_name == "res" else f"{dir_name}/"
                hashed_value = fnv1a_64(prefix + relative_path)
                hash_str = hash_to_string(hashed_value)

                # 构建目标路径
                sub_dir_index = int(hash_str[0], 16)
                sub_directory = hash_str[sub_dir_index]
                target_sub_directory = os.path.join(dest_root, sub_directory)
                os.makedirs(target_sub_directory, exist_ok=True)
                target_path = os.path.join(target_sub_directory, hash_str + file_ext)

                # 检测哈希冲突
                if hash_str in all_records:
                    has_err = True
                    fp.write(f"== 哈希冲突: {prefix + relative_path} 和 {all_records[hash_str]} 都哈希为 {hash_str}\n")
                else:
                    count += 1
                    all_records[hash_str] = prefix + relative_path

                    # 处理文件（编译/加密/复制）
                    if file_ext == ".lua":
                        byte_code(jitPath, filepath)
                        encrypt_file(encPath, tmpFile, target_path)
                        os.remove(tmpFile)
                    else:
                        if checkFileExt(filepath):
                            encrypt_file(encPath, filepath, target_path)
                        else:
                            shutil.copy(filepath, target_path)

                    # 记录新生成的文件（用于后续更新version.json）
                    # 计算相对outres的路径（作为version.json的key）
                    rel_outres_path = os.path.relpath(target_path, dest_root).replace("\\", "/")
                    new_outres_files[target_path] = rel_outres_path

                    print(f"处理完成：{filepath} → {target_path}")

    # 4. 检查哈希冲突
    if has_err:
        print("\n!!! 哈希冲突 !!!  请检查日志文件 collision_log.txt")
    else:
        print(f"\n文件处理完成，共处理 {count} 个文件")

        # 5. 更新version.json
        if new_outres_files:
            update_version_json()
        else:
            print("无新生成的文件，跳过version.json更新")

    print("\n任务结束")
