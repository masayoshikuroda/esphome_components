#include "speech_balloon.h"

#include "esphome/core/log.h"

namespace esphome {
namespace speech_balloon {

static const char *const TAG = "speech_balloon";

void SpeechBalloon::dump_config() {
  ESP_LOGCONFIG(TAG, "Speech Balloon:");
  ESP_LOGCONFIG(TAG, "  Left: %d", this->left_);
  ESP_LOGCONFIG(TAG, "  Top: %d", this->top_);
  ESP_LOGCONFIG(TAG, "  Tail X: %d", this->tail_x_);
  ESP_LOGCONFIG(TAG, "  Tail Y: %d", this->tail_y_);
  ESP_LOGCONFIG(TAG, "  Right: %d", this->right_);
  ESP_LOGCONFIG(TAG, "  Bottom: %d", this->bottom_);
  ESP_LOGCONFIG(TAG, "  Scroll speed: %d", this->scroll_speed_);
  ESP_LOGCONFIG(TAG, "  Message: %s", this->message_.c_str());
}

void SpeechBalloon::set_message(const std::string &message) {
  if (this->message_ == message) {
    return;
  }

  this->message_ = message;
  this->scroll_position_ = 0;
}

void SpeechBalloon::draw(display::Display &it) {
  if (this->font_ == nullptr) {
    ESP_LOGCONFIG(TAG, "NullPointerException: font");
    return;
  }

  if (this->message_.empty()) {
    return;
  }

  // calculate baloon text area max
  const int left   = this->left_   == -1 ? 2 * this->font_->get_height() : this->left_;
  const int right  = this->right_  == -1 ? it.get_width() - this->font_->get_height() / 2 : this->right_;
  const int top    = this->top_    == -1 ? it.get_height() - this->font_->get_height() : this->top_;
  const int bottom = this->bottom_ == -1 ? top + this->font_->get_height() : this->bottom_;
  const int width  = right - left;
  const int height = bottom - top;
  if (width <= 0 || height <= 0) {
    ESP_LOGCONFIG(TAG, "Empty text area: left=%d, top=%d, right=%d, bottom=%d", left, top, right, bottom);
    return;
  }
  const int center_x  = left + width  / 2;
  const int center_y  = top  + height / 2;

  // calculate baloon text area from text length
  int message_width, x_offset, baseline, message_height;
  this->font_->measure(this->message_.c_str(), &message_width, &x_offset, &baseline, &message_height);
  if (message_width <= 0 || message_height <= 0) {
    return; // message is empty!
  }
  const int message_bottom = center_y + message_height / 2;

  // draw baloon text　area
  const int balloon_width = message_width <= width ? message_width : width;
  const int balloon_left  = center_x - balloon_width / 2;
  const int balloon_right = center_x + balloon_width / 2;
  it.filled_rectangle(balloon_left, top, balloon_width, height, this->back_color_);
  
  // draw baloon text area corners
  const int radius = height / 2;
  it.filled_circle(balloon_left,  center_y, radius, this->back_color_);
  it.filled_circle(balloon_right, center_y, radius, this->back_color_);
  
  // draw baloon tail
  const int tail_x = this->tail_x_ == -1 ? center_x - radius    : this->tail_x_;
  const int tail_y = this->tail_y_ == -1 ? top - message_height : this->tail_y_;
  it.filled_triangle(tail_x, tail_y, center_x - radius, center_y, center_x + radius, center_y, this->back_color_);

  // draw baloon text
  it.start_clipping(left, top, right, bottom);
  if (message_width <= width) {
    it.printf(center_x, message_bottom, this->font_, this->fore_color_, display::TextAlign::BOTTOM_CENTER, this->message_.c_str());
  } else {
    const int left_x = left - this->scroll_position_;
    it.printf(left_x,   message_bottom, this->font_, this->fore_color_, display::TextAlign::BOTTOM_LEFT,   this->message_.c_str());

    this->scroll_position_ += this->scroll_speed_;
    if (this->scroll_position_ > message_width) {
      this->scroll_position_ = - message_width;
    }
  }
  it.end_clipping();
}

}  // namespace speech_balloon
}  // namespace esphome