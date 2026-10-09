"""Deployable WebSocket sidecar, inert until an operator starts a listener.
Reuses HomeLab registry/sessions; no new owner endpoints, token issuer, provider
account or TLS credentials. Existing-Mira attachment adapter is not supplied.
"""
from dataclasses import dataclass, field
from gateway_admission import authorize_audio_request
from gateway_connection import run_connection
from streaming_session import StreamingSession


@dataclass
class QualifiedProvider:
    bridge: object = field(repr=False)
    arm: object = field(repr=False)
    close: object = field(repr=False)
    session_reference: str = field(repr=False)
    identity_verified: bool = False
    kind: str = 'unavailable'


class RemoteGateway:
    def __init__(self, registry, sessions, provider_factory=None, *, fixture=False):
        self.registry = registry; self.sessions = sessions
        self.provider_factory = provider_factory; self.fixture = fixture
        self._active = set()

    def process_request(self, connection, request):
        try:
            authorize_audio_request(self.registry, request.path, list(request.headers.raw_items()))
        except PermissionError:
            return connection.respond(403, 'Audio admission denied.\n')
        if self.provider_factory is None:
            return connection.respond(503, 'Qualified conversation adapter unavailable.\n')
        return None

    async def handle(self, socket):
        provider = None; session = None; device_id = None; driver_started = False
        try:
            grant = authorize_audio_request(self.registry, socket.request.path, list(socket.request.headers.raw_items()))
            if grant.device_id in self._active or self.provider_factory is None:
                raise PermissionError('device busy or route unavailable')
            self._active.add(grant.device_id); device_id = grant.device_id
            provider = self.provider_factory(grant.device_id)
            expected = 'synthetic_fixture' if self.fixture else 'existing_mira'
            if not isinstance(provider, QualifiedProvider) or provider.identity_verified is not True or provider.kind != expected:
                raise PermissionError('qualified conversation adapter required')
            if type(provider.session_reference) is not str or not 1 <= len(provider.session_reference) <= 128:
                raise PermissionError('verified conversation reference required')
            session = StreamingSession(self.registry, self.sessions, provider.bridge,
                grant.credential, grant.device_id, provider.arm,
                fixture=self.fixture, native_session_ref=provider.session_reference)
            driver_started = True
            await run_connection(socket, session)
        except Exception:
            # No token, raw media or provider exception is exposed to the peer.
            await socket.close(code=1008, reason='Companion route closed')
        finally:
            try:
                if session is not None and not driver_started:session.close()
                if provider is not None and isinstance(provider, QualifiedProvider):provider.close()
            finally:
                if device_id is not None:self._active.discard(device_id)

    def listener(self, *, host='127.0.0.1', port=8766, ssl_context=None, trusted_tls_proxy=False):
        """Return a lazy serve context. Only entering it binds a socket.
        Production requires TLS or an explicitly verified proxy on loopback.
        Fixture mode can bind unencrypted localhost only; firmware rejects it.
        """
        if type(port) is not int or not 0 <= port <= 65535 or (port == 0 and not self.fixture):
            raise ValueError('explicit listener port required')
        if self.fixture:
            if host != '127.0.0.1' or ssl_context is not None or trusted_tls_proxy:
                raise ValueError('fixture listener is localhost only')
        elif ssl_context is None and not (trusted_tls_proxy and host == '127.0.0.1'):
            raise ValueError('verified TLS boundary required')
        elif trusted_tls_proxy and host != '127.0.0.1':
            raise ValueError('trusted proxy requires loopback bind')
        from websockets.asyncio.server import serve
        return serve(self.handle, host, port, ssl=ssl_context,
            process_request=self.process_request, max_size=4112, max_queue=2,
            compression=None, ping_interval=None, open_timeout=2,
            close_timeout=.25, server_header=None)
