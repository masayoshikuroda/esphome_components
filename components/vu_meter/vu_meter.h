#pragma once

#include "esphome/core/component.h"
#include "esphome/components/speaker/speaker.h"
#include "esphome/components/sensor/sensor.h"
#include <cmath>
#include <vector>

namespace esphome {
namespace vu_meter {

class VuMeter : public Component, public speaker::Speaker, public sensor::Sensor {
 public:
  void setup() override;
  void loop() override;

  void set_source(speaker::Speaker *source) { this->source_ = source; }
  void set_update_interval(uint32_t v) { this->update_interval_ = v; }

  void set_audio_stream_info(const audio::AudioStreamInfo &audio_stream_info) {
    speaker::Speaker::set_audio_stream_info(audio_stream_info);
    if (this->source_ != nullptr) {
      this->source_->set_audio_stream_info(audio_stream_info);
    }
  }

#ifdef USE_ESP32
  size_t play(const uint8_t *data, size_t length, TickType_t ticks_to_wait) override;
#endif
  size_t play(const uint8_t *data, size_t length) override;
  
  void start() override {
    if (this->source_ != nullptr) {
      this->source_->set_audio_stream_info(this->get_audio_stream_info());
      this->source_->start();
    }
  }
  void stop() override { 
    // バッファが完全に出力されるまで待つ（最後の音声が途切れるのを防ぐ）
    if (this->source_ != nullptr) {
      uint32_t timeout = 5000;  // 5秒のタイムアウト
      uint32_t start = millis();
      while (this->source_->has_buffered_data() && (millis() - start) < timeout) {
        delay(10);
      }
      this->source_->stop();
    }
  }
  bool has_buffered_data() const override { 
    return this->source_ != nullptr && this->source_->has_buffered_data();
  }

 protected:
  speaker::Speaker *source_{nullptr};
  
  // 計算用に一時退避する軽量バッファ
  std::vector<int16_t> sample_buffer_;
  
  float sum_squares_ = 0;
  size_t sample_count_ = 0;
  uint32_t last_update_ = 0;
  uint32_t update_interval_ = 100;
  float spl_offset_ = 65.0f;
  
  // 初回play()呼び出しの初期化フラグ
  bool initialized_ = false;
};

}  // namespace vu_meter
}  // namespace esphome