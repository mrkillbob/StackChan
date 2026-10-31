from pathlib import Path
import importlib.util
root=Path(__file__).resolve().parent.parent
spec=importlib.util.spec_from_file_location('profile',root/'tools/build_profile.py')
p=importlib.util.module_from_spec(spec);spec.loader.exec_module(p)
key='muse_voice_protocol'
assert key in p.FEATURES, 'voice protocol must have its own pre-flash closure'
for name in ('full','no-games'):
 r=p.resolve(name);assert r['definitions']['STACKCHAN_BUILD_MUSE_VOICE_PROTOCOL']==1
assert p.resolve('mira-core')['definitions']['STACKCHAN_BUILD_MUSE_VOICE_PROTOCOL']==0
for selected in ([],['muse'],['muse',key]):
 r=p.resolve('custom',{'schema':1,'features':selected});assert r['definitions']['STACKCHAN_BUILD_MUSE_VOICE_PROTOCOL']==int(key in selected)
try:p.resolve('custom',{'schema':1,'features':[key]})
except ValueError as e:assert 'requires muse' in str(e).lower()
else:raise AssertionError('voice protocol without Muse must fail closed')
r=p.resolve('custom',{'schema':1,'features':['muse',key]})
assert 'set(STACKCHAN_PROFILE_MUSE_VOICE_PROTOCOL 1)' in p.render_cmake(r)
assert '#define STACKCHAN_BUILD_MUSE_VOICE_PROTOCOL 0' in (root/'main/build_profiles/features.hpp').read_text()
print('Voice profile dependency and seven closure cases passed')
