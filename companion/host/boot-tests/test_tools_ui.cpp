#include "tools_ui.hpp"
#include "audio_owner.hpp"
#include <cassert>
#include <set>
using namespace stock_tools::ui;
bool inside(Rect r){return r.x>=0&&r.y>=0&&r.x+r.w<=320&&r.y+r.h<=240;}
bool overlap(Rect a,Rect b){return a.x<b.x+b.w&&b.x<a.x+a.w&&a.y<b.y+b.h&&b.y<a.y+a.h;}
int main(){
 for(auto r:{BackButton,RowOne,RowTwo,Primary,EditButton,EmulateButton,UrlInput})assert(inside(r)&&r.w>=44&&r.h>=44);
 assert(inside(EmulateDetail)&&!overlap(EmulateDetail,Primary));
 assert(!overlap(Rect{8,108,304,38},Rect{8,148,304,24}));
 assert(!overlap(Rect{8,120,304,24},Rect{8,148,304,24}));
 assert(!overlap(RowOne,RowTwo));assert(!overlap(EditButton,EmulateButton));
 for(int i=0;i<10;++i){auto a=KeyRect(i);assert(inside(a)&&a.w>=44&&a.h>=44);assert(!overlap(a,UrlInput));for(auto f:EditorFooter)assert(!overlap(a,f));for(int j=i+1;j<10;++j)assert(!overlap(a,KeyRect(j)));}
 for(auto f:EditorFooter)assert(inside(f)&&f.w>=44&&f.h>=44);
 std::set<char> keys;for(auto page:KeyPages){assert(page.size()<=10);for(char c:page){assert(keys.insert(c).second);assert(Allowed(Screen::Edit,Character+c));}}
 for(char c:std::string_view("https://example.com/A?x=1&y=2#tag"))assert(keys.count(c));
 assert(Parent(Screen::Learn)==Screen::Infrared&&Parent(Screen::Send)==Screen::Infrared);
 assert(Parent(Screen::Edit)==Screen::Nfc&&Parent(Screen::Emulate)==Screen::Nfc);
 for(auto s:{Screen::Home,Screen::Infrared,Screen::Learn,Screen::Send,Screen::Nfc,Screen::Edit,Screen::Emulate})assert(Allowed(s,Back));
 assert(!Allowed(Screen::Home,StartLearn)&&!Allowed(Screen::Edit,StartEmulate)&&!Allowed(Screen::Send,StartLearn));
 using namespace companion;
 assert(CurrentAudioOwner()==AudioOwner::None);assert(AcquireAudio(AudioOwner::Mira));assert(!AcquireAudio(AudioOwner::Stock));
 ReleaseAudio(AudioOwner::Stock);assert(CurrentAudioOwner()==AudioOwner::Mira);
 ReleaseAudio(AudioOwner::Mira);assert(AcquireAudio(AudioOwner::Stock));assert(!AcquireAudio(AudioOwner::Mira));ReleaseAudio(AudioOwner::Stock);
}
