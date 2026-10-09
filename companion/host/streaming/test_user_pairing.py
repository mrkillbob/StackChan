"""Offline tests: only the human terminal and physical serial boundaries are surrogates."""
import contextlib
import importlib.util
import io
import sys
import types
import unittest
from unittest.mock import patch

if importlib.util.find_spec('user_pairing'):
    import user_pairing
else:
    user_pairing = None
from provisioning import unpack_grant

TOKEN = 'offline-fixture-token-NOT-A-CREDENTIAL'

class Terminal(io.StringIO):
    def __init__(self, tty=True): super().__init__(); self.tty = tty
    def isatty(self): return self.tty

class SerialBoundary:
    """Surrogate for pyserial's external open/control-line/write/close effects only."""
    def __init__(self, events, short=False, error=None):
        object.__setattr__(self, 'events', events)
        object.__setattr__(self, 'short', short)
        object.__setattr__(self, 'error', error)
        object.__setattr__(self, 'frames', [])
    def __setattr__(self, key, value):
        self.events.append((key, value)); object.__setattr__(self, key, value)
    def open(self):
        self.events.append(('open',))
        if self.error == 'open': raise OSError(TOKEN)
    def write(self, frame):
        self.events.append(('write',)); self.frames.append(frame)
        if self.error == 'write': raise OSError(TOKEN)
        return len(frame)-1 if self.short else len(frame)
    def close(self): self.events.append(('close',))

class UserPairingTests(unittest.TestCase):
    def setUp(self):
        self.assertIsNotNone(user_pairing, 'user-controlled pairing CLI is not implemented')

    def run_cli(self, *, answer='PAIR', token=TOKEN, tty=(True,True,True), short=False, error=None, args=None):
        events=[]; boundary=SerialBoundary(events, short, error)
        streams=[Terminal(flag) for flag in tty]
        def serial_factory(**kwargs):
            events.append(('construct', kwargs)); return boundary
        def confirm(prompt):
            # Removing reset/Pair disclosure must fail before any serial open.
            self.assertIn('Pair trusted host', streams[1].getvalue())
            self.assertIn('reset', streams[1].getvalue().lower())
            events.append(('confirm',)); return answer
        def masked(prompt): events.append(('masked-token',)); return token
        fake=types.SimpleNamespace(Serial=serial_factory)
        with patch.object(sys,'stdin',streams[0]), patch.object(sys,'stdout',streams[1]), patch.object(sys,'stderr',streams[2]), \
             patch('builtins.input',confirm), patch('getpass.getpass',masked), patch.dict(sys.modules,{'serial':fake}):
            result=user_pairing.main(args if args is not None else ['--port','/dev/offline-fixture'])
        output=streams[1].getvalue()+streams[2].getvalue()
        self.assertNotIn(TOKEN, output)
        return result,events,boundary,output

    def test_noninteractive_input_is_denied_before_token_or_serial(self):
        for flags in [(False,True,True),(True,False,True),(True,True,False)]:
            with self.subTest(flags=flags):
                result,events,_,_=self.run_cli(tty=flags)
                self.assertNotEqual(result,0); self.assertEqual(events,[])

    def test_default_confirmation_denies_all_serial_effects(self):
        result,events,boundary,_=self.run_cli(answer='')
        self.assertNotEqual(result,0)
        self.assertFalse(boundary.frames)
        self.assertFalse(any(e[0]=='construct' for e in events))

    def test_confirmation_is_exact_and_not_a_yes_shortcut(self):
        for answer in ['yes','pair',' PAIR ','y']:
            with self.subTest(answer=answer):
                result,events,_,_=self.run_cli(answer=answer)
                self.assertNotEqual(result,0)
                self.assertFalse(any(e[0]=='construct' for e in events))

    def test_one_write_uses_real_grant_framing_and_pinned_recipient(self):
        result,events,boundary,output=self.run_cli()
        self.assertEqual(result,0)
        self.assertEqual(len(boundary.frames),1)
        frame=boundary.frames[0]
        self.assertLessEqual(len(frame),755)
        self.assertEqual(frame[:5],b'SCPG\x01')
        uri,device,token,request=unpack_grant(frame)
        self.assertEqual(uri,'wss://192.168.1.75:18443/companion/audio')
        self.assertEqual(device,'stackchan-80456b561404')
        self.assertEqual(token,TOKEN)
        self.assertTrue(1<=len(request)<=128)
        self.assertIn('no acknowledgement',output.lower())
        self.assertIn('screen',output.lower())

    def test_serial_remains_closed_until_control_lines_are_disabled(self):
        _,events,_,_=self.run_cli()
        names=[e[0] for e in events]
        self.assertEqual(names,['masked-token','confirm','construct','dtr','rts','port','open','write','close'])
        options=events[2][1]
        self.assertIsNone(options['port'])
        self.assertEqual(options['baudrate'],115200)
        self.assertGreater(options['write_timeout'],0)
        self.assertLessEqual(options['write_timeout'],2)
        self.assertEqual(events[3:5],[('dtr',False),('rts',False)])

    def test_short_write_closes_without_retry_or_success_claim(self):
        result,events,boundary,output=self.run_cli(short=True)
        self.assertNotEqual(result,0)
        self.assertEqual(len(boundary.frames),1)
        self.assertEqual(events[-1],('close',))
        self.assertNotIn('One frame written',output)

    def test_serial_exceptions_never_expose_token_and_always_close(self):
        for operation in ['open','write']:
            with self.subTest(operation=operation):
                result,events,_,_=self.run_cli(error=operation)
                self.assertNotEqual(result,0); self.assertEqual(events[-1],('close',))

    def test_malformed_or_oversized_token_is_denied_before_serial(self):
        for token in ['', 'x'*257, 'space token', '\u00e9']:
            with self.subTest(size=len(token)):
                result,events,_,_=self.run_cli(token=token)
                self.assertNotEqual(result,0)
                self.assertFalse(any(e[0]=='construct' for e in events))

    def test_largest_credential_still_makes_one_bounded_frame(self):
        result,_,boundary,_=self.run_cli(token='x'*256)
        self.assertEqual(result,0); self.assertLessEqual(len(boundary.frames[0]),755)

    def test_endpoint_and_device_cannot_be_overridden_through_argv(self):
        result,events,_,output=self.run_cli(args=['--endpoint',TOKEN,'--device',TOKEN,'--token',TOKEN])
        self.assertNotEqual(result,0); self.assertEqual(events,[])
        self.assertNotIn(TOKEN,output)

    def test_masking_failure_is_denied_without_echo_fallback(self):
        import getpass
        import warnings
        def warning(prompt): warnings.warn('terminal unavailable',getpass.GetPassWarning); return TOKEN
        with patch('getpass.getpass',warning):
            # Dedicated run avoids replacing the getpass warning surrogate above.
            events=[]; streams=[Terminal() for _ in range(3)]
            fake=types.SimpleNamespace(Serial=lambda **kwargs:events.append(kwargs))
            with patch.object(sys,'stdin',streams[0]),patch.object(sys,'stdout',streams[1]),patch.object(sys,'stderr',streams[2]),patch.dict(sys.modules,{'serial':fake}):
                result=user_pairing.main(['--port','/dev/offline-fixture'])
            self.assertNotEqual(result,0);self.assertEqual(events,[])
            self.assertNotIn(TOKEN,''.join(s.getvalue() for s in streams))

if __name__=='__main__': unittest.main()
