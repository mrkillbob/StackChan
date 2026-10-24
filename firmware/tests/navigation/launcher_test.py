"""Execute the real launcher methods with UI/hardware boundaries replaced by inert fixtures."""
from pathlib import Path
import subprocess, tempfile, sys
root=Path(__file__).resolve().parents[2]/'main'
source=Path(sys.argv[1]).read_text() if len(sys.argv)>1 else (root/'apps/app_launcher/app_launcher.cpp').read_text()
methods=[]
for name in ('onLauncherOpen','onLauncherRunning','onLauncherClose','create_launcher_view','handle_category_navigation'):
 start=source.index('void AppLauncher::'+name+'(');begin=source.index('{',start);depth=1;end=begin+1
 while depth:
  depth+=(source[end]=='{')-(source[end]=='}');end+=1
 methods.append(source[start:end])
fixture=r'''
#include "apps/app_launcher/category_routes.hpp"
#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <cstdio>
struct LvglLockGuard {};
namespace mclog {template<class... T>void tagInfo(T...) {}}
struct lv_obj_t{};lv_obj_t screen;lv_obj_t* lv_screen_active(){return &screen;}
namespace mooncake {struct AppProps_t {struct Info{std::string name;}info;int appID;};}
struct View {std::vector<mooncake::AppProps_t> shown;std::function<void(int)> onAppClicked;int next=-1;
 void init(std::vector<mooncake::AppProps_t> p){shown=std::move(p);}void update(){if(next!=-1){auto id=next;next=-1;onAppClicked(id);}}};
namespace view {using LauncherView=View;}
struct Startup{void update(){}bool isDone(){return true;}};
namespace setup_workers{using StartupWorker=Startup;}
struct StackChan{void update(){}};StackChan& GetStackChan(){static StackChan s;return s;}
struct AppLauncher {
 std::unique_ptr<View> _view;std::unique_ptr<int> _screensaver;std::unique_ptr<Startup> _startup_worker;
 std::optional<launcher_categories::Group> _category;lv_obj_t* _launcher_screen=nullptr;
 int _pending_navigation=-1;unsigned _screensaver_timecount=0;bool _startup_checked=false;
 int opened=-1;
 std::vector<mooncake::AppProps_t> installed={{{"Mira"},1},{{"Stackchan"},2},{{"USB CONTROLS"},3},{{"Settings"},4},{{"AVATAR"},5},{{"GAMES"},6}};
 auto getAppProps(){return installed;}auto getAppInfo(){return installed.front().info;}
 void openApp(int id){opened=id;}void screensaver_update(){}
 void onLauncherOpen();void onLauncherRunning();void onLauncherClose();void create_launcher_view();void handle_category_navigation();
};
'''
checks=r'''
int main(){AppLauncher l;for(int round=0;round<10;++round){
 l.onLauncherOpen();assert(!l._category);assert(l._view->shown.front().info.name=="Companions");
 l._view->next=launcher_categories::Token(launcher_categories::Group::Companions);l.onLauncherRunning();
 assert(l._category==launcher_categories::Group::Companions);assert(l._view->shown.size()==3);
 assert(l._view->shown[0].info.name=="Mira"&&l._view->shown[1].info.name=="Stackchan");
 l.onLauncherRunning();assert(l._category==launcher_categories::Group::Companions);
 l._view->next=round%2?1:2;l.onLauncherRunning();assert(l.opened==(round%2?1:2));
 l.onLauncherClose();assert(!l._view);l.onLauncherOpen();assert(!l._category);
 assert(l._view->shown.front().info.name=="Companions");l.onLauncherClose();
 }std::puts("10 real launcher method cycles passed");}
'''
with tempfile.TemporaryDirectory(prefix='stackchan-launcher-test-') as tmp:
 p=Path(tmp);(p/'test.cpp').write_text(fixture+'\n'+'\n'.join(methods)+'\n'+checks)
 subprocess.run(['clang++','-std=c++17','-DSTACKCHAN_COMPANION_BOOT=1','-fsanitize=address,undefined','-I'+str(root),str(p/'test.cpp'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
