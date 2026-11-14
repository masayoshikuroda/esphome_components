#include "vu_meter.h"
#include "esphome/core/log.h"

namespace esphome {
namespace vu_meter {

static const char *const TAG = "vu_meter";

void VuMeter::setup() {
  if (this->source_ == nullptr) {
    ESP_LOGE(TAG, "No source speaker is configured!");
    this->mark_failed();
    return;
  }
}

void VuMeter::loop() {
  // Background SPL calculation and publish
  uint32_t now = millis();
  if (now - this->last_update_ >= this->update_interval_) {
    if (this->sample_count_ > 0) {
      float rms = std::sqrt(this->sum_squares_ / this->sample_count_);
      float spl = 0.0f;
      if (rms > 0.00001f) {
          spl = 20.0f * std::log10(rms) + this->spl_offset_;
      }
      this->publish_state(spl);
    }
    this->sum_squares_ = 0;
    this->sample_count_ = 0;
    this->last_update_ = now;
  }
}

#ifdef USE_ESP32
size_t VuMeter::play(const uint8_t *data, size_t length, TickType_t ticks_to_wait) {
  // Initial setup on first use (if audio stream info is not yet set on this object)
  if (!this->initialized_) {
    this->initialized_ = true;
    if (this->source_ != nullptr) {
      this->source_->set_audio_stream_info(this->get_audio_stream_info());
    }
  }
  
  // 1. First, pass the data to the real speaker with priority and get the actual number of bytes processed
  size_t written = this->source_->play(data, length, ticks_to_wait);

  // 2. Only compute the RMS using the exact range actually written (written)
  if (written >= 2) {
    // To handle a large amount of data at 48 kHz, downsample every 128 bytes (64 samples) and accumulate
    for (size_t i = 0; i < written - 1; i += 128) {
      int16_t sample = (int16_t)(data[i] | (data[i + 1] << 8));
      float normalized = (float)sample / 32768.0f;
      this->sum_squares_ += normalized * normalized;
      this->sample_count_++;
    }
  }

  // 3. Report the actual number of bytes the real speaker processed back upstream
  return written;
}
#endif

size_t VuMeter::play(const uint8_t *data, size_t length) {
#ifdef USE_ESP32
  return this->play(data, length, 0);
#else
  // Initial setup on first use (if audio stream info is not yet set on this object)
  if (!this->initialized_) {
    this->initialized_ = true;
    if (this->source_ != nullptr) {
      this->source_->set_audio_stream_info(this->get_audio_stream_info());
    }
  }
  
  // Fallback for non-ESP32 builds
  size_t written = this->source_->play(data, length);
  if (written >= 2) {
    for (size_t i = 0; i < written - 1; i += 128) {
      int16_t sample = (int16_t)(data[i] | (data[i + 1] << 8));
      float normalized = (float)sample / 32768.0f;
      this->sum_squares_ += normalized * normalized;
      this->sample_count_++;
    }
  }
  return written;
#endif
}

}  // namespace vu_meter
}  // namespace esphome