#include "screensaver_setup.hpp"
#include <apps/app_launcher/view/screensaver_styles.hpp>
#include <apps/app_launcher/view/slideshow_policy.hpp>
#include <settings.h>
#include <nvs.h>
#ifdef STACKCHAN_COMPANION_BOOT
#include <companion/settings_policy.hpp>
#endif
#include <esp_heap_caps.h>
#include <algorithm>
#include <array>
#include <cstring>
namespace setup_workers {
namespace {
class ScreensaverWorker final:public WorkerBase {
    view::saver::Selection selection_;
    lv_obj_t *screen_=nullptr,*root_=nullptr,*title_=nullptr,*canvas_=nullptr,*toggle_=nullptr,*order_=nullptr,*count_=nullptr;
    uint16_t *pixels_=nullptr,*thumbnail_=nullptr;
    bool preview_failed_=false;
    unsigned id_=0;uint32_t started_=0,last_=0;int action_=0;
    struct Callback {ScreensaverWorker* self;int action;};
    std::array<Callback,7> callbacks_{};unsigned callback_count_=0;
    lv_obj_t* button(const char* text,int x,int y,int w,int action){
        auto* b=lv_button_create(root_);lv_obj_set_pos(b,x,y);lv_obj_set_size(b,w,28);
        auto* label=lv_label_create(b);lv_label_set_text(label,text);lv_obj_center(label);
        auto& cb=callbacks_[callback_count_++];cb={this,action};
        lv_obj_add_event_cb(b,[](lv_event_t* e){auto* c=static_cast<Callback*>(lv_event_get_user_data(e));c->self->action_=c->action;},LV_EVENT_CLICKED,&cb);
        return label;
    }
    void labels(){
        const auto style=static_cast<view::saver::Style>(id_);
        lv_label_set_text_fmt(title_,"%u / %u  %s",view::saver::AllowedOrdinal(id_),view::saver::AllowedCount,view::saver::Name(style));
        lv_label_set_text(toggle_,selection_.selected(id_)?"Deselect":"Select");
        lv_label_set_text(order_,selection_.order==view::saver::Order::Random?"Random":"Sequential");
        if(preview_failed_)lv_label_set_text(count_,"Preview unavailable");
        else if(selection_.count())lv_label_set_text_fmt(count_,"%u selected",selection_.count());
        else lv_label_set_text(count_,"Empty: Save uses DVD");
    }
    void restart(){preview_failed_=false;if(canvas_)lv_obj_remove_flag(canvas_,LV_OBJ_FLAG_HIDDEN);started_=GetHAL().millis();if(id_)effect_catalog::Reset(uint16_t(id_-1));labels();}
    bool save(){
        selection_.normalize();
        // A single bounded versioned key stores both set and order. Checked
        // NVS calls avoid the legacy Settings setter's abort-on-error behavior.
#ifdef STACKCHAN_COMPANION_BOOT
        if(!companion::SettingsWriteAllowed("screensaver","playlist"))return false;
#endif
        const auto encoded=selection_.payload();nvs_handle_t handle=0;
        if(nvs_open("screensaver",NVS_READWRITE,&handle)!=ESP_OK)return false;
        auto error=nvs_set_str(handle,"playlist",encoded.c_str());
        if(error==ESP_OK)error=nvs_commit(handle);
        nvs_close(handle);if(error!=ESP_OK)return false;
        if(nvs_open("screensaver",NVS_READONLY,&handle)!=ESP_OK)return false;
        std::array<char,128> check{};size_t length=check.size();
        error=nvs_get_str(handle,"playlist",check.data(),&length);nvs_close(handle);
        return error==ESP_OK&&length==encoded.size()+1&&encoded==check.data();
    }
    void preview(uint32_t elapsed){
        if(preview_failed_||!pixels_||!thumbnail_||!canvas_)return;
        // Complete the frame before scaling it. The old wall-clock cutoff could
        // expire during clears and permanently hide every otherwise valid preview.
        if(id_){if(!effect_catalog::Render(uint16_t(id_-1),elapsed,pixels_,320,240)){preview_failed_=true;lv_obj_add_flag(canvas_,LV_OBJ_FLAG_HIDDEN);lv_label_set_text(count_,"Preview unavailable");return;}}
        else {
            // Local preview of the stock DVD slide, without loading a screen.
            std::fill_n(pixels_,320*240,uint16_t(0));
            unsigned x=(elapsed/10)%512,y=(elapsed/13)%384;if(x>256)x=512-x;if(y>192)y=384-y;
            for(unsigned row=0;row<48;++row)for(unsigned col=0;col<64;++col){
                bool eye=row>=17&&row<23&&((col>=14&&col<20)||(col>=44&&col<50));
                bool mouth=row>=29&&row<31&&col>=23&&col<41;
                pixels_[(y+row)*320+x+col]=(eye||mouth)?0:0xffff;
            }
        }
        for(unsigned y=0;y<120;++y)for(unsigned x=0;x<160;++x)thumbnail_[y*160+x]=pixels_[(y*2)*320+x*2];
        lv_obj_invalidate(canvas_);
    }
public:
    ScreensaverWorker(){
        Settings settings("screensaver");selection_=view::saver::Selection::fromPayload(settings.GetString("playlist"));
        id_=selection_.first();screen_=lv_screen_active();root_=lv_obj_create(screen_);
        lv_obj_set_pos(root_,0,24);lv_obj_set_size(root_,320,192);lv_obj_set_style_pad_all(root_,0,0);lv_obj_set_style_border_width(root_,0,0);lv_obj_set_style_radius(root_,0,0);
        lv_obj_set_style_bg_color(root_,lv_color_hex(0xEDF4FF),0);lv_obj_set_style_bg_opa(root_,LV_OPA_COVER,0);lv_obj_remove_flag(root_,LV_OBJ_FLAG_SCROLLABLE);
        title_=lv_label_create(root_);lv_obj_set_pos(title_,8,3);lv_obj_set_width(title_,304);lv_label_set_long_mode(title_,LV_LABEL_LONG_DOT);
        pixels_=static_cast<uint16_t*>(heap_caps_malloc(320*240*2,MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT));
        thumbnail_=static_cast<uint16_t*>(heap_caps_malloc(160*120*2,MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT));
        if(pixels_&&thumbnail_){std::memset(pixels_,0,320*240*2);std::memset(thumbnail_,0,160*120*2);canvas_=lv_canvas_create(root_);lv_canvas_set_buffer(canvas_,thumbnail_,160,120,LV_COLOR_FORMAT_RGB565);lv_obj_set_pos(canvas_,8,27);lv_obj_remove_flag(canvas_,static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE));}
        toggle_=button("Select",178,27,134,1);button("Only this",178,59,134,2);button("Select all",178,91,134,3);order_=button("Sequential",178,123,134,4);
        button("<",8,157,45,5);button(">",58,157,45,6);button("Save",234,157,78,7);
        count_=lv_label_create(root_);lv_obj_set_pos(count_,108,159);lv_obj_set_size(count_,122,28);lv_obj_set_style_text_font(count_,LV_FONT_DEFAULT,0);
        restart();preview(0);
        if(!pixels_||!thumbnail_)lv_label_set_text(count_,"Preview unavailable");
    }
    ~ScreensaverWorker() override {
        // Delete canvas and every callback before freeing its borrowed pixels.
        if(root_&&lv_obj_is_valid(root_))lv_obj_delete(root_);
        if(pixels_)heap_caps_free(pixels_);
        if(thumbnail_)heap_caps_free(thumbnail_);
    }
    void update() override {
        if(!root_||lv_screen_active()!=screen_||!lv_obj_is_valid(root_)||!lv_obj_is_visible(root_))return;
        const int action=action_;action_=0;
        if(action==1)selection_.set(id_,!selection_.selected(id_));
        else if(action==2){selection_.bits.fill(0);selection_.set(id_,true);}
        else if(action==3){auto order=selection_.order;selection_=view::saver::Selection::defaults();selection_.order=order;}
        else if(action==4)selection_.order=selection_.order==view::saver::Order::Random?view::saver::Order::Sequential:view::saver::Order::Random;
        else if(action==5||action==6){id_=view::saver::AdjacentAllowed(id_,action==6);restart();}
        else if(action==7){if(save())_is_done=true;else lv_label_set_text(count_,"Save failed");return;}
        if(action)labels();
        const uint32_t now=GetHAL().millis();if(uint32_t(now-last_)>=60){last_=now;preview(uint32_t(now-started_));}
    }
};
}
std::unique_ptr<WorkerBase> MakeScreensaverWorker(){return std::make_unique<ScreensaverWorker>();}
}
