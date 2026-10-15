"""Synthetic trust pins only; no certificate creation or listeners."""
import importlib.util
import os
from pathlib import Path
import json
import tempfile
import unittest

MODULE = Path(__file__).resolve().parents[1] / 'streaming/public_config.py'

def module():
    spec=importlib.util.spec_from_file_location('fixture_config', MODULE)
    result=importlib.util.module_from_spec(spec);spec.loader.exec_module(result)
    return result

class PublicConfigChecks(unittest.TestCase):
    def setUp(self):
        self.previous=os.environ.pop('STACKCHAN_BINDING_CONFIG',None)
    def tearDown(self):
        os.environ.pop('STACKCHAN_BINDING_CONFIG',None)
        if self.previous is not None:os.environ['STACKCHAN_BINDING_CONFIG']=self.previous
    def test_unconfigured_is_unavailable(self):
        config=module()
        self.assertEqual((config.DEVICE,config.HOST,config.EXPIRY),('','',0))
        with self.assertRaises(ValueError):config.require()
    def test_owner_only_fixture_and_exact_endpoint(self):
        fixture=dict(device='stackchan-001122334455',host='192.168.42.10',port=18443,uri='wss://192.168.42.10:18443/companion/audio',ca_sha256='a'*64,expiry=2000000000,cert_sha256='b'*64,cert_begin=1900000000,cert_end=2000000000,native_library_sha256='c'*64)
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'bindings.json';path.write_text(json.dumps(fixture));path.chmod(0o600)
            os.environ['STACKCHAN_BINDING_CONFIG']=str(path);config=module();config.require()
            self.assertEqual(config.CA_HASH,'a'*64)
            fixture['uri']='wss://192.168.42.11:18443/companion/audio';path.write_text(json.dumps(fixture))
            with self.assertRaises(ValueError):module().require()
            fixture['uri']='wss://192.168.42.10:18443/companion/audio';path.write_text(json.dumps(fixture));path.chmod(0o644)
            with self.assertRaises(ValueError):module().require()
    def test_duplicate_config_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            path=Path(tmp)/'bindings.json';path.write_text('{"device":"one","device":"two"}');path.chmod(0o600)
            os.environ['STACKCHAN_BINDING_CONFIG']=str(path)
            with self.assertRaises(ValueError):module().require()

if __name__=='__main__':unittest.main()
