#!/usr/bin/env python
# -*- coding: UTF-8 -*-
"""
NAME
    GenResMD5 --


SYNOPSIS
    GenResMD5 [-h]
"""

import os
import hashlib
import json
import re

ignoreDir = ["Default"]
output = "version.json"
scriptRoot = os.path.split(os.path.realpath(__file__))[0]
projectRoot = os.path.split(scriptRoot)[0]

info = {}
info["EngineVersion"] = "8.8.8" #引擎版本动，除非是大换血    游戏引擎版本过低，请重新下载游戏
info["GameVersion"] = "1.1.6" #热更用
info["packages"] = ["game"] # first package name is fixed for cpp
info["asserts"] = {}

def joinDir(root, *dirs):
    for item in dirs:
        root = os.path.join(root, item)
    return root

scanRoot = joinDir(projectRoot, "outres")
print("资源路径: %s " %(scanRoot))

def getMD5(root):
    files = os.listdir(root)
    for f in files:
        itemPath = joinDir(root, f)
        print("版本====================================================================================: %s "%(info["GameVersion"]))
        print("热更: %s "%(itemPath))
        if os.path.isdir(itemPath):
            if (f[0] == '.' or (f in ignoreDir)):
                pass
            else:
                getMD5(itemPath)
        elif os.path.isfile(itemPath):
            if f[0] != '.' and f != output:
                fp = open(itemPath, 'rb')
                m5 = hashlib.md5()
                m5.update(fp.read())
                fp.close()
                name = itemPath[(len(scanRoot) + 1):]
                if os.sep == '\\':
                    name = re.sub('\\\\', '/', name)
                # key is path, value[0] = md5, value[2] = size
                info["asserts"][name] = [m5.hexdigest(), os.path.getsize(itemPath)]

getMD5(scanRoot)
jsonStr = json.dumps(info)
fp = open(joinDir(scanRoot, output), "wb")
fp.write(jsonStr.encode("ISO8859-1"))
fp.close()
