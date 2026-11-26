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
  ESP_LOGCONFIG(TAG, "  Width: %d", this->width_);
  ESP_LOGCONFIG(TAG, "  Height: %d", this->height_);
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

  if (this->width_ <= 0 || this->height_ <= 0) {
    ESP_LOGCONFIG(TAG, "Empty text area: width=%d, height=%d", this->width_, this->height_);
    return;
  }

  int message_width, x_offset, baseline, message_height;
  this->font_->measure(this->message_.c_str(), &message_width, &x_offset, &baseline, &message_height);
  if (message_width <= 0 || message_height <= 0) {
    return;
  }

  const int left = this->left_;
  const int right = left + this->width_;
  const int top = this->top_;
  const int bottom = top + this->height_;

  const int center_x  = left + this->width_ / 2;
  const int center_y  = top  + this->height_ / 2;
  const int message_bottom = center_y + message_height / 2;

  // draw baloon
  const int balloon_width = message_width <= this->width_ ? message_width : this->width_;
  const int balloon_left  = center_x - balloon_width / 2;
  const int balloon_right = center_x + balloon_width / 2;
  it.filled_rectangle(balloon_left, top, balloon_width, this->height_, this->background_color_);
  const int radius = this->height_ / 2;
  it.filled_circle(balloon_left,  center_y, radius, this->background_color_);
  it.filled_circle(balloon_right, center_y, radius, this->background_color_);
  it.filled_triangle(this->tail_x_, this->tail_y_, center_x - radius, center_y, center_x + radius, center_y, this->background_color_);

  // draw message
  it.start_clipping(left, top, right, bottom);
  if (message_width <= this->width_) {
    it.printf(center_x,  message_bottom, this->font_, this->foreground_color_, display::TextAlign::BOTTOM_CENTER, this->message_.c_str());
  } else {
    const int left_x = this->left_ - this->scroll_position_;
    it.printf(left_x,    message_bottom, this->font_, this->foreground_color_, display::TextAlign::BOTTOM_LEFT,   this->message_.c_str());

    this->scroll_position_ += this->scroll_speed_;
    if (this->scroll_position_ > message_width) {
      this->scroll_position_ = - message_width;
    }
  }
  it.end_clipping();
}

}  // namespace speech_balloon
}  // namespace esphome