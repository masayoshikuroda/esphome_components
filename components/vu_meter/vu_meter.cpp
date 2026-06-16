#include "vu_meter.h"
#include "esphome/core/log.h"

namespace esphome {
namespace vu_meter {

static const char *const TAG = "vu_meter";

void VuMeter::setup() {
  if (this->source_ == nullptr) {
    ESP_LOGE(TAG, "Source speaker が設定されていません！");
    this->mark_failed();
    return;
  }
}

void VuMeter::loop() {
  // バックグラウンドでの音圧計算とpublish
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
    this->last_update_ = now; // 💡 修正：末尾のアンダースコアを追加しました
  }
}

size_t VuMeter::play(const uint8_t *data, size_t length) {
  // 1. まず最優先で本物のスピーカーにデータを流し、実際に処理された「本物のバイト数」を取得
  size_t written = this->source_->play(data, length);

  // 2. 厳密に「実際に書き込まれた範囲内（written）」だけで計算を行う
  if (written >= 2) {
    // 48kHzという膨大なデータをさばくため、128バイト（64サンプル）ごとに間引いて足し算
    for (size_t i = 0; i < written - 1; i += 128) {
      int16_t sample = (int16_t)(data[i] | (data[i + 1] << 8));
      float normalized = (float)sample / 32768.0f;
      this->sum_squares_ += normalized * normalized;
      this->sample_count_++;
    }
  }

  // 3. 本物のスピーカーが「処理できたバイト数」をそのまま上流に正直に報告する
  return written;
}

}  // namespace vu_meter
}  // namespace esphome