import asyncio
import json
import unittest
from types import SimpleNamespace
try:
    import numeric_pairing_worker as w
except ImportError:
    w=None
from test_numeric_pairing import Clock,REQ
from pairing_receipt import decode_receipt
import struct
class Port:
    def __init__(self,short=False):self.events=[];self.short=short;self.reads=[struct.pack('>4sBB16s',b'SCPS',1,1,struct.pack('>II8s',91,1,bytes(8)))];self.closed=False;self.is_open=False
    def open(self):self.events.append('open');self.is_open=True
    def read(self,n):return self.reads.pop(0) if self.reads else b''
    def write(self,x):self.events.append(('write',x));return len(x)-1 if self.short else len(x)
    def close(self):self.closed=True;self.is_open=False;self.events.append('close')
class WorkerTests(unittest.IsolatedAsyncioTestCase):
    def setUp(self):self.assertIsNotNone(w,'numeric worker missing');self.events=[];self.clock=Clock();self.port=Port();self.opened=0;self.loaded=0
    def worker(self,approved=True,paused=False):
        def factory():self.opened+=1;return self.port
        def loader():self.loaded+=1;raise RuntimeError('synthetic loader boundary')
        return w.NumericWorker(approved=approved,paused=lambda:paused,serial_factory=factory,runtime_loader=loader,emit=self.events.append,clock=self.clock,sleep=self.sleep)
    async def sleep(self,n):self.clock.now+=n;await asyncio.sleep(0)
    async def test_import_and_constructor_inert(self):
        worker=self.worker();self.assertEqual(self.opened,0);self.assertEqual(self.loaded,0);self.assertIsNone(worker.port)
    async def test_activation_and_pause_denial_before_open(self):
        for approved,paused in ((False,False),(True,True)):
            worker=self.worker(approved,paused);await worker.command('prepare');await worker.wait()
            self.assertEqual(self.opened,0);self.assertEqual(self.loaded,0);self.assertEqual(worker.state,'failed')
    async def test_open_before_pair_no_write_during_stabilization(self):
        # No Ready until stabilization ends; recorded factory/readers are synthetic.
        self.port.reads=[];worker=self.worker();await worker.command('prepare')
        while self.clock.now<2:await asyncio.sleep(0)
        self.assertEqual(self.port.events,['open']);self.assertEqual(self.loaded,0)
        await worker.command('cancel');await worker.wait();self.assertTrue(self.port.closed);self.assertEqual(worker.state,'cancelled')
    async def test_invalid_command_never_opens(self):
        worker=self.worker()
        with self.assertRaises(Exception):await worker.command('Prepare')
        self.assertEqual(self.opened,0)
    async def test_eof_close_cancels_inflight(self):
        self.port.reads=[];worker=self.worker();await worker.command('prepare');await asyncio.sleep(0);await worker.close();self.assertTrue(self.port.closed);self.assertEqual(worker.state,'closed')
    async def test_ready_then_listener_then_one_numeric_write(self):
        from contextlib import asynccontextmanager
        events=[];port=self.port;clock=self.clock
        ready=struct.pack('>4sBB16s',b'SCPS',1,1,struct.pack('>II8s',91,1,bytes(8)))
        sent=False
        def read(cap):
            nonlocal sent
            if clock.now>=2 and not sent:sent=True;events.append('ready');return ready
            return b''
        port.read=read
        class Runtime:
            transport=object()
            def issue(self,ttl):raise AssertionError('no confirmation')
            def revoke(self):events.append('revoke')
            @asynccontextmanager
            async def listener(self,gateway):
                events.append('listener_open')
                try:yield
                finally:events.append('listener_closed')
        def load():events.append('load');return Runtime()
        worker=w.NumericWorker(approved=True,paused=lambda:False,serial_factory=lambda:port,runtime_loader=load,emit=self.events.append,clock=clock,sleep=self.sleep)
        await worker.command('prepare')
        for _ in range(500):
            if any(type(x) is tuple and x[0]=='write' for x in port.events):break
            await asyncio.sleep(0)
        writes=[x for x in port.events if type(x) is tuple]
        self.assertEqual(len(writes),1);self.assertEqual(writes[0][1][:5],b'SCNP\x01');self.assertEqual(len(writes[0][1]),25)
        self.assertEqual(events[:3],['ready','load','listener_open'])
        await worker.command('cancel');await worker.wait();self.assertTrue(port.closed);self.assertIn('listener_closed',events)
    async def test_retry_is_explicit_and_not_automatic(self):
        worker=self.worker(approved=False);await worker.command('prepare');await worker.wait()
        self.assertEqual(worker.state,'failed');self.assertEqual(self.opened,0)
        await worker.command('retry');await worker.wait();self.assertEqual(self.opened,0)
        self.assertEqual(sum(x['state']=='failed' for x in self.events),2)
    async def test_missing_ready_never_loads_or_writes(self):
        self.port.reads=[];worker=self.worker();await worker.command('prepare');await worker.wait()
        self.assertEqual(worker.state,'failed');self.assertEqual(self.loaded,0);self.assertEqual(self.port.events,['open','close'])
    async def test_terminal_cancel_withheld_until_listener_and_usb_cleanup(self):
        from contextlib import asynccontextmanager
        gate=asyncio.Event();port=self.port;clock=self.clock;sent=False
        ready=struct.pack('>4sBB16s',b'SCPS',1,1,struct.pack('>II8s',91,1,bytes(8)))
        def read(cap):
            nonlocal sent
            if clock.now>=2 and not sent:sent=True;return ready
            return b''
        port.read=read
        class Runtime:
            transport=object()
            def issue(self,ttl):return 'synthetic'
            def revoke(self):pass
            @asynccontextmanager
            async def listener(self,gateway):
                try:yield
                finally:await gate.wait()
        worker=w.NumericWorker(approved=True,paused=lambda:False,serial_factory=lambda:port,runtime_loader=Runtime,emit=self.events.append,clock=clock,sleep=self.sleep)
        await worker.command('prepare')
        while not any(type(x) is tuple for x in port.events):await asyncio.sleep(0)
        cancel=asyncio.create_task(worker.command('cancel'));await asyncio.sleep(0);await asyncio.sleep(0)
        self.assertFalse(port.closed);self.assertFalse(any(x['state']=='cancelled' for x in self.events))
        gate.set();await cancel
        self.assertTrue(port.closed);self.assertEqual(self.events[-1]['state'],'cancelled')
    async def test_close_failure_no_terminal_ack_or_retry(self):
        self.port.reads=[]
        def bad_close():raise OSError('synthetic secret')
        self.port.close=bad_close
        worker=self.worker();await worker.command('prepare');await asyncio.sleep(0)
        await worker.command('cancel');await worker.wait()
        self.assertTrue(worker.cleanup_blocked)
        self.assertFalse(any(x['state'] in ('cancelled','closed') for x in self.events))
        with self.assertRaises(Exception):await worker.command('retry')
if __name__=='__main__':unittest.main()
