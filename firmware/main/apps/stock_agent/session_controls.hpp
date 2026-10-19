#pragma once
#include <lvgl.h>
namespace stock_agent {
// UI-only Home/status footer. Call Create/Destroy while holding the LVGL lock.
// Stock avatar/status/permission indicators and provider lifecycle are untouched.
class CleanSessionControls {
 lv_obj_t* footer_=nullptr;lv_obj_t* status_=nullptr;
 lv_timer_t* timer_=nullptr;void (*stop_)()=nullptr;const char* (*read_status_)()=nullptr;
 static void Label(lv_obj_t* parent,const char* text){auto l=lv_label_create(parent);lv_label_set_text(l,text);lv_obj_center(l);}
public:
 void Destroy(){if(timer_)lv_timer_delete(timer_);timer_=nullptr;if(footer_)lv_obj_delete(footer_);footer_=status_=nullptr;stop_=nullptr;read_status_=nullptr;}
 void Create(void (*stop)(),const char* (*read_status)()){
  Destroy();stop_=stop;read_status_=read_status;
  footer_=lv_obj_create(lv_layer_top());lv_obj_set_pos(footer_,0,196);lv_obj_set_size(footer_,320,44);lv_obj_set_style_pad_all(footer_,0,0);lv_obj_remove_flag(footer_,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_border_width(footer_,0,0);lv_obj_set_style_bg_color(footer_,lv_color_hex(0x10232d),0);lv_obj_set_style_text_color(footer_,lv_color_hex(0xffffff),0);lv_obj_set_style_text_font(footer_,&lv_font_montserrat_14,0);
  status_=lv_label_create(footer_);lv_obj_set_size(status_,182,36);lv_obj_set_pos(status_,134,4);lv_label_set_long_mode(status_,LV_LABEL_LONG_DOT);lv_label_set_text(status_,read_status_?read_status_():"Stackchan");
  auto stop_button=lv_button_create(footer_);lv_obj_set_pos(stop_button,4,2);lv_obj_set_size(stop_button,124,40);Label(stop_button,"Stop / Home");lv_obj_add_event_cb(stop_button,[](lv_event_t* e){auto self=static_cast<CleanSessionControls*>(lv_event_get_user_data(e));if(self->stop_)self->stop_();},LV_EVENT_CLICKED,this);
  timer_=lv_timer_create([](lv_timer_t* timer){auto self=static_cast<CleanSessionControls*>(lv_timer_get_user_data(timer));if(self->status_&&self->read_status_)lv_label_set_text(self->status_,self->read_status_());},40,this);
 }
};
}
