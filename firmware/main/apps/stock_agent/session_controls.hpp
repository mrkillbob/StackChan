#pragma once
#include <lvgl.h>
namespace stock_agent {
// UI-only disclosure. Call Create/Destroy while holding the LVGL lock.
// Stock avatar/status/permission indicators and provider lifecycle are untouched.
class CleanSessionControls {
 lv_obj_t* reveal_=nullptr;lv_obj_t* menu_=nullptr;lv_obj_t* status_=nullptr;
 lv_timer_t* timer_=nullptr;void (*stop_)()=nullptr;const char* (*read_status_)()=nullptr;
 void Show(bool visible){if(visible){lv_obj_remove_flag(menu_,LV_OBJ_FLAG_HIDDEN);lv_obj_add_flag(reveal_,LV_OBJ_FLAG_HIDDEN);}else{lv_obj_add_flag(menu_,LV_OBJ_FLAG_HIDDEN);lv_obj_remove_flag(reveal_,LV_OBJ_FLAG_HIDDEN);}}
 static void Label(lv_obj_t* parent,const char* text){auto l=lv_label_create(parent);lv_label_set_text(l,text);lv_obj_center(l);}
public:
 void Destroy(){if(timer_)lv_timer_delete(timer_);timer_=nullptr;if(menu_)lv_obj_delete(menu_);if(reveal_)lv_obj_delete(reveal_);menu_=status_=reveal_=nullptr;stop_=nullptr;read_status_=nullptr;}
 void Create(void (*stop)(),const char* (*read_status)()){
  Destroy();stop_=stop;read_status_=read_status;
  reveal_=lv_button_create(lv_layer_top());lv_obj_set_pos(reveal_,284,4);lv_obj_set_size(reveal_,32,28);lv_obj_set_style_bg_opa(reveal_,LV_OPA_TRANSP,0);lv_obj_set_style_border_width(reveal_,0,0);lv_obj_set_style_shadow_width(reveal_,0,0);Label(reveal_,LV_SYMBOL_LIST);
  lv_obj_add_event_cb(reveal_,[](lv_event_t* e){static_cast<CleanSessionControls*>(lv_event_get_user_data(e))->Show(true);},LV_EVENT_CLICKED,this);
  menu_=lv_obj_create(lv_layer_top());lv_obj_set_pos(menu_,0,0);lv_obj_set_size(menu_,320,84);lv_obj_set_style_pad_all(menu_,0,0);lv_obj_remove_flag(menu_,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(menu_,lv_color_hex(0x10232d),0);lv_obj_set_style_text_color(menu_,lv_color_hex(0xffffff),0);lv_obj_set_style_text_font(menu_,&lv_font_montserrat_16,0);
  status_=lv_label_create(menu_);lv_obj_set_size(status_,244,32);lv_obj_set_pos(status_,5,6);lv_label_set_long_mode(status_,LV_LABEL_LONG_DOT);lv_label_set_text(status_,read_status_?read_status_():"Stock AI");
  auto hide=lv_button_create(menu_);lv_obj_set_pos(hide,252,2);lv_obj_set_size(hide,60,32);Label(hide,"Hide");lv_obj_add_event_cb(hide,[](lv_event_t* e){static_cast<CleanSessionControls*>(lv_event_get_user_data(e))->Show(false);},LV_EVENT_CLICKED,this);
  auto stop_button=lv_button_create(menu_);lv_obj_set_pos(stop_button,80,40);lv_obj_set_size(stop_button,160,40);Label(stop_button,"Stop / Home");lv_obj_add_event_cb(stop_button,[](lv_event_t* e){auto self=static_cast<CleanSessionControls*>(lv_event_get_user_data(e));if(self->stop_)self->stop_();},LV_EVENT_CLICKED,this);
  Show(false);
  timer_=lv_timer_create([](lv_timer_t* timer){auto self=static_cast<CleanSessionControls*>(lv_timer_get_user_data(timer));if(self->status_&&self->read_status_)lv_label_set_text(self->status_,self->read_status_());},40,this);
 }
};
}
