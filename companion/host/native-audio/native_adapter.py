"""Worker-only binding. Constructing/loading this module never opens device IO.
Explicit open_for_approved_test() is separate from arm_for_approved_test().
Use CodecHostBridge for already-tested Opus/resampling; no Python callback runs
on a CoreAudio thread. One serialized worker/control owner required.
"""
import ctypes as C

TO_VOICE = 'Pro Tools Audio Bridge 2-A_UID'
FROM_VOICE = 'Pro Tools Audio Bridge 2-B_UID'

class NativeHostAdapter:
    def __init__(self, library):
        self.lib=C.CDLL(str(library)); self.handle=C.c_void_p()
        self.epoch=0; self.muted=True; self.playing=False
        specs={
            'sc_open':([C.c_char_p,C.c_char_p,C.POINTER(C.c_void_p)],C.c_int),
            'sc_start':([C.c_void_p],C.c_int),
            'sc_begin':([C.c_void_p,C.c_uint32],C.c_int),
            'sc_arm':([C.c_void_p,C.c_uint32],C.c_int),
            'sc_mute':([C.c_void_p,C.c_uint32],C.c_int),
            'sc_playing':([C.c_void_p,C.c_uint32,C.c_int],C.c_int),
            'sc_push':([C.c_void_p,C.c_uint32,C.POINTER(C.c_int16),C.c_uint32],C.c_int),
            'sc_pull':([C.c_void_p,C.c_uint32,C.POINTER(C.c_int16),C.c_uint32],C.c_int),
            'sc_error':([C.c_void_p],C.c_int),
            'sc_counter':([C.c_void_p,C.c_int,C.c_int],C.c_uint64),
            'sc_close':([C.c_void_p],None),
        }
        for name,(args,result) in specs.items():
            function=getattr(self.lib,name); function.argtypes=args; function.restype=result
    def _require(self):
        if not self.handle.value: raise RuntimeError('device IO has not been explicitly opened')
    def _check(self,status):
        if status: raise RuntimeError(f'CoreAudio bridge status {status}')
    def open_for_approved_test(self):
        if self.handle.value: raise RuntimeError('already open')
        self._check(self.lib.sc_open(TO_VOICE.encode(),FROM_VOICE.encode(),C.byref(self.handle)))
        try: self._check(self.lib.sc_start(self.handle))
        except Exception: self.close(); raise
    def begin(self,epoch):
        self._require()
        if type(epoch) is not int or not self.epoch<epoch<=0xffffffff: raise ValueError('new uint32 epoch required')
        self._check(self.lib.sc_begin(self.handle,epoch))
        self.epoch=epoch; self.muted=True; self.playing=False
    def arm_for_approved_test(self,epoch):
        self._require()
        if epoch!=self.epoch: raise ValueError('stale epoch')
        self._check(self.lib.sc_arm(self.handle,epoch)); self.muted=False
    def set_playing(self,epoch,playing):
        self._require()
        if type(playing) is not bool: raise ValueError('playing flag')
        if epoch!=self.epoch: return False
        self._check(self.lib.sc_playing(self.handle,epoch,int(playing))); self.playing=playing; return True
    def robot_pcm(self,epoch,pcm):
        self._require()
        if type(pcm) is not bytes or not pcm or len(pcm)%2 or len(pcm)>11520: raise ValueError('bounded PCM16 chunk <=120ms')
        if epoch!=self.epoch or self.muted or self.playing: return False
        for start in range(0,len(pcm),4096):
            chunk=pcm[start:start+4096]
            samples=(C.c_int16*(len(chunk)//2)).from_buffer_copy(chunk)
            if self.lib.sc_push(self.handle,epoch,samples,len(samples))!=len(samples): return False
        return True
    def robot_output_pcm(self,epoch,frames):
        self._require()
        if type(frames) is not int or not 1<=frames<=2048: raise ValueError('frame bound')
        samples=(C.c_int16*frames)()
        if epoch==self.epoch and not self.muted:
            count=self.lib.sc_pull(self.handle,epoch,samples,frames)
            if count<0: self.muted=True
        return bytes(samples)
    def interrupt(self,epoch):
        self._require()
        if epoch!=self.epoch: return False
        self._check(self.lib.sc_mute(self.handle,epoch)); self.muted=True; self.playing=False; return True
    def close(self):
        if self.handle.value: self.lib.sc_close(self.handle); self.handle=C.c_void_p()
        self.muted=True; self.playing=False
