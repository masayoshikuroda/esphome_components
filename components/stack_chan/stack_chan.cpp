#include "stack_chan.h"
#include "esphome/components/display/display.h"
#include "esphome/core/color.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include <algorithm>
#include <stdlib.h>

namespace esphome {
namespace stack_chan {

using namespace display;

static const char *const TAG = "stack_chan";

void StackChan::setup() {
    ESP_LOGCONFIG(TAG, "setup() called");
    srandom(millis());
}

void StackChan::dump_config() {
    ESP_LOGCONFIG(TAG, "dump_config() called");
}

void StackChan::draw(Display &it) {
    int w = it.get_width();
    int h = it.get_height();

    // ==== Mouse Open Level ====
    float mouse_level = h /2 * mouth_open_ratio_/100.0;
    bool idle = mouth_open_ratio_ < 1;

    // ===== Face Shake =====
    float fx = 0;
    float fy = 0;
    if (idle) {
        float tx = millis()/1000.0/face_shake_duration_;
        float ty = millis()/1000.0/(60.0/face_respiratory_rate_);
        fx = sin(tx * 2*M_PI) * w * face_shake_x_/100.0;
        fy = cos(ty * 2*M_PI) * h * face_shake_y_/100.0;
    }

    // ===== Eye Blink =====
    uint32_t now = millis();
    if (idle) {
        // 瞬き開始
        if (!blinking_ && now >= next_blink_) {
            blinking_ = true;
            blink_end_ = now + eye_blink_duration_;   // 閉じている時間
        }
        // 瞬き終了
        if (blinking_ && now >= blink_end_) {
            blinking_ = false;
            long r = random() % 1000;  // 0–999
            next_blink_ = now + 2 * eye_blink_interval_ * r;
        }
    } else {
        blinking_ = false;
    }

    // ===== Face Position =====
    int cx = w / 2 + w * face_offset_x_/100 + (int)fx;
    int cy = h / 2 + h * face_offset_y_/100 + (int)fy;

    // ===== Eye Position =====
    int eye_y  = cy - h * eye_offset_y_/100;
    int eye_dx = w * eye_offset_x_/100;
    int eye_r  = w * eye_radius_/100;
    int eye_t  = std::max(1, (int)(h * eye_thickness_/100));
          
    // ===== Mouse Position =====
    int mouth_y = cy + h * mouth_offset_y_/100;
    int mouth_w = std::max(1, (int)(w * mouth_width_/100));
    int mouth_t = std::max(1, (int)(h * mouth_thickness_/100));

    // Draw Eyes
    switch (face_expression_) {
    case FaceExpression::Sleepy:
        it.filled_rectangle(cx - eye_dx - 2*eye_r, eye_y, 4*eye_r, eye_t, fore_color_);
        it.filled_rectangle(cx + eye_dx - 2*eye_r, eye_y, 4*eye_r, eye_t, fore_color_);
        break;
    case FaceExpression::Angry:
        it.filled_circle(cx - eye_dx, eye_y, 2* eye_r, fore_color_);
        it.filled_circle(cx + eye_dx, eye_y, 2* eye_r, fore_color_);
        for (int i=0; i <= 3*eye_r; i++) {
            it.line(cx - eye_dx - 2*eye_r, eye_y - eye_r - i, cx - eye_dx + 2*eye_r, eye_y + eye_r - i, COLOR_OFF);
            it.line(cx + eye_dx - 2*eye_r, eye_y + eye_r - i, cx + eye_dx + 2*eye_r, eye_y - eye_r - i, COLOR_OFF);
        }
        break;
    case FaceExpression::Sad:
        it.filled_circle(cx - eye_dx, eye_y, 2* eye_r, fore_color_);
        it.filled_circle(cx + eye_dx, eye_y, 2* eye_r, fore_color_);
        for (int i=0; i <= 3*eye_r; i++) {
            it.line(cx - eye_dx + 2*eye_r, eye_y - eye_r - i, cx - eye_dx - 2*eye_r, eye_y + eye_r - i, COLOR_OFF);
            it.line(cx + eye_dx - 2*eye_r, eye_y - eye_r - i, cx + eye_dx + 2*eye_r, eye_y + eye_r - i, COLOR_OFF);
        }
        break;
    case FaceExpression::Happy:
        it.filled_circle(cx - eye_dx, eye_y, 2* eye_r, fore_color_);
        it.filled_circle(cx + eye_dx, eye_y, 2* eye_r, fore_color_);
        it.filled_circle(cx - eye_dx, eye_y, 2* eye_r - 2, COLOR_OFF);
        it.filled_circle(cx + eye_dx, eye_y, 2* eye_r - 2, COLOR_OFF);
        it.filled_rectangle(cx - eye_dx - 2*eye_r, eye_y + eye_t, 4*eye_r + eye_t, 2*eye_r, COLOR_OFF);
        it.filled_rectangle(cx + eye_dx - 2*eye_r, eye_y + eye_t, 4*eye_r + eye_t, 2*eye_r, COLOR_OFF);
        break;
    case FaceExpression::Doubt:
        it.filled_circle(cx - eye_dx, eye_y, 2* eye_r, fore_color_);
        it.filled_circle(cx + eye_dx, eye_y, 2* eye_r, fore_color_);
        it.filled_rectangle(cx - eye_dx - 2*eye_r, eye_y - 2*eye_r, 4*eye_r + 1, 2*eye_r, COLOR_OFF);
        it.filled_rectangle(cx + eye_dx - 2*eye_r, eye_y - 2*eye_r, 4*eye_r + 1, 2*eye_r, COLOR_OFF);
        break;
    default:
        if (!blinking_) {
            it.filled_circle(cx - eye_dx, eye_y, eye_r, fore_color_);
            it.filled_circle(cx + eye_dx, eye_y, eye_r, fore_color_);
        } else {
            int lw = std::max(2, eye_r / 2);
            it.filled_rectangle(cx - eye_dx - eye_r, eye_y - lw / 2, eye_r * 2,lw, fore_color_);
            it.filled_rectangle(cx + eye_dx - eye_r, eye_y - lw / 2, eye_r * 2,lw, fore_color_);          
        }
    }

    // Draw Mouse
    int open_h = mouth_t + h * mouse_level / 100;
    int open_w = mouth_t + mouth_w * (1.0 - mouse_level / 100);
    it.filled_rectangle(cx - open_w/2, mouth_y - open_h/2, open_w,open_h, fore_color_);
}

}  // namespace graph
}  // namespace esphome