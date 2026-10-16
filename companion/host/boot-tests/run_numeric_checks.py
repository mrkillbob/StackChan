#!/usr/bin/env python3
"""Compile actual numeric pairing parser with deterministic transport boundaries."""
from pathlib import Path
import json, subprocess, tempfile
repo=Path(__file__).resolve().parents[3]
tests=Path(__file__).resolve().parent
sdk=Path('/Users/mikedemott/Developer/AgentWork/StackChan/.tools/sdk/esp-idf-v5.5.4')
cjson=sdk/'components/json/cJSON'
with tempfile.TemporaryDirectory(prefix='stackchan-numeric-tests-') as directory:
    output=Path(directory)
    flags=['-fsanitize=address,undefined','-fno-omit-frame-pointer']
    subprocess.run(['clang','-c',*flags,str(cjson/'cJSON.c'),'-o',str(output/'cJSON.o')],check=True)
    for name in ['numeric_pairing','numeric_tls','wireless_pairing_next']:
        argv=['clang++','-std=c++17','-Wall','-Wextra','-Werror',*flags,'-I'+str(repo/'firmware/main'),'-I'+str(repo/'firmware/main/companion'),'-I'+str(tests/'numeric-fixture'),'-I'+str(cjson)]
        if name in ('numeric_tls','wireless_pairing_next'):argv+=['-include',str(tests/'numeric-fixture/sdk_boundary.hpp'),str(repo/'firmware/main/companion/numeric_pairing.cc'),str(output/'cJSON.o')]
        argv+=[str(tests/('test_'+name+'.cpp')),'-o',str(output/name)]
        subprocess.run(argv,check=True)
        subprocess.run([str(output/name)],check=True)
print(json.dumps({'result':'passed','groups':3,'sanitizers':['address','undefined'],'hardware':False,'network':False}))
