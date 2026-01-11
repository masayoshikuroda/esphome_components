# ESPHome Custom Components

## StackChan

ESPHome版 StackChan です。
以下設定例です。

```yaml:stackchan.yaml
(前略)

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
          id(face)->set_fore_color(Color(255, 255, 0));
          id(face)->draw(it); 

```

口を開けてしゃべらせる場合
```yaml:stackchan.yaml
number:
  - platform: template
    id: mouth_open_ratio
    name: Mouth Open Ratio
    optimistic: true
    initial_value: 0
    min_value: 0
    max_value: 100
    step: 1
    unit_of_measurement: "%"
    set_action:
      - lambda: |-
          id(face).set_mouth_open_ratio(x);
```

顔の表情を変える場合
```yaml:stackchan.yaml
select:
  - platform: template
    id: face_expression
    name: "Face Expression"
    optimistic: true
    options:
      - Neutral
      - Angry
      - Sleepy
      - Happy
      - Sad
      - Doubt
    on_value:
      then:
        - lambda: |-
            using Expr = esphome::stack_chan::FaceExpression;
            if      (x == "Angry")  id(face)->set_face_expression(Expr::Angry);
            else if (x == "Sleepy") id(face)->set_face_expression(Expr::Sleepy);
            else if (x == "Happy")  id(face)->set_face_expression(Expr::Happy);
            else if (x == "Sad")    id(face)->set_face_expression(Expr::Sad);
            else if (x == "Doubt")  id(face)->set_face_expression(Expr::Doubt);
            else                    id(face)->set_face_expression(Expr::Neutral);
```