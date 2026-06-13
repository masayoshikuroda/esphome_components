# ESPHome Custom Components

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
