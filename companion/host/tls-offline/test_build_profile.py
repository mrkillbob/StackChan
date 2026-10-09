import unittest, subprocess, tempfile, pathlib
ROOT=pathlib.Path(__file__).resolve().parents[3]
VALIDATOR=ROOT/'companion/validate_local_profile.cmake'
GENERATOR=ROOT/'companion/configure_local_tls.py'
# Public CA fixture distributed with the pinned official ESP-IDF SDK.
CA=ROOT.parents[1]/'.tools/sdk/esp-idf-v5.5.4/components/mbedtls/mbedtls/framework/data_files/test-ca.crt'
class BuildProfile(unittest.TestCase):
 def check_profile(self, **values):
  with tempfile.TemporaryDirectory() as d:
   script=pathlib.Path(d)/'profile.cmake'
   script.write_text('\n'.join('set('+k+' "'+str(v)+'")' for k,v in values.items())+'\ninclude("'+str(VALIDATOR)+'")\n')
   return subprocess.run(['cmake','-P',str(script)],capture_output=True,text=True)
 def test_foreign_console_cannot_be_built_as_pairing_owner(self):
  result=self.check_profile(STACKCHAN_COMPANION_TRUSTED_USB_PROVISIONING='ON',CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG='ON')
  self.assertNotEqual(result.returncode,0)
  self.assertIn('USB console',result.stderr)
 def test_disabled_pairing_preserves_existing_console(self):
  self.assertEqual(self.check_profile(CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG='ON').returncode,0)
 def test_primary_uart_and_no_secondary_console_allows_pairing(self):
  self.assertEqual(self.check_profile(STACKCHAN_COMPANION_TRUSTED_USB_PROVISIONING='ON',CONFIG_ESP_CONSOLE_UART_DEFAULT='ON',CONFIG_ESP_CONSOLE_SECONDARY_NONE='ON').returncode,0)
 def test_local_ca_requires_certificate_dates(self):
  result=self.check_profile(STACKCHAN_COMPANION_LOCAL_CA_FILE=str(CA),STACKCHAN_COMPANION_LOCAL_TLS_HOST='192.168.1.75')
  self.assertNotEqual(result.returncode,0);self.assertIn('certificate date',result.stderr)
 def test_only_one_local_parameter_cannot_downgrade_to_public_roots(self):
  result=self.check_profile(STACKCHAN_COMPANION_LOCAL_TLS_HOST='192.168.1.75')
  self.assertNotEqual(result.returncode,0);self.assertIn('together',result.stderr)
 def test_approved_profile_passes(self):
  self.assertEqual(self.check_profile(STACKCHAN_COMPANION_LOCAL_CA_FILE=str(CA),STACKCHAN_COMPANION_LOCAL_TLS_HOST='192.168.1.75',CONFIG_MBEDTLS_HAVE_TIME_DATE='ON',STACKCHAN_COMPANION_TRUSTED_USB_PROVISIONING='ON',CONFIG_ESP_CONSOLE_UART_DEFAULT='ON',CONFIG_ESP_CONSOLE_SECONDARY_NONE='ON').returncode,0)
 def generate(self,ca,host,output):
  return subprocess.run(['python3',str(GENERATOR),'--ca',str(ca),'--host',host,'--output',str(output)],capture_output=True,text=True)
 def test_generated_accessors_have_selected_public_ca_and_host(self):
  with tempfile.TemporaryDirectory() as d:
   output=pathlib.Path(d)/'trust.cc';result=self.generate(CA,'192.168.1.75',output)
   self.assertEqual(result.returncode,0,result.stderr)
   driver=pathlib.Path(d)/'read.cc';driver.write_text('#include <iostream>\nextern "C" const char* companion_local_ca_pem();\nextern "C" const char* companion_local_tls_host();\nint main(){std::cout<<companion_local_tls_host()<<"\\n"<<companion_local_ca_pem();}')
   binary=pathlib.Path(d)/'read';subprocess.run(['clang++',str(driver),str(output),'-o',str(binary)],check=True)
   read=subprocess.check_output([str(binary)],text=True)
   self.assertEqual(read,'192.168.1.75\n'+CA.read_text())
 def test_key_material_cannot_enter_firmware(self):
  with tempfile.TemporaryDirectory() as d:
   bad=pathlib.Path(d)/'not-a-ca.pem';bad.write_text('-----BEGIN PRIVATE KEY-----\nnot-a-real-key\n-----END PRIVATE KEY-----\n')
   output=pathlib.Path(d)/'trust.cc';result=self.generate(bad,'192.168.1.75',output)
   self.assertNotEqual(result.returncode,0);self.assertFalse(output.exists())
 def test_noncanonical_or_public_address_is_rejected(self):
  for host in ['192.168.001.75','127.0.0.1','8.8.8.8','192.168.1.75/evil','192.168.1.75:18443']:
   with self.subTest(host=host),tempfile.TemporaryDirectory() as d:
    output=pathlib.Path(d)/'trust.cc';self.assertNotEqual(self.generate(CA,host,output).returncode,0);self.assertFalse(output.exists())
if __name__=='__main__':unittest.main()
