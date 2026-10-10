#!/usr/bin/env python3
"""Capture one explicitly exported, public SCFD frame; inert until invoked.

On StackChan, keep Companion muted/stopped and press Export diagnostics after
starting this helper. It never sends commands, pairing data or credentials.
Opening USB may still reset a board through the OS driver: this helper does not attest physical silence or
that a prior reboot record survived. No listener or audio device is opened.
"""
import argparse
import json
import math
from pathlib import Path
import time
from streaming.diagnostic_snapshot import MAGIC,FRAME_SIZE,SnapshotError,decode_snapshot

class CaptureError(ValueError):pass

def _serial_factory():
    from diagnostic_serial import DiagnosticSerial
    return DiagnosticSerial(port=None,baudrate=115200,timeout=.1,write_timeout=.1)

def capture_snapshot(port_name,*,serial_factory=None,clock=time.monotonic,timeout_seconds=30):
    if type(port_name) is not str or not 1<=len(port_name)<=256 or any(ord(c)<32 for c in port_name):raise CaptureError('invalid_capture')
    if type(timeout_seconds) not in (int,float) or not math.isfinite(timeout_seconds) or not 0<timeout_seconds<=30:raise CaptureError('invalid_capture')
    started=clock()
    if type(started) not in (int,float) or not math.isfinite(started):raise CaptureError('invalid_capture')
    try:port=(serial_factory or _serial_factory)()
    except Exception:raise CaptureError('serial_unavailable') from None
    if getattr(port,'is_open',True) is not False:raise CaptureError('serial_must_be_closed')
    buffer=bytearray();scanned=0;deadline=started+timeout_seconds
    try:
        port.dtr=False;port.rts=False;port.port=port_name;port.timeout=min(.1,timeout_seconds)
        port.open()
        while True:
            now=clock()
            if now>=deadline:raise CaptureError('capture_expired')
            if scanned>=8192:raise CaptureError('scan_limit')
            port.timeout=min(.1,deadline-now)
            requested=min(512,8192-scanned)
            chunk=port.read(requested)
            if clock()>=deadline:raise CaptureError('capture_expired')
            if type(chunk) is not bytes or len(chunk)>requested:raise CaptureError('invalid_serial_read')
            scanned+=len(chunk);buffer.extend(chunk)
            while True:
                offset=buffer.find(MAGIC)
                if offset<0:
                    # Retain only a possible split magic prefix; discard console.
                    if len(buffer)>3:del buffer[:-3]
                    break
                if offset:del buffer[:offset]
                if len(buffer)<FRAME_SIZE:break
                candidate=bytes(buffer[:FRAME_SIZE])
                try:return decode_snapshot(candidate)
                except SnapshotError:del buffer[0]
    except CaptureError:raise
    except Exception:raise CaptureError('serial_read_failed') from None
    finally:
        buffer.clear()
        try:
            port.close()
            if getattr(port,'is_open',True) is not False:raise CaptureError('serial_close_failed')
        except Exception:raise CaptureError('serial_close_failed') from None

def save_snapshot(path,value):
    """Save the validated parser result, never console bytes; preserve evidence."""
    payload=json.dumps(value,sort_keys=True,indent=2,allow_nan=False)+'\n'
    with Path(path).open('x',encoding='utf-8') as output:output.write(payload)

def main(argv=None):
    parser=argparse.ArgumentParser(description='Read one human-exported diagnostic snapshot while StackChan is muted/stopped. No USB commands are sent.')
    parser.add_argument('--port',required=True)
    parser.add_argument('--output',required=True,type=Path)
    args=parser.parse_args(argv)
    try:
        value=capture_snapshot(args.port)
        save_snapshot(args.output,value)
    except (CaptureError,OSError,ValueError):
        parser.exit(1,'Diagnostic capture failed; no snapshot saved.\n')
    print('Validated public diagnostic snapshot saved.')
    return 0
if __name__=='__main__':raise SystemExit(main())
