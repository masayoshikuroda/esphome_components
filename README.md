# ESPHome Custom Components

## VU Meter

Microphoneの音圧レベルを取得する Sound Level Sensorコンポーネントのように、Speakerから出力される音圧レベルを計算するコンポーネントです。
Resamplerコンポーネントと同じように Speakerコンポネントとして振る舞い、入力の音圧レベルを計算後、物理スピーカーにそのまま出力します。
計算した音圧レベルは、Sensor経由で取得できます。

### 設定方法

```yaml:vumeter.yaml
(前略)

web_server:
  port: 8888

external_components:
  - source:
      type: git
      url: https://github.com/masayoshikuroda/esphome_components
      ref: main
    components: [ vu_meter ]

sensor:
  - platform: vu_meter
    id: speaker_vu_meter
    source: i2s_speaker
    name: "Speaker VU Meter"
    update_interval_ms: 100

media_player:
  - platform: speaker
    volume_min: 0.5
    volume_max: 0.8
    announcement_pipeline:
      speaker: speaker_vu_meter
      format: FLAC
      sample_rate: 48000
      num_channels: 1
```

## M5PM1

M5StickS3の電源管理 ICであるM5PM1用コンポーネントです。

### 設定方法

```yaml:stackchan.yaml
esphome:
  name: "m5stack-stick-s3"
  friendly_name: "M5Stack Stick S3"
  on_boot:
    - priority: 500 # This runs after the normal ES8311 setup.
      then:
        - lambda: |-
            id(m5pm1_id).set_speaker_amplifier(true);           
    - priority: 300
      then:
        - audio_dac.set_volume:
            volume: 80%
m5pm1:
  id: m5pm1_id
  i2c_id: i2c_internal
  address: 0x6E
```

## StackChan

ESPHome版 StackChan です。

### 設定方法

```yaml:stackchan.yaml
(前略)

packages:
  colors: !include common/colors.yaml
  fonts: !include common/fonts.yaml
  stackchan: !include common/stackchan.yaml

web_server:
  port: 8888

external_components:
  - source:
      type: git
      url: https://github.com/masayoshikuroda/esphome_components
      ref: main
    components: [ stack_chan ]

stack_chan:
  - id: face

display:
  - platform: mipi_spi
    (略)
    update_interval: 33ms
    pages:
      - id: page1
        lambda:  |-
          it.fill(id(face_backcolor));
          id(face)->set_fore_color(id(face_forecolor));
          id(face)->set_face_expression(id(face_expression));
          id(face).set_mouth_open_ratio(id(mouse_open_ratio));
          id(face)->draw(it); 

```

ブラウザ上で、スタックチャンの表情や口の大きさなどを変更することができます。

以下カスタム例です。

### マイクの音を拾って、口をぱくぱくする場合

マイク設定が予め必要となります。

```yaml:stackchan.yaml
sensor:
  - platform: sound_level
    passive: true
    measurement_duration: 66ms
    rms:
      name: "Microphone Average Loudness"
      filters:
        - offset: 65.0
      on_value:
        then:
          - lambda: |-
              float QUIET_LEVEL = 32.0f;
              float LOUD_LEVEL  = 120.0f;
              float current_db = x;

              int ratio = 0;
              if (current_db < QUIET_LEVEL) {
                ratio = 0;
              } else if(current_db > LOUD_LEVEL) {
                ratio = 100;
              } else {
                ratio = (int)((current_db - QUIET_LEVEL) / (LOUD_LEVEL - QUIET_LEVEL) * 100.0f);
              }
              
              id(mouse_open_ratio) = ratio;
```

### スピーカの音にあわせて、口をぱくぱくする場合

```yaml:stackchan.yaml
sensor:
  - platform: vu_meter
    id: speaker_vu_meter
    source: i2s_speaker
    name: "Speaker VU Meter"
    update_interval_ms: 100
    on_value:
      then:
        - lambda: |-
            float QUIET_LEVEL = 32.0f;
            float LOUD_LEVEL  = 60.0f;
            float current_db = x;

            int ratio = 0;
            if (current_db < QUIET_LEVEL) {
              ratio = 0;
            } else if(current_db > LOUD_LEVEL) {
              ratio = 100;
            } else {
              ratio = (int)((current_db - QUIET_LEVEL) / (LOUD_LEVEL - QUIET_LEVEL) * 100.0f);
            }
              
            id(mouse_open_ratio) = ratio;
```

### あくびをさせる場合

```yaml:stackchan.yaml
interval:
  - interval: 33ms
    then:
      - lambda: |-
          float time = millis() / 1000.0;
          float cycle_duration = 10.0; // 10-second cycle
          float PI = 3.141593;
          float min_value = 0.0;
          float max_value = 50.0;
          float base_value = min_value + 0.5 * (max_value - min_value) * (1 + sin(2 * PI * time / cycle_duration));
          int ratio = (int)base_value;
          id(face).set_mouth_open_ratio(ratio);
```
### 漫符の表示

絵文字を使って、画面右上に表示するもが簡単。
絵文字が含まれるフォントをあらかじめロードさせておくことが必要。
displayのlambda式中で、face描画後に記述する。

