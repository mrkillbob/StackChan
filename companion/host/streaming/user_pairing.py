"""Human-operated, one-shot volatile USB grant handoff. No gateway/network IO.

Run in a real terminal. Tokens are accepted only through masked getpass, never
arguments or files. This CLI cannot confirm device receipt or pairing-window
state; it does not read serial, connect audio, or save robot settings.
"""
import argparse
import getpass
import sys
import uuid
import warnings

from provisioning import MAX_FRAME, pack_grant

ENDPOINT = 'wss://192.168.1.75:18443/companion/audio'
DEVICE = 'stackchan-80456b561404'


class _Parser(argparse.ArgumentParser):
    def error(self, message):
        # argparse's default error can echo unknown arguments, including a token
        # accidentally supplied by the caller. Keep all failures generic.
        raise ValueError('invalid command arguments')


def main(argv=None):
    token = frame = None
    connection = None
    try:
        parser = _Parser(description='Interactive one-shot USB pairing; never pass a token in arguments.')
        parser.add_argument('--port', required=True, help='Verified robot serial port path; no automatic discovery.')
        try:
            args = parser.parse_args(argv)
        except SystemExit as exit_request:
            return int(exit_request.code)
        if not all(stream.isatty() for stream in (sys.stdin, sys.stdout, sys.stderr)):
            print('Denied: run from an interactive terminal with no redirection.', file=sys.stderr)
            return 2
        if not args.port or len(args.port) > 256 or any(ord(c) < 33 or ord(c) > 126 for c in args.port):
            print('Denied: provide a bounded serial port path.', file=sys.stderr)
            return 2
        print(f'Pinned device: {DEVICE}\nPinned endpoint: {ENDPOINT}')
        print('Read the current token from your protected local preparation sheet or transport-test terminal. Do not paste it into chat.')
        with warnings.catch_warnings():
            # getpass otherwise falls back to echoed input if terminal control
            # fails. Treat that warning as denial before any fallback read.
            warnings.simplefilter('error', getpass.GetPassWarning)
            token = getpass.getpass('Token (masked): ')
        frame = pack_grant(ENDPOINT, DEVICE, token, uuid.uuid4().hex)
        if len(frame) > MAX_FRAME:
            raise ValueError('frame exceeds receiver limit')
        print('On the robot, open Companion and physically tap Pair trusted host now (opens a 60-second window).')
        print('Opening this serial port MAY reset a USB-UART robot even with DTR/RTS disabled.')
        print('If it resets or the pairing window expires, retry manually with a fresh Pair window; no automatic retry occurs.')
        print('This sends one grant frame only. It does not connect audio or save NVS settings.')
        if input('Type PAIR to authorize this single write, or Enter to cancel: ') != 'PAIR':
            print('Cancelled. Serial port was not opened.')
            return 2
        import serial  # Existing official pyserial installation; no driver/install/discovery actions.
        connection = serial.Serial(port=None, baudrate=115200, timeout=0, write_timeout=2)
        connection.dtr = False
        connection.rts = False
        connection.port = args.port
        connection.open()
        written = connection.write(frame)  # Exactly one bounded write; never retry a partial frame.
        if written != len(frame):
            print('Incomplete write. Close and retry manually after a fresh physical Pair window.', file=sys.stderr)
            return 3
        print('One frame written. No acknowledgement was read. Check the robot screen for a staged grant.')
        print('Use Connect separately only when ready. This CLI has not started audio.')
        return 0
    except (Exception, KeyboardInterrupt):
        # Never include exception details, traceback, frame, token or argument values.
        print('Pairing was not completed. Check the terminal, token and serial setup; retry manually.', file=sys.stderr)
        return 3
    finally:
        if connection is not None:
            try:
                connection.close()
            except Exception:
                pass  # Do not echo a serial library exception or attempt a second operation.
        token = frame = None  # Python immutable objects cannot promise secure memory erasure.


if __name__ == '__main__':
    raise SystemExit(main())
