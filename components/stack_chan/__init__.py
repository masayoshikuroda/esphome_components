import esphome.codegen as cg
from esphome.components import color
from esphome.components.font import Font
import esphome.config_validation as cv
from esphome.const import (
    CONF_BORDER,
    CONF_COLOR,
    CONF_CONTINUOUS,
    CONF_DIRECTION,
    CONF_DURATION,
    CONF_HEIGHT,
    CONF_ID,
    CONF_LEGEND,
    CONF_LINE_THICKNESS,
    CONF_LINE_TYPE,
    CONF_MAX_RANGE,
    CONF_MAX_VALUE,
    CONF_MIN_RANGE,
    CONF_MIN_VALUE,
    CONF_NAME,
    CONF_NAME_FONT,
    CONF_SENSOR,
    CONF_SHOW_LINES,
    CONF_SHOW_UNITS,
    CONF_SHOW_VALUES,
    CONF_TRACES,
    CONF_VALUE_FONT,
    CONF_WIDTH,
    CONF_X_GRID,
    CONF_Y_GRID,
)

CODEOWNERS = ["@masayoshikuroda"]

DEPENDENCIES = ["display"]
MULTI_CONF = True

CONF_FORE_COLOR = "fore_color"
CONF_FACE_OFFSET_X = "face_offset_x"
CONF_FACE_OFFSET_Y = "face_offset_y"
CONF_FACE_SHAKE_DURATION = "face_shake_duration"
CONF_FACE_RESPIRATORY_RATE = "face_respiratory_rate"
CONF_FACE_SHAKE_X = "face_shake_x"
CONF_FACE_SHAKE_Y = "face_shake_y"
CONF_FACE_EXPRESSION = "face_expression"
CONF_EYE_OFFSET_X = "eye_offset_x"
CONF_EYE_OFFSET_Y = "eye_offset_y"
CONF_EYE_RADIUS = "eye_radius"
CONF_EYE_BLINK_INTERVAL = "eye_blink_interval"
CONF_EYE_BLINK_DURATION = "eye_blink_duration"
CONF_MOUTH_OFFSET_Y = "mouse_offset_y"
CONF_MOUTH_WIDTH = "mouse_width"
CONF_MOUTH_THICKNESS = "mouse_thickness"
CONF_MOUTH_OPEN_RATIO = "mouse_open_ratio"

stack_chan_ns = cg.esphome_ns.namespace("stack_chan")
StackChan_ = stack_chan_ns.class_("StackChan", cg.Component)

FaceExpression = stack_chan_ns.enum("FaceExpression")

FACE_EXPRESSION_MAP = {
    "neutral": FaceExpression.Neutral,
    "angry":   FaceExpression.Angry,
    "sleepy":  FaceExpression.Sleepy,
    "happy":   FaceExpression.Happy,
    "sad":     FaceExpression.Sad,
    "doubt":   FaceExpression.Doubt,
}

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(StackChan_),
    cv.Optional(CONF_FACE_OFFSET_X, default=0): cv.int_,
    cv.Optional(CONF_FACE_OFFSET_Y, default=0): cv.int_,
    cv.Optional(CONF_FACE_SHAKE_DURATION, default=10): cv.int_,
    cv.Optional(CONF_FACE_RESPIRATORY_RATE, default=12): cv.int_,
    cv.Optional(CONF_FACE_SHAKE_X, default=6): cv.int_,
    cv.Optional(CONF_FACE_SHAKE_Y, default=6): cv.int_,
    cv.Optional(CONF_FACE_EXPRESSION, default="neutral"): cv.enum(FACE_EXPRESSION_MAP, lower=True),
    cv.Optional(CONF_EYE_OFFSET_X, default=30): cv.int_,
    cv.Optional(CONF_EYE_OFFSET_Y, default=12): cv.int_,
    cv.Optional(CONF_EYE_RADIUS, default=3.2): cv.float_,
    cv.Optional(CONF_EYE_BLINK_INTERVAL, default=5): cv.int_,
    cv.Optional(CONF_EYE_BLINK_DURATION, default=120): cv.int_,
    cv.Optional(CONF_MOUTH_OFFSET_Y, default= 15): cv.int_,
    cv.Optional(CONF_MOUTH_WIDTH, default=30): cv.int_,
    cv.Optional(CONF_MOUTH_THICKNESS, default=2.0): cv.float_,
    cv.Optional(CONF_MOUTH_OPEN_RATIO, default=0): cv.int_,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[cv.CONF_ID])
    await cg.register_component(var, config)
    
    cg.add(var.set_face_offset_x(config[CONF_FACE_OFFSET_X]))
    cg.add(var.set_face_offset_y(config[CONF_FACE_OFFSET_Y]))
    cg.add(var.set_face_shake_duration(config[CONF_FACE_SHAKE_DURATION]))
    cg.add(var.set_face_respiratory_rate(config[CONF_FACE_RESPIRATORY_RATE]))
    cg.add(var.set_face_shake_x(config[CONF_FACE_SHAKE_X]))
    cg.add(var.set_face_shake_y(config[CONF_FACE_SHAKE_Y]))
    cg.add(var.set_face_expression(config[CONF_FACE_EXPRESSION]))
    cg.add(var.set_eye_offset_x(config[CONF_EYE_OFFSET_X]))
    cg.add(var.set_eye_offset_y(config[CONF_EYE_OFFSET_Y]))
    cg.add(var.set_eye_radius(config[CONF_EYE_RADIUS]))
    cg.add(var.set_eye_blink_interval(config[CONF_EYE_BLINK_INTERVAL]))
    cg.add(var.set_eye_blink_duration(config[CONF_EYE_BLINK_DURATION]))
    cg.add(var.set_mouth_offset_y(config[CONF_MOUTH_OFFSET_Y]))
    cg.add(var.set_mouth_width(config[CONF_MOUTH_WIDTH]))
    cg.add(var.set_mouth_thickness(config[CONF_MOUTH_THICKNESS]))
    cg.add(var.set_mouth_open_ratio(config[CONF_MOUTH_OPEN_RATIO]))
