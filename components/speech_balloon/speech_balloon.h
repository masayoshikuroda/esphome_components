#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display.h"
#include "esphome/components/font/font.h"
#include "esphome/core/color.h"

#include <string>

namespace esphome {
namespace speech_balloon {

class SpeechBalloon : public Component {
 public:
  void setup() override {}
  void loop() override {}
  void dump_config() override;

  void draw(display::Display &it);

  void set_font(font::Font *font) { this->font_ = font; }
  font::Font *get_font() const { return this->font_; }

  void set_left(int left) { this->left_ = left; }
  int get_left() const { return this->left_; }
  void set_top(int top) { this->top_ = top; }
  int get_top() const { return this->top_; }
  void set_right(int right) { this->right_ = right; }
  int get_right() const { return this->right_; }
  void set_bottom(int bottom) { this->bottom_ = bottom; }
  int get_bottom() const { return this->bottom_; }

  void set_tail_x(int tail_x) { this->tail_x_ = tail_x; }
  int get_tail_x() const { return this->tail_x_; }
  void set_tail_y(int tail_y) { this->tail_y_ = tail_y; }
  int get_tail_y() const { return this->tail_y_; }

  void set_foreground_color(Color color) { this->foreground_color_ = color; }
  Color get_foreground_color() const { return this->foreground_color_; }
  void set_background_color(Color color) { this->background_color_ = color; }
  Color get_background_color() const { return this->background_color_; }

  void set_message(const std::string &message);
  const std::string &get_message() const { return this->message_; }

  void set_scroll_speed(int speed) { this->scroll_speed_ = speed; }
  int get_scroll_speed() const { return this->scroll_speed_; }

 protected:
  font::Font *font_{nullptr};
  
  int left_{-1};
  int top_{-1};
  int right_{-1};
  int bottom_{-1};

  int tail_x_{0};
  int tail_y_{-1};
  
  Color foreground_color_{0, 0, 0};
  Color background_color_{255, 255, 255};

  std::string message_;

  int scroll_speed_{5};
  int scroll_position_{0};
};

}  // namespace speech_balloon
}  // namespace esphome