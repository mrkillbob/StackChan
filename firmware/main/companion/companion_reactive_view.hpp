#pragma once
#include "desk_reactive_avatar.hpp"
#include <lvgl.h>
#include <cstring>
#include <cstdio>
namespace companion::desk {
// Pure companion-view face: no native Avatar exists in initCompanion. Its
// objects never alter any native owner's Avatar modify lock or speech slot.
// Every method is called only inside the existing HAL LvglLockGuard.
class CompanionReactiveView {
    lv_obj_t* root_=nullptr;lv_obj_t* left_=nullptr;lv_obj_t* right_=nullptr;lv_obj_t* mouth_=nullptr;lv_obj_t* privacy_=nullptr;
    static bool Is(const char* a,const char* b){return std::strcmp(a,b)==0;}
    static lv_obj_t* Part(lv_obj_t* parent,int x,int y,int w,int h){
        auto* p=lv_obj_create(parent);lv_obj_remove_style_all(p);lv_obj_set_pos(p,x,y);lv_obj_set_size(p,w,h);
        lv_obj_set_style_bg_color(p,lv_color_hex(0x94e1dd),0);lv_obj_set_style_bg_opa(p,LV_OPA_COVER,0);lv_obj_set_style_radius(p,4,0);return p;
    }
public:
    void Detach(){root_=left_=right_=mouth_=privacy_=nullptr;}
    void Open(lv_obj_t* parent){
        root_=lv_obj_create(parent);lv_obj_remove_style_all(root_);lv_obj_set_pos(root_,236,80);lv_obj_set_size(root_,76,70);lv_obj_add_flag(root_,LV_OBJ_FLAG_HIDDEN);
        left_=Part(root_,16,16,6,12);right_=Part(root_,50,16,6,12);mouth_=Part(root_,28,48,20,2);
        privacy_=lv_label_create(parent);lv_obj_set_pos(privacy_,8,30);lv_obj_set_size(privacy_,176,14);lv_obj_set_style_text_font(privacy_,&lv_font_montserrat_14,0);
        lv_label_set_text(privacy_,"Mic? Out?");
    }
    void Render(const ReactiveFrame& f,bool comparing){
        if(!root_||!privacy_)return;
        const char* mic=Is(f.status.mic,"Mic off confirmed")?"Mic off":Is(f.status.mic,"Listening: capture active")?"Mic on":"Mic?";
        const char* out=Is(f.status.output,"Output off confirmed")?"Out off":Is(f.status.output,"Playback active")?"Out on":"Out?";
        const char* stop=Is(f.status.stop,"Stop pending")?" Stopping":Is(f.status.stop,"Stop unconfirmed")?" Stop?":"";
        char status[64];std::snprintf(status,sizeof(status),"%s %s%s",mic,out,stop);lv_label_set_text(privacy_,status);
        if(!f.apply_face||comparing){lv_obj_add_flag(root_,LV_OBJ_FLAG_HIDDEN);return;}
        lv_obj_remove_flag(root_,LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(left_,16+f.eye_x,16+f.eye_y);lv_obj_set_pos(right_,50+f.eye_x,16+f.eye_y);
        lv_obj_set_size(mouth_,20,2+f.mouth_weight/5);
    }
};
}
