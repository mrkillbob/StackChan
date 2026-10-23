"""Execute generated production activation and audio-open paths with inert boundaries."""
from pathlib import Path
import importlib.util
import shutil
import subprocess
import tempfile

firmware = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('backend', firmware / 'main/apps/stock_agent/generate_backend.py')
backend = importlib.util.module_from_spec(spec)
spec.loader.exec_module(backend)


def extract(source, signature):
    start = source.index(signature)
    position = source.index('{', start)
    depth, end = 1, position + 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


code = r'''
#include <cassert>
#include <cstdio>
#include <string>
#define ESP_LOGE(...) ((void)0)
#define ESP_LOGW(...) ((void)0)
#define ESP_LOGI(...) ((void)0)
#define pdMS_TO_TICKS(value) (value)
void vTaskDelay(int){}
using esp_err_t=int;constexpr int ESP_OK=0,ESP_ERR_TIMEOUT=1,kDeviceStateIdle=0,kDeviceStateConnecting=1;
using ListeningMode=int;
namespace Lang {namespace Strings {constexpr auto CHECKING_NEW_VERSION="check",CHECK_NEW_VERSION_FAILED="%d %s",ERROR="error",ACTIVATION="activation";}
namespace Sounds {constexpr auto OGG_EXCLAMATION="sound";}}
namespace stock_services {int offers=0;void OfferFirmwareInstall(const std::string&,const std::string&){++offers;}}
struct SetupRequest{};struct Hal {int setup=0;[[noreturn]]void requestAppSetup(){++setup;throw SetupRequest{};}}hal;
Hal& GetHAL(){return hal;}
enum class PowerSaveLevel{PERFORMANCE,LOW_POWER};
struct Display {void SetStatus(const char*){}};
struct Board {Display display;PowerSaveLevel power=PowerSaveLevel::LOW_POWER;
 static Board& GetInstance(){static Board b;return b;}Display* GetDisplay(){return &display;}
 void SetPowerSaveLevel(PowerSaveLevel value){power=value;}};
struct Ota {bool update=false,binding=false,challenge=false;int activations=0,valid=0;
 int CheckVersion(){return ESP_OK;}std::string GetCheckVersionUrl(){return "test";}
 bool HasNewVersion(){return update;}std::string GetFirmwareUrl(){return "test";}
 std::string GetFirmwareVersion(){return "test";}void MarkCurrentVersionValid(){++valid;}
 bool RequiresAppBinding(){return binding;}bool HasActivationChallenge(){return challenge;}
 int Activate(){++activations;return ESP_OK;}};
struct Protocol {bool opened=false,succeeds=true;int attempts=0;
 bool IsAudioChannelOpened(){return opened;}bool OpenAudioChannel(){++attempts;assert(Board::GetInstance().power==PowerSaveLevel::PERFORMANCE);return succeeds;}};
struct Application {Ota* ota_;Protocol* protocol_;int state=kDeviceStateConnecting,listening=0;
 int GetDeviceState(){return state;}void Alert(const char*,const char*,const char*,const char*){}
 void SetListeningMode(int){++listening;}void CheckNewVersion();void ContinueOpenAudioChannel(ListeningMode);};
'''
cases = r'''
int main(){
 Ota ota;Protocol protocol;Application app{&ota,&protocol};app.CheckNewVersion();assert(ota.valid==1&&!ota.activations);
 ota.update=true;app.CheckNewVersion();assert(stock_services::offers==1); // no automatic install
 ota.update=false;ota.binding=true;bool redirected=false;
 try{app.CheckNewVersion();}catch(SetupRequest&){redirected=true;}assert(redirected&&hal.setup==1&&!ota.activations);
 ota.binding=false;ota.challenge=true;redirected=false;
 try{app.CheckNewVersion();}catch(SetupRequest&){redirected=true;}assert(redirected&&hal.setup==2&&ota.activations==3);
 app.state=kDeviceStateIdle;app.ContinueOpenAudioChannel(0);assert(!protocol.attempts&&!app.listening);
 app.state=kDeviceStateConnecting;app.ContinueOpenAudioChannel(0);assert(protocol.attempts==1&&app.listening==1);
 protocol.succeeds=false;app.ContinueOpenAudioChannel(0);
 assert(protocol.attempts==2&&app.listening==1&&Board::GetInstance().power==PowerSaveLevel::LOW_POWER);
 protocol.opened=true;app.ContinueOpenAudioChannel(0);
 assert(protocol.attempts==2&&app.listening==2&&Board::GetInstance().power==PowerSaveLevel::PERFORMANCE);
}
'''
with tempfile.TemporaryDirectory(prefix='stackchan-vendor-reconciliation-') as temporary:
    directory = Path(temporary)
    backend.generate(firmware / 'xiaozhi-esp32/main', firmware / 'managed_components/78__esp-wifi-connect', directory)
    production = (directory / 'stock_application.cc').read_text()
    cpp, exe = directory / 'vendor.cpp', directory / 'vendor'
    cpp.write_text(code + extract(production, 'void Application::CheckNewVersion()') + '\n' +
                   extract(production, 'void Application::ContinueOpenAudioChannel(') + cases)
    subprocess.run([shutil.which('clang++') or 'c++', '-std=c++17', '-fsanitize=address,undefined',
                    str(cpp), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
print('Generated production paths preserve bounded App activation, offered installs and audio handshake power ordering')
