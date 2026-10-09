#include <settings.h>
#include <cassert>
#include <companion/audio_owner.hpp>
int writes=0,commits=0,opens=0,closes=0,last_mode=-1;
int main(){
{Settings s("board",true);assert(last_mode==NVS_READONLY);
s.SetString("uuid","new");s.SetInt("volume",10);s.SetBool("boot",true);s.EraseKey("pairing");s.EraseAll();
assert(s.GetString("missing","kept")=="kept");assert(s.GetInt("missing",42)==42);assert(s.GetBool("missing",true));}
assert(opens==1 && closes==1 && writes==0 && commits==0);
{Settings s("mqtt",true);
 s.SetString("endpoint","blocked");assert(s.GetString("endpoint","none")=="none");
 assert(companion::AcquireAudio(companion::AudioOwner::Stock));
 s.SetString("endpoint","session-only");s.SetInt("port",8883);
 assert(s.GetString("endpoint")=="session-only"&&s.GetInt("port")==8883);
 Settings board("board",true);board.SetString("uuid","still-blocked");
 assert(board.GetString("uuid","kept")=="kept");
 companion::ReleaseAudio(companion::AudioOwner::Stock);
 s.SetString("endpoint","blocked-after-stop");assert(s.GetString("endpoint")=="session-only");
}
assert(writes==0&&commits==0&&opens==closes);
}
