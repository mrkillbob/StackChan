#!/usr/bin/env python3
"""Embed a reviewed public CA only; no keys, credentials, network or device IO."""
import argparse, ipaddress, json, pathlib, re, subprocess

def generate(ca,host,output):
    if bool(ca)!=bool(host):raise ValueError('CA and host must be configured together')
    pem=''
    if ca:
        address=ipaddress.IPv4Address(host)
        networks=[ipaddress.IPv4Network(n) for n in ('10.0.0.0/8','172.16.0.0/12','192.168.0.0/16')]
        if str(address)!=host or not any(address in n for n in networks):raise ValueError('canonical RFC1918 host required')
        pem=pathlib.Path(ca).read_text(encoding='ascii')
        if len(pem)>16384 or not re.fullmatch(r'-----BEGIN CERTIFICATE-----\s+[A-Za-z0-9+/=\r\n]+-----END CERTIFICATE-----\s*',pem):raise ValueError('one public PEM certificate required; keys are prohibited')
        parsed=subprocess.run(['openssl','x509','-in',str(ca),'-noout','-text'],capture_output=True,text=True,timeout=5)
        if parsed.returncode or 'CA:TRUE' not in parsed.stdout:raise ValueError('valid CA certificate required')
        valid=subprocess.run(['openssl','x509','-in',str(ca),'-noout','-checkend','0'],capture_output=True,text=True,timeout=5)
        if valid.returncode:raise ValueError('expired CA rejected')
    source='// Generated public trust; never contains server keys or grants.\n'
    source+='extern "C" const char* companion_local_ca_pem(){return R"SC_LOCAL_CA('+pem+')SC_LOCAL_CA";}\n'
    source+='extern "C" const char* companion_local_tls_host(){return '+json.dumps(host)+';}\n'
    pathlib.Path(output).parent.mkdir(parents=True,exist_ok=True)
    pathlib.Path(output).write_text(source)

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--ca',default='');p.add_argument('--host',default='');p.add_argument('--output',type=pathlib.Path,required=True)
    a=p.parse_args()
    try:generate(a.ca,a.host,a.output)
    except (ValueError,OSError,UnicodeError,subprocess.TimeoutExpired):p.exit(2,'Local TLS configuration rejected. No key material is accepted.\n')
