"""POSIX diagnostic serial: suppress application modem-line changes.

Import opens nothing. This does not prevent OS/USB-driver changes at os.open;
physical boot continuity must be qualified. No claim of reset-free transport.
Only the read-only diagnostic helper uses this class; pairing is unchanged.
"""
import termios
from serial.serialposix import Serial

class DiagnosticSerial(Serial):
    def _update_dtr_state(self):
        # pySerial calls this on open even for a preconfigured False value.
        pass

    def _update_rts_state(self):
        pass

    def _reconfigure_port(self, force_update=False):
        if self._rtscts or self._dsrdtr or self._rs485_mode is not None:
            raise ValueError('diagnostic_flow_control_forbidden')
        super()._reconfigure_port(force_update)
        attrs=termios.tcgetattr(self.fd)
        if attrs[2]&termios.HUPCL:
            attrs[2]&=~termios.HUPCL
            termios.tcsetattr(self.fd,termios.TCSANOW,attrs)
        if termios.tcgetattr(self.fd)[2]&termios.HUPCL:
            raise ValueError('diagnostic_hangup_not_disabled')

    def write(self, data):
        raise ValueError('diagnostic_write_forbidden')

    def send_break(self, duration=.25):
        raise ValueError('diagnostic_break_forbidden')

    def _update_break_state(self):
        raise ValueError('diagnostic_break_forbidden')
