#pragma once
#include <cstdint>
#include <utility>
#include <vector>
#include "esphome/core/color.h"
#include "esphome/core/component.h"

namespace esphome {

// forward declare Display
namespace display {
class Display;
class BaseFont;
}  // namespace display

namespace stack_chan {
    enum  FaceExpression { Neutral, Angry, Sleepy, Happy, Sad, Doubt };

    class StackChan;

    class StackChan : public Component {
        public:
            void setup() override;
            float get_setup_priority() const override { return setup_priority::PROCESSOR; }
            void dump_config() override;


            void draw(display::Display& buff);

            void set_fore_color(Color v) { fore_color_ = v; }
            void set_face_offset_x(int v) { face_offset_x_ = v; }
            void set_face_offset_y(int v) { face_offset_y_ = v; }
            void set_face_shake_duration(int v) { face_shake_duration_ = v; }
            void set_face_respiratory_rate(int v) { face_respiratory_rate_ = v; }
            void set_face_shake_x(int v) { face_shake_x_ = v; }
            void set_face_shake_y(int v) { face_shake_y_ = v; }
            void set_face_expression(FaceExpression v) { face_expression_ = v; }
            void set_eye_offset_x(int v) { eye_offset_x_ = v; }
            void set_eye_offset_y(int v) { eye_offset_y_ = v; }
            void set_eye_radius(float v) { eye_radius_ = v; }
            void set_eye_blink_interval(int v) { eye_blink_interval_ = v; }
            void set_eye_blink_duration(int v) { eye_blink_duration_ = v; }
            void set_mouth_offset_y(int v) { mouth_offset_y_ = v; }
            void set_mouth_width(int v) { mouth_width_ = v; }
            void set_mouth_thickness(float v) { mouth_thickness_ = v; }
            void set_mouth_open_ratio(int v) { mouth_open_ratio_ = v; }
            
        protected:
            Color fore_color_ = Color::WHITE;
            int face_offset_x_ = 0;
            int face_offset_y_ = 0;
            int face_shake_duration_ = 10;
            int face_respiratory_rate_ = 12;
            int face_shake_x_ = 6;
            int face_shake_y_ = 4;
            FaceExpression face_expression_ = FaceExpression::Neutral;
            int eye_offset_x_ = 30;
            int eye_offset_y_ = 12;
            float eye_radius_ = 3.2f;
            int eye_blink_interval_ = 5;
            int eye_blink_duration_ = 120;
            int mouth_offset_y_ = 15;
            int mouth_width_ = 30;
            float mouth_thickness_ = 2.0f;
            int mouth_open_ratio_ = 0;

        private:
            uint32_t blink_end_ = 0;
            uint32_t next_blink_ = 0;
            bool blinking_ = false;
    };

}  // namespace graph
}  // namespace esphome