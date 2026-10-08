#include <settings.h>
#include <cassert>
int writes=0,commits=0,opens=0,closes=0,last_mode=-1;
int main(){
{Settings s("board",true);assert(last_mode==NVS_READONLY);
s.SetString("uuid","new");s.SetInt("volume",10);s.SetBool("boot",true);s.EraseKey("pairing");s.EraseAll();
assert(s.GetString("missing","kept")=="kept");assert(s.GetInt("missing",42)==42);assert(s.GetBool("missing",true));}
assert(opens==1 && closes==1 && writes==0 && commits==0);
}
