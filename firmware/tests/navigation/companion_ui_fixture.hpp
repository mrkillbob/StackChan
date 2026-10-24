#pragma once
// Inert LVGL boundary: tracks object ownership, inherited visibility, event
// callbacks and each lock release. No display, transport or hardware is used.
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

struct lv_obj_t;
struct lv_event_t {lv_obj_t* target;void* user;};
struct lv_timer_t {void (*callback)(lv_timer_t*);void* user;bool deleted=false;};
struct lv_obj_t {
 lv_obj_t* parent=nullptr;bool hidden=false,deleted=false;std::string text;
 int x=0,y=0,w=0,h=0;void* user=nullptr;
 void (*callback)(lv_event_t*)=nullptr;void* callback_user=nullptr;
};
inline lv_obj_t screen,top;
inline std::vector<std::unique_ptr<lv_obj_t>> objects;
inline std::vector<std::unique_ptr<lv_timer_t>> timers;
inline std::function<void()> on_unlock;
inline unsigned lock_depth=0;
struct LvglLockGuard {
 LvglLockGuard(){++lock_depth;}
 ~LvglLockGuard(){if(!--lock_depth&&on_unlock)on_unlock();}
};
inline constexpr int LV_OBJ_FLAG_HIDDEN=1,LV_OBJ_FLAG_SCROLLABLE=2,
 LV_EVENT_CLICKED=1,LV_LABEL_LONG_DOT=1,LV_OPA_TRANSP=0,
 LV_STATE_DISABLED=1,lv_font_montserrat_16=16,lv_font_montserrat_14=14;
#define LV_SYMBOL_LIST "LIST_ICON"
inline bool Visible(lv_obj_t* p){return p&&!p->deleted&&!p->hidden&&(!p->parent||Visible(p->parent));}
inline lv_obj_t* lv_obj_create(lv_obj_t* parent){auto p=std::make_unique<lv_obj_t>();p->parent=parent;auto raw=p.get();objects.push_back(std::move(p));return raw;}
inline lv_obj_t* lv_label_create(lv_obj_t* p){return lv_obj_create(p);}
inline lv_obj_t* lv_button_create(lv_obj_t* p){return lv_obj_create(p);}
inline void lv_obj_delete(lv_obj_t* p){assert(p&&!p->deleted);for(auto& child:objects)if(child->parent==p&&!child->deleted)lv_obj_delete(child.get());p->deleted=true;}
inline void lv_obj_remove_flag(lv_obj_t* p,int f){if(f==LV_OBJ_FLAG_HIDDEN)p->hidden=false;}
inline void lv_obj_add_flag(lv_obj_t* p,int f){if(f==LV_OBJ_FLAG_HIDDEN)p->hidden=true;}
inline void lv_obj_set_pos(lv_obj_t* p,int x,int y){p->x=x;p->y=y;}
inline void lv_obj_set_size(lv_obj_t* p,int w,int h){p->w=w;p->h=h;}
inline void lv_obj_set_width(lv_obj_t* p,int w){p->w=w;}
inline void lv_obj_set_height(lv_obj_t* p,int h){p->h=h;}
inline void lv_label_set_text(lv_obj_t* p,const char* text){assert(p&&!p->deleted);p->text=text;}
inline void lv_obj_set_user_data(lv_obj_t* p,void* data){p->user=data;}
inline void* lv_obj_get_user_data(lv_obj_t* p){return p->user;}
inline void* lv_event_get_user_data(lv_event_t* e){return e->user;}
inline lv_obj_t* lv_event_get_target(lv_event_t* e){return e->target;}
inline void lv_obj_add_event_cb(lv_obj_t* p,void (*callback)(lv_event_t*),int,void* user){p->callback=callback;p->callback_user=user;}
inline void Click(lv_obj_t* p){assert(Visible(p)&&p->callback);lv_event_t e{p,p->callback_user};p->callback(&e);}
inline lv_obj_t* Text(const std::string& text){for(auto& p:objects)if(Visible(p.get())&&p->text==text)return p.get();return nullptr;}
inline lv_obj_t* Button(const std::string& text){auto label=Text(text);return label?label->parent:nullptr;}
inline unsigned LiveObjects(){return std::count_if(objects.begin(),objects.end(),[](auto& p){return !p->deleted;});}
inline lv_obj_t* lv_screen_active(){return &screen;}
inline lv_obj_t* lv_layer_top(){return &top;}
inline int lv_color_hex(uint32_t c){return c;}
inline void lv_obj_center(lv_obj_t*){}
inline void lv_label_set_long_mode(lv_obj_t*,int){}
inline void lv_obj_set_style_pad_all(lv_obj_t*,int,int){}
inline void lv_obj_set_style_bg_color(lv_obj_t*,int,int){}
inline void lv_obj_set_style_text_color(lv_obj_t*,int,int){}
inline void lv_obj_set_style_text_font(lv_obj_t*,const int*,int){}
inline void lv_obj_set_style_bg_opa(lv_obj_t*,int,int){}
inline void lv_obj_set_style_border_width(lv_obj_t*,int,int){}
inline void lv_obj_set_style_shadow_width(lv_obj_t*,int,int){}
inline lv_timer_t* lv_timer_create(void (*callback)(lv_timer_t*),int,void* user){auto timer=std::make_unique<lv_timer_t>();timer->callback=callback;timer->user=user;auto p=timer.get();timers.push_back(std::move(timer));return p;}
inline void lv_timer_delete(lv_timer_t* p){assert(!p->deleted);p->deleted=true;}
inline void* lv_timer_get_user_data(lv_timer_t* p){return p->user;}
inline void TickTimers(){for(auto& p:timers)if(!p->deleted)p->callback(p.get());}
