#!/usr/bin/env python

import os
import shutil
import subprocess

MSBUILD_PATH = os.path.join(os.environ['MSBUILD_PATH'], 'msbuild.exe')
SLN_ROOT = os.environ['CBP_SLN_ROOT']

assert(os.path.exists(MSBUILD_PATH))
assert(os.path.exists(SLN_ROOT))

# relative to SLN_ROOT
OUT='tmp'
FOMOD='installer\\generateFomod.py'
SLN = 'CBP.sln'
DLL='CBP.dll'

REBUILD=True
CONFIGS= ['ReleaseAVX2 MT', 'Release MT']

def cleanup(p):
    for f in os.scandir(p):
         os.unlink(f)

def prepare(p):
    if os.path.exists(p):
        if not os.path.isdir(p):
            raise Exception('Invalid output path: {}'.format(p))
    else:
        os.mkdir(p)

def safe_mkdir(p):
    try:
        os.mkdir(p)
    except FileExistsError:
        pass

def test_file(p):
    return os.path.exists(p) and os.path.isfile(p) 

def make_package(fmscript, targets):
    cmd = [ 'python', fmscript ]
    cmd.extend(targets)

    print(cmd)

    r = subprocess.run(cmd)
    if r.returncode != 0:
        raise Exception('Could not build the package ({})'.format(r.returncode))
        
def build_solution(cfg, bc, path, rebuild):
    args = [ '-p:Configuration={};OutDir={}'.format(cfg, path) ]
    if rebuild:
        args.append('-t:Rebuild')

    cmd = bc
    cmd.extend(args)

    print('Building {} ..'.format(cfg))

    r = subprocess.run(cmd)
    if r.returncode != 0:
        raise Exception('Build failed: {}'.format(cfg))

OUTPUT_PATH = os.path.join(SLN_ROOT, OUT)
basecmd = [MSBUILD_PATH, os.path.join(SLN_ROOT, SLN)]

prepare(OUTPUT_PATH)

package_targets = []

for e in CONFIGS:
    path = os.path.join(OUTPUT_PATH, e)
    safe_mkdir(path)
    cleanup(path)
    build_solution(e, basecmd, path, REBUILD)

    dll = os.path.join(path, DLL)
    if not test_file(dll):
        raise Exception('Could not find dll: {}'.format(dll))

    package_targets.append('{}|{}'.format(e, dll))

make_package(os.path.join(SLN_ROOT, FOMOD), package_targets)

print('OK')
