from pathlib import Path
import importlib.util,json
r=Path(__file__).resolve().parent.parent;spec=importlib.util.spec_from_file_location('profiles',r/'tools/build_profile.py');p=importlib.util.module_from_spec(spec);spec.loader.exec_module(p)
assert 'bounded_motion' in p.FEATURES,'bounded motion must be independently selectable before flash'
for selected in [[],['bounded_motion'],['hermes'],['hermes','bounded_motion']]:
 d=p.resolve('custom',{'schema':1,'features':selected},r/'main');assert d['definitions']['STACKCHAN_BUILD_BOUNDED_MOTION']==int('bounded_motion' in selected)
 assert 'music' not in d['enabled'] and 'games' not in d['enabled']
assert p.resolve('mira-core',main_root=r/'main')['definitions']['STACKCHAN_BUILD_BOUNDED_MOTION']==0
import tempfile,subprocess,re,os
cmake=Path('/Users/mikedemott/Developer/AgentWork/StackChan/.tools/espressif/tools/cmake/3.30.2/CMake.app/Contents/bin/cmake')
block=re.search(r'target_compile_definitions\(\$\{COMPONENT_LIB\} PRIVATE \$\{STACKCHAN_PROFILE_DEFINITIONS\}.*?\)\n',Path(os.environ.get('MOTION_CMAKE_SOURCE',str(r/'main/CMakeLists.txt'))).read_text(),re.S).group(0)
with tempfile.TemporaryDirectory(prefix='bounded-profile-') as temp:
 t=Path(temp)
 for enabled in (False,True):
  d=p.resolve('custom',{'schema':1,'features':['bounded_motion'] if enabled else []},r/'main');(t/'selection.cmake').write_text(p.render_cmake(d))
  (t/'main.cpp').write_text('#include <cstdio>\n#ifndef STACKCHAN_BOUNDED_MOTION\n#define STACKCHAN_BOUNDED_MOTION 0\n#endif\nint main(){std::printf("%d",STACKCHAN_BOUNDED_MOTION);}')
  (t/'CMakeLists.txt').write_text('cmake_minimum_required(VERSION 3.20)\nproject(bounded_profile LANGUAGES CXX)\nset(COMPONENT_LIB profile)\nadd_executable(profile main.cpp)\ninclude(selection.cmake)\n'+block)
  build=t/('build-'+str(int(enabled)))
  ninja='/Users/mikedemott/Developer/AgentWork/StackChan/.tools/espressif/tools/ninja/1.12.1/ninja'
  subprocess.run([str(cmake),'-S',str(t),'-B',str(build),'-G','Ninja','-DCMAKE_MAKE_PROGRAM='+ninja],check=True,capture_output=True)
  subprocess.run([str(cmake),'--build',str(build),'--parallel','1'],check=True,capture_output=True)
  actual=subprocess.check_output([str(build/'profile')],text=True);assert actual==str(int(enabled)),{'actual':actual,'enabled':enabled,'block':block,'selection':(t/'selection.cmake').read_text()}
print('PASS bounded-motion selection, core omission and production compile alias')