```yaml:stackchan.yaml
・・・
packages:
  ・・・
  fonts-emoji: github://masayoshikuroda/esphome_components/common/fonts-emoji.yaml@main
  stackchan-decoration: github://masayoshikuroda/esphome_components/common/stackchan-decoration.yaml@main
・・・
display:
    pages:
      - id: stackchan
        lambda: |-
          ・・・
        　id(face)->draw(it);
          it.print(it.get_width(), 0, id(font_40_emoji), id(face_forecolor), TextAlign::TOP_RIGHT, "❤️");
```
### 吹き出しの表示

画面下部に直接文字列を描画する。
displayのlambda式中で、face描画後に記述する。

```yaml:stackchan.yaml
・・・
packages:
  ・・・
  fonts: github://masayoshikuroda/esphome_components/common/fonts.yaml@main
  fonts-ja: github://masayoshikuroda/esphome_components/common/fonts-ja.yaml@main
・・・
display:
    pages:
      - id: stackchan
        lambda: |-
          ・・・
        　id(face)->draw(it);
          it.printf(it.get_width()/2, it.get_height(), id(font_20_gothic), id(face_forecolor), TextAlign::BOTTOM_CENTER, "Hello World!");
```

## StackChan Voice Assistant

Voice Assisttantコンポーネントを設定し、Home Assistant上のassist機能を利用することで
StackChanをAIアシスタントとして動作させます。

定義済みのpackageを利用することでyaml設定を最小限にして構成することができます。

```yaml:stackchan.yaml
・・・
packages:
  ・・・
  colors:               github://masayoshikuroda/esphome_components/common/colors.yaml@main
  fonts:                github://masayoshikuroda/esphome_components/common/fonts.yaml@main
  fonts-ja:             github://masayoshikuroda/esphome_components/common/fonts-ja.yaml@main
  fonts-emoji:          github://masayoshikuroda/esphome_components/common/fonts-emoji.yaml@main
  stackchan:            github://masayoshikuroda/esphome_components/common/stackchan.yaml@main
  stackchan-decoration: github://masayoshikuroda/esphome_components/common/stackchan-decoration.yaml@main
  stackchan-baloon:     github://masayoshikuroda/esphome_components/common/stackchan-baloon.yaml@main
  stackchan-audio:      github://masayoshikuroda/esphome_components/common/stackchan-audio.yaml@main
  stackchan-voice:      github://masayoshikuroda/esphome_components/common/stackchan-voice.yaml@main
・・・
microphone:
  - platform: i2s_audio
    id: i2s_mic
・・・
speaker:
  - platform: i2s_audio
    id: i2s_speaker
・・・    
display:
　　・・・    
    pages:
      - id: stackchan
        lambda: |-
          it.fill(id(face_backcolor));
          id(face)->set_back_color(id(face_backcolor));
          id(face)->set_fore_color(id(face_forecolor));
          id(face)->set_face_expression(id(face_expression));
          id(face).set_mouth_open_ratio(id(mouse_open_ratio));
          id(face)->draw(it);
          it.print(it.get_width(), 0, id(font_40_emoji), id(face_forecolor), TextAlign::TOP_RIGHT, id(face_decoration).c_str());
          it.printf(it.get_width()/2, it.get_height(), id(font_20_gothic), id(face_forecolor), TextAlign::BOTTOM_CENTER, id(face_message).c_str());

script:
  - id: on_client_connected
    then:
     - lambda: |-
        id(face_decoration) = "";
        id(face_message) = "CONNECTED"; 
  - id: on_client_disconnected
    then:
     - lambda: |-
        id(face_decoration) = "";
        id(face_message) = "DISCONNECTED";   
  - id: on_wake_word_start
    then:
      - lambda: |-
          id(face_decoration) = "";
          id(face_message) = "DETECTING";
  - id: on_wake_word_stop
    then:
      - lambda: |-
          id(face_decoration) = "";
          id(face_message) = "";
  - id: on_wake_word_detected
    then:
      - lambda: |-
          id(face_decoration) = "❗️";
          id(face_message) = "DETECTED";
  - id: on_voice_assistant_start
    then:
     - lambda: |-
        id(face_decoration) = "";
        id(face_message) = "STARTED";
  - id: on_voice_assistant_listening
    then:
    - lambda: |-
        id(face_decoration) = "👂";
        id(face_message) = "LISTENING";
  - id: on_voice_assistant_transcripted
    then:
    - lambda: |-
        id(face_decoration) = "📝";
        id(face_message) = "TRANSCRIPTED";
  - id: on_voice_assistant_intent_start
    then:
    - lambda: |-
        id(face_decoration) = "🌐";
        id(face_message) = "THINKING";
  - id: on_voice_assistant_speaking
    then:
    - lambda: |-
        id(face_decoration) = "📢";
        id(face_message) = "SPEAKING";
  - id: on_voice_assistant_error
    then:
    - lambda: |-
        id(face_decoration) = "❌";
        id(face_message) = "ERROR";
  - id: on_voice_assistant_end
    then:

web_server:
  port: 8888
```

- microphone, speakerコンポーネント定義が必要です
- デバイス起動時に micro_wake_word が開始されます
- ウェイクワードは "Okay Nabu"
- voice_assistant の status が更新されると、対応する script が呼び出されます
- web_server 上で設定、動作を変更できます