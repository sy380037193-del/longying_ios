import os
import sys
import shutil
import subprocess

scriptRoot = os.path.split(os.path.realpath(__file__))[0]
projRoot = os.path.split(scriptRoot)[0]
cmd_env = os.environ.copy()
cmd_env['LUA_PATH'] = os.path.join(scriptRoot, "?.lua")
tmpFile = os.path.join(scriptRoot, "000luabytecode.tmp")

dirFiles = {
    "res": [""],
    "src": ["app/views/mainui/Shop/DuoBaoGe.lua"],
}

def checkFileExt(path):
    # binExt = [".ogg", ".mp3", ".jpg", ".jpeg", ".png", ".pvr", ".ccz", ".bmp", ".tmx", ".plist", ".pb", ".csb", ".atlas", ".mb", ".mt", ".fnt", ".ttf"]
    binExt = ['.json', '.plist', '.png', '.ogg', '.atlas', '.jpg', '.csb', '.lua', '.mt', '.mb', '.ttf', '.mp3', '.lua-bak']
    ext = os.path.splitext(path)[1]
    ext = ext.lower()
    return ext in binExt


def fnv1a_64(filename):
    FNV_prime = 1099511628211
    offset_basis = 14695981039346656037
    hash = offset_basis
    for c in filename:
        hash ^= ord(c)
        hash *= FNV_prime
    return hash & 0xffffffffffffffff  # make sure hash is 64 bits.

def hash_to_string(hash):
    return "{:016x}".format(hash)


def byte_code(jitPath, path):
    jitcmd = '%s -b "%s" "%s"' %(jitPath, path, tmpFile)
    # do shell cmd
    cmd = subprocess.Popen(jitcmd, shell = True, stdout = subprocess.PIPE, env = cmd_env)
    cmd.wait()
    if os.path.exists(tmpFile) == False:
        print("Error: Fail to compile:%s" %(path))
        sys.exit(-1)


def encrypt_file(encPath, inpath, outpath):
    enccmd = '%s -k b91f4c4c00c92d562181b58d6ecc7491 -v 5e33f4430966a3cd1592d6b1bdc7d32b -i "%s" -o "%s"' %(encPath, inpath, outpath)
    # do shell cmd
    cmd = subprocess.Popen(enccmd, shell = True, stdout = subprocess.PIPE, env = cmd_env)
    cmd.wait()
    if os.path.exists(outpath) == False:
        print("Error: Fail to encrypt:%s" %(inpath))
        sys.exit(-1)


if __name__ == "__main__":
    jitPath = os.path.join(scriptRoot, "win32")
    jitPath = os.path.join(jitPath, "64")
    jitPath = os.path.join(jitPath, "luajit.exe")

    encPath = os.path.join(scriptRoot, "win32")
    encPath = os.path.join(encPath, "encoder.exe")

    all_records = {}
    dest_root = os.path.join(projRoot, "outres")
    if not os.path.exists(dest_root):
        os.makedirs(dest_root)

    has_err = False
    count = 0
    with open(os.path.join(scriptRoot, "collision_log.txt"), "w") as fp:
        res_root = os.path.join(projRoot, "resources")
        for dir_name, filenames in dirFiles.items():
            source_directory = os.path.join(res_root, dir_name)
            for filename in filenames:
                filepath = os.path.join(source_directory, filename)
                if os.path.isfile(filepath):
                    # relative path to res, and change \ to /
                    relative_path = os.path.relpath(filepath, source_directory).replace("\\", "/")
                    _, file_ext = os.path.splitext(relative_path)
                    # get hash code
                    prefix = "" if dir_name == "res" else "src/" # keep src dir in path
                    hashed_value = fnv1a_64(prefix + relative_path)
                    hash_str = hash_to_string(hashed_value)
                    # first hash char at index to find char for dir name
                    sub_directory = hash_str[int(hash_str[0], 16)]
                    target_sub_directory = os.path.join(dest_root, sub_directory)
                    # try create sub dir
                    if not os.path.exists(target_sub_directory):
                        os.makedirs(target_sub_directory)
                    # try write hash file
                    target_path = os.path.join(target_sub_directory, hash_str + file_ext)
                    if hash_str in all_records:
                        has_err = True
                        fp.write(f"== Hash Collision: {relative_path} and {all_records[hash_str]} both hashed to {hash_str}\n")
                    else:
                        count += 1
                        all_records[hash_str] = relative_path
                        # do byte code
                        if file_ext == ".lua":
                            byte_code(jitPath, filepath)
                            encrypt_file(encPath, tmpFile, target_path)
                            os.remove(tmpFile)
                        else:
                            if checkFileExt(filepath):
                                encrypt_file(encPath, filepath, target_path)
                            else:
                                shutil.copy(filepath, target_path)
                        print(f"== Copied {filepath} to {target_path}")
    if has_err:
        print("!!! hash collision !!!  check the log file collision_log.txt")
    else:
        print("Finished, file count:", count)
