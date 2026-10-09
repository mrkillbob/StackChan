#!/usr/bin/env python3
"""Assert official menu structure and one nested Tools app, without device I/O."""
from pathlib import Path
import subprocess,re,json
repo=Path(__file__).resolve().parents[3]
base="1b5765599fba8aaad1811d9a79358ccc7051f5f3"
def original(name):return subprocess.check_output(["git","-C",str(repo),"show",base+":"+name])
def menu_labels(source):
 source=source[source.index("    _menu_sections = {"):]
 source=source[:source.index("\n    };")]
 source="\n".join(line.split("//",1)[0] for line in source.splitlines())
 return re.findall(r'"([^"\n]*)"',source)
setup="firmware/main/apps/app_setup/app_setup.cpp"
assert menu_labels((repo/setup).read_text())==menu_labels(original(setup).decode())
for directory in ["firmware/main/apps/app_setup/view","firmware/main/apps/app_launcher/view","firmware/main/apps/common"]:
 for name in subprocess.check_output(["git","-C",str(repo),"ls-tree","-r","--name-only",base,"--",directory],text=True).splitlines():
  assert (repo/name).read_bytes()==original(name),name
shell=(repo/"firmware/main/apps/stock_shell/stock_apps.cc").read_text()
registration=shell[shell.index("[[noreturn]] void Run()") :]
assert registration.count("std::make_unique<ToolsApp>()")==1
assert "network_setup::MakeApp()" not in registration and "stock_app_center::MakeApp" not in registration
assert "for(auto feature:LauncherNativeFeatures)" in registration
assert "IsSetupFeature(*feature)" in registration
assert "native_ids.at(" not in registration
order=[registration.index(text) for text in ["stock_agent::MakeApp()","for(auto feature:LauncherNativeFeatures)","std::make_unique<AppSetup>()","std::make_unique<CompanionApp>()","std::make_unique<ToolsApp>()","std::make_unique<GamesApp>()"]]
assert order==sorted(order)
assert "Welcome Setup" not in (repo/setup).read_text()
# Dedicated tool pages are members of the one Tools app, not launcher apps.
tools=shell[shell.index("class ToolsApp"):shell.index("class GamesApp")]
for page in ["Home","Infrared","Learn","Send","Nfc","Edit","Emulate"]:assert "case Screen::"+page+":" in tools
for action in ["OpenInfrared","OpenNfc","OpenLearn","OpenSend","OpenEdit"]:assert action in tools
assert "installApp" not in tools
print(json.dumps({"result":"passed","setup":"official 6 sections/12 rows","launcher":"original seven, then Companion/Tools/Games","tools":"one Tools tile -> selection -> dedicated graphical pages","device_access":False}))
